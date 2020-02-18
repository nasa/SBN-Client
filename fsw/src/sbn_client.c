#include <pthread.h>

#include "sbn_pack.h"
#include "sbn_client.h"
#include "sbn_client_utils.h"

/* Global variables */
pthread_mutex_t receive_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  received_condition = PTHREAD_COND_INITIALIZER;
CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
MsgId_to_pipes_t MsgId_Subscriptions[CFE_SBN_CLIENT_MSG_ID_TO_PIPE_ID_MAP_SIZE];
int sbn_client_sockfd = 0;
int sbn_client_cpuId = 0;
// TODO: Our use of sockfd is not uniform. Should pass to each function XOR use as global
// TODO: sbn_client_cpuId does not need to live here; perhaps it should go elsewhere


void CFE_SBN_Client_InitPipeTbl(void)
{
    uint8  i;

    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++){
        invalidate_pipe(&PipeTbl[i]);
    }/* end for */
    
    
}

CFE_SB_PipeId_t CFE_SBN_Client_GetAvailPipeIdx(void)
{
    uint8 i;

    /* search for next available pipe entry */
    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
    {
        if(PipeTbl[i].InUse == CFE_SBN_CLIENT_NOT_IN_USE){
            return i;
        }/* end if */

    }/* end for */

    return CFE_SBN_CLIENT_INVALID_PIPE;
}

/* NOTE: Using memcpy to move message into pipe. What about pointer passing?
 *    Can we only look to msgId then memcpy only that then read directly
 *    into pipe? This could speed things up... */
void ingest_app_message(int sockfd, SBN_MsgSz_t MsgSz)
{
    int status;
    unsigned char msg_buffer[CFE_SB_MAX_SB_MSG_SIZE];
    CFE_SB_MsgId_t MsgId;
    
    status = CFE_SBN_CLIENT_ReadBytes(sockfd, msg_buffer, MsgSz);
    
    if (status != CFE_SUCCESS)
    {
      printf("CFE_SBN_CLIENT_ReadBytes returned a bad status = %d\n", status);
    }

    MsgId = CFE_SBN_Client_GetMsgId((CFE_SB_MsgPtr_t)msg_buffer);
    
    /* TODO: check that msgid is valid - How? */
    
    /* Take mutex */
    pthread_mutex_lock(&receive_mutex);
    //puts("\n\nRECEIVED LOCK!\n\n");
    
    /*Put message into pipe */
    
    int i;
    
    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
    {    
        if (PipeTbl[i].InUse == CFE_SBN_CLIENT_IN_USE)
        {
            int j;
            
            for(j = 0; j < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; j++)
            {
                if (PipeTbl[i].SubscribedMsgIds[j] == MsgId)
                {
                    
                    if (PipeTbl[i].NumberOfMessages == 
                        CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
                    {
                        //TODO: error pipe overflow
                        puts("SBN_CLIENT: ERROR pipe overflow");
                        
                        pthread_mutex_unlock(&receive_mutex);
                        return;
                    }
                    else /* message is put into pipe */
                    {    
                        puts("message received");
                        memcpy(PipeTbl[i].Messages[message_entry_point(
                            PipeTbl[i])], msg_buffer, MsgSz);
                        PipeTbl[i].NumberOfMessages++;
                        
                        pthread_mutex_unlock(&receive_mutex);
                        pthread_cond_signal(&received_condition); /* only a received message should send signal */
                        return;
                    } /* end if */
                    
                }/* end if */
                 
            } /* end for */
            
        } /* end if */
    
    } /* end for */
    
    puts("SBN_CLIENT: ERROR no subscription for this msgid");  
    pthread_mutex_unlock(&receive_mutex);
    return;
}

/**
 * \brief Sends a local subscription over the wire to a peer.
 *
 * @param[in] SubType Whether this is a subscription or unsubscription.
 * @param[in] MsgID The CCSDS message ID being (un)subscribed.
 * @param[in] QoS The CCSDS quality of service being (un)subscribed.
 * @param[in] Peer The Peer interface
 */
void SendSubToSbn(int SubType, CFE_SB_MsgId_t MsgID,
    CFE_SB_Qos_t QoS)
{
    char Buf[SBN_PACKED_SUB_SZ] = {0};
    Pack_t Pack;
    Pack_Init(&Pack, Buf, SBN_PACKED_SUB_SZ, 0);
    Pack_UInt16(&Pack, 54);
    Pack_UInt8(&Pack, SubType);
    Pack_UInt32(&Pack, 2);
    Pack_Data(&Pack, SBN_IDENT, SBN_IDENT_LEN);
    Pack_UInt16(&Pack, 1);

    Pack_MsgID(&Pack, MsgID);
    Pack_Data(&Pack, &QoS, sizeof(QoS)); /* 2 uint8's */
    
    // int i = 0;
    // puts("SUB MSG: ");
    // for (i;i < Pack.BufUsed; i++)
    // {
    //   printf("0x%02x ", (unsigned char)Buf[i]);
    // }
    // printf("i = %d\n", i);
    // puts("");
    
    size_t write_result = write_message(sbn_client_sockfd, Buf, Pack.BufUsed);
    
    if (write_result != Pack.BufUsed)
    {
      //TODO: error
      puts("SBN_CLIENT: ERROR SendSubToSbn!!\n");
    }
    
}/* end SendLocalSubToPeer */


int32 recv_msg(int32 sockfd)
{
    unsigned char sbn_hdr_buffer[SBN_PACKED_HDR_SZ];
    unsigned char msg[CFE_SB_MAX_SB_MSG_SIZE];
    SBN_MsgSz_t MsgSz;
    SBN_MsgType_t MsgType;
    SBN_CpuID_t CpuID;
    
    int status = CFE_SBN_CLIENT_ReadBytes(sockfd, sbn_hdr_buffer, 
                                          SBN_PACKED_HDR_SZ);
    
    if (status != CFE_SUCCESS)
    {
        printf("SBN_CLIENT: recv_msg call to CFE_SBN_CLIENT_ReadBytes returned" 
               "status = %d\n", status);
    }
    else
    {
        Unpack_t Unpack;
        Unpack_Init(&Unpack, sbn_hdr_buffer, SBN_PACKED_HDR_SZ);
        Unpack_UInt16(&Unpack, &MsgSz);
        Unpack_UInt8(&Unpack, &MsgType);
        Unpack_UInt32(&Unpack, &CpuID);

        printf("Msg Size: %d\t Msg Type: %d\t Processor_ID: %d\n", MsgSz, 
               MsgType, CpuID);

        //TODO: check cpuID to see if it is correct for this location?

        switch(MsgType)
        {
            case SBN_NO_MSG:
                status = CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
                break;
            case SBN_SUB_MSG:
                status = CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
                break;
            case SBN_UNSUB_MSG:
                status = CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
                break;
            case SBN_APP_MSG:
                ingest_app_message(sockfd, MsgSz);
                status = CFE_SUCCESS;
                break;
            case SBN_PROTO_MSG:      
                status = CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
                break;
            case SBN_RECVD_HEARTBEAT_MSG:
                status = CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
                break;

            default:
                printf("SBN_CLIENT: ERROR - recv_msg unrecognized type %d\n", 
                       MsgType);
                status =  CFE_EVS_ERROR; //TODO: change error
        }
        
    }
    
    return status;
}

