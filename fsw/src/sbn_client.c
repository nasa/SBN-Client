#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "cfe_platform_cfg.h"
#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "sbn_pack.h"
#include "sbn_client.h"

#define  CFE_SBN_CLIENT_NO_PROTOCOL   0

// TODO: can this be included instead of duplicated here?
#define CCSDS_TIME_SIZE 6 // <- see mps_mission_cfg.h

// Refer to sbn_cont_tbl.c to make sure these match
// SBN is running here:
#define PORT    1234
#define IP_ADDR "127.0.0.1"

// Peer (us) is running here:
#define PEER_PORT    1235
#define PEER_IP_ADDR "127.0.0.1"

// Private functions
int32 SBN_ClientInit(void);
int connect_to_server(const char *server_ip, uint16_t server_port);
int send_msg(int sockfd, CFE_SB_Msg_t *msg);
int send_heartbeat(int sockfd);
int recv_msg(int sockfd);
///////////////////////


// TODO: Our use of sockfd is not uniform. Should pass to each function XOR use as global
int sockfd = 0;
int cpuId = 0;
struct sockaddr_in server_address;

void *heartbeatMinder(void *vargp);
pthread_t heart_thread_id;
void *receiveMinder(void *vargp);
pthread_t receive_thread_id;
CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
MsgId_to_pipes_t MsgId_Subscriptions[CFE_SBN_CLIENT_MSG_ID_TO_PIPE_ID_MAP_SIZE];

// message_entry_point determines which slot a new message enters the pipe.
// the mod allows it to go around the bend easily, i.e. 2 + 4 % 5 = 1, 
// slots 2,3,4,0 are taken so 1 is entry
int message_entry_point(CFE_SBN_Client_PipeD_t pipe)
{
    return (pipe.NextMessage + pipe.NumberOfMessages) % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
}

int CFE_SBN_CLIENT_ReadBytes(int sockfd, unsigned char *msg_buffer, size_t MsgSz)
{
    int bytes_received = 0;
    int total_bytes_recd = 0;
    
    //TODO:Some kind of timeout on this.
    while (total_bytes_recd != MsgSz)
    {
        bytes_received = read(sockfd, msg_buffer + total_bytes_recd, MsgSz - total_bytes_recd);
        
        if (bytes_received < 0)
        {
            //TODO:ERROR socket is dead somehow        
            puts("BAD!!!! CFE_SBN_CLIENT_PIPE_BROKEN_ERR\n");
            return CFE_SBN_CLIENT_PIPE_BROKEN_ERR;
        }
        else if (bytes_received == 0)
        {
            //TODO:ERROR closed remotely 
            puts("BAD!!!! CFE_SBN_CLIENT_PIPE_CLOSED_ERR\n");
            return CFE_SBN_CLIENT_PIPE_CLOSED_ERR;
        }
        
        total_bytes_recd += bytes_received;
    }
    
    printf("SBN_Client: Received: %d\n", total_bytes_recd);
}


void CFE_SBN_Client_InitPipeTbl(void)
{
    puts("CFE_SBN_Client_InitPipeTbl");
    uint8  i;

    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++){
        InvalidatePipe(&PipeTbl[i]);
    }/* end for */
    
}

void InvalidatePipe(CFE_SBN_Client_PipeD_t *pipe)
{
    int i;
    
    pipe->InUse         = CFE_SBN_CLIENT_NOT_IN_USE;
    pipe->SysQueueId    = CFE_SBN_CLIENT_UNUSED_QUEUE;
    pipe->PipeId        = CFE_SBN_CLIENT_INVALID_PIPE;
    memset(&pipe->PipeName[0],0,OS_MAX_API_NAME);
    
    for(i = 0; i < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; i++)
    {
        pipe->SubscribedMsgIds[i] = CFE_SBN_CLIENT_INVALID_MSG_ID;
    }
    
    
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

//NOTE:using memcpy to move message into pipe. What about pointer passing?
//    :can we only look to msgId then memcpy only that then read directly
//    : into pipe? This could speed things up...
void ingest_app_message(int sockfd, SBN_MsgSz_t MsgSz)
{
    int bytes_received = 0;
    int total_bytes_recd = 0;
    char msg_buffer[CFE_SB_MAX_SB_MSG_SIZE];
    CFE_SB_MsgId_t MsgId;
    
    int status = CFE_SBN_CLIENT_ReadBytes(sockfd, msg_buffer, MsgSz);
    
    //TODO: Status check goes here

    MsgId = CFE_SB_GetMsgId(msg_buffer);
    
    //TODO: check that msgid is valid
    
    // put message into pipe
    
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
                    if (PipeTbl[i].NumberOfMessages == CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
                    {
                        //TODO: error pipe overflow
                        return;
                    }
                    else
                    {    
                        memcpy(PipeTbl[i].Messages[message_entry_point(PipeTbl[i])], msg_buffer, MsgSz);
                        PipeTbl[i].NumberOfMessages++;
                        return;
                    } /* end if */
                    
                }/* end if */
                 
            } /* end for */
            //TODO: error no subscription for this msgid
        
        } /* end if */
    
    } /* end for */
    //TODO: error no pipes in use
}

    
uint8 CFE_SBN_Client_GetPipeIdx(CFE_SB_PipeId_t PipeId)
{
  // Quick check because PipeId should match PipeIdx
    if (PipeTbl[PipeId].PipeId == PipeId && PipeTbl[PipeId].InUse == CFE_SBN_CLIENT_IN_USE)
    {
        return PipeId;
    }
    else
    {
        int i;
    
        for(i=0;i<CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;i++)
        {

            if(PipeTbl[i].PipeId == PipeId && PipeTbl[i].InUse == CFE_SBN_CLIENT_IN_USE)
            {
                return i;
            }/* end if */

        } /* end for */
    
        // Pipe ID not found. TODO: error event? No, lets have caller do that...
        return CFE_SBN_CLIENT_INVALID_PIPE;
    }/* end if */
  
}/* end CFE_SBN_Client_GetPipeIdx */

uint8 CFE_SBN_Client_GetMessageSubscribeIndex(CFE_SB_PipeId_t PipeId)
{
    int i;
    
    for (i = 0; i < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; i++)
    {
        if (PipeTbl[PipeId].SubscribedMsgIds[i] == CFE_SBN_CLIENT_INVALID_MSG_ID)
        {
            return i;
        }
    }
    
    return CFE_SBN_CLIENT_MAX_MSG_IDS_MET;
}

int32 SBN_ClientInit(void)
{
    // Gets socket file descriptor
    sockfd = connect_to_server(IP_ADDR, PORT);
    cpuId = 2;

    if (sockfd < 0)
    {
        printf("SBN_client: Failed to get sockfd, error %d\n", sockfd);
        exit(sockfd);
    }
    
    // Create pipe table
    CFE_SBN_Client_InitPipeTbl();

    // Receive a message or something?
    recv_msg(sockfd);
    recv_msg(sockfd);

    // Thread for watchdog?
    pthread_create(&heart_thread_id, NULL, heartbeatMinder, NULL);
    pthread_create(&receive_thread_id, NULL, receiveMinder, NULL);

    // TODO: is thread ever cleaned up?
    // pthread_join(thread_id, NULL);

    // TODO: return failure?
    return OS_SUCCESS;
}

int connect_to_server(const char *server_ip, uint16_t server_port)
{
    int address_converted, connection;

    // Create an ipv4 TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, CFE_SBN_CLIENT_NO_PROTOCOL);

    // socket error
    if (sockfd < 0)
    {
        return -1;
    }

    memset(&server_address, '0', sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    address_converted = inet_pton(AF_INET, server_ip, &server_address.sin_addr);

    // inet_pton can have two separate errors, a value of 1 is success.
    if (address_converted == 0)
    {
        return -2;
    }

    if (address_converted == -1)
    {
        return -3;
    }

    connection = connect(sockfd, (struct sockaddr *)&server_address,
                         sizeof(server_address));

    // connect error
    if (connection < 0)
    {
        return -4;
    }

    return sockfd;
}


// Deal with sending out heartbeat messages
#define SBN_TCP_HEARTBEAT_MSG 0xA0
void *heartbeatMinder(void *vargp)
{
    while(1) // TODO: check run state?
    {
        sleep(3);
        if (sockfd != 0)
        {
            printf("SBN_Client: Sending heartbeat\n");
            send_heartbeat(sockfd);
        }
    }
    return NULL;
}

// TODO: return value?
int send_heartbeat(int sockfd)
{
    printf("SBN_Client: Sending a heartbeat\n");

    int retval = 0;
    char sbn_header[SBN_PACKED_HDR_SZ] = {0};

    Pack_t Pack;
    Pack_Init(&Pack, sbn_header, 0 + SBN_PACKED_HDR_SZ, 0);

    Pack_UInt16(&Pack, 0);
    Pack_UInt8(&Pack, SBN_TCP_HEARTBEAT_MSG);
    Pack_UInt32(&Pack, 2);

    retval = write(sockfd, sbn_header, sizeof(sbn_header));

    printf("SBN_Client: Did the send work? %d\n", retval);
}


// Pipe creation / subscription

int32 __wrap_CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, const char *PipeName)
{
    printf("SBN_Client: CreatingPipe\n");
    
    /* AppId is static for now */
    
    /* caller name is static for now */
    
    /* name will not require NULL terminator */
    
    /* TODO: determine if semaphore is necessary */
    
    /* TODO: determine if taskId is necessary */
    
    /* set user's pipe id value to 'invalid' for error cases below */
    if(PipeIdPtr != NULL){
        *PipeIdPtr = CFE_SBN_CLIENT_INVALID_PIPE;
    }/* end if */
    
    /* check input parameters */
    if((PipeIdPtr == NULL)||(Depth > CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)||(Depth == 0))
    {
        // TODO: what does this do? CFE_SB.HKTlmMsg.Payload.CreatePipeErrorCounter++;
        // TODO: only if semaphore is necessary! CFE_SB_UnlockSharedData(__func__,__LINE__);
        /* replaced by send event below! CFE_EVS_SendEvent(CFE_SB_CR_PIPE_BAD_ARG_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "CreatePipeErr:Bad Input Arg:app=%s,ptr=0x%lx,depth=%d,maxdepth=%d",
                CFE_SB_GetAppTskName(TskId,FullName),(unsigned long)PipeIdPtr,(int)Depth,CFE_PLATFORM_SB_MAX_PIPE_DEPTH);
            */    
        CFE_EVS_SendEvent(CFE_SBN_CLIENT_CR_PIPE_ERR_EID,CFE_EVS_EventType_ERROR,
          "CreatePipeErr:Bad Input Arg:app=%s,ptr=0x%lx,depth=%d,maxdepth=%d",
          APP_NAME,(unsigned long)PipeIdPtr,(int)Depth,
          CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH);
        return CFE_SBN_CLIENT_BAD_ARGUMENT;
    }/*end if*/
    
      uint8 i;
    
    for(i=0;i<CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;i++)
    {
      
      if (PipeTbl[i].InUse != CFE_SBN_CLIENT_IN_USE)
      {
        // TODO:Initialize pipe
        PipeTbl[i].InUse = CFE_SBN_CLIENT_IN_USE;
        //PipeTbl[i].SysQueueId = ?
        PipeTbl[i].PipeId = i;
        //PipeTbl[i].QueueDepth = ?
        //PipeTbl[i].AppId = ?
        PipeTbl[i].SendErrors = 0;
        //strcpy(&CFE_SB.PipeTbl[PipeTblIdx].AppName[0],&AppName[0]); TODO: is App name required? will cfs proxy handle it?
        strncpy(&PipeTbl[i].PipeName[0], PipeName, OS_MAX_API_NAME); //TODO: Use different value for size?
        PipeTbl[i].NumberOfMessages = 0;
        PipeTbl[i].NextMessage = 0;
        //TODO: init Messages to empty?
        
        *PipeIdPtr = i;
        
        return CFE_SUCCESS;
      }
    }
        
    /* if pipe table is full, send event and return error */
    CFE_EVS_SendEvent(CFE_SBN_CLIENT_MAX_PIPES_MET_EID,CFE_EVS_EventType_ERROR,
      "CreatePipeErr:Max Pipes(%d)In Use.app %s",
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPES, APP_NAME);
    
    
    
    return CFE_SBN_CLIENT_MAX_PIPES_MET;
}

int32 __wrap_CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
  
    uint8 i;

    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
    {
        if (PipeTbl[i].PipeId == PipeId)
        {
            if (PipeTbl[i].InUse == CFE_SBN_CLIENT_IN_USE)
            {
                InvalidatePipe(&PipeTbl[i]);
                CFE_EVS_SendEvent(CFE_SBN_CLIENT_PIPE_DELETED_EID, 
                CFE_EVS_EventType_DEBUG, "Pipe Deleted:id %d,owner %s",
                (int)PipeId, APP_NAME);
                return CFE_SUCCESS;
            }
            else
            {
                //TODO:error
            }
            
        }
        
    }
    
    //TODO: if we get here no pipes matched, error
    
    
}

int32 __wrap_CFE_SB_Subscribe(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
    uint8 PipeIdx;
    uint8 MsgIdIdx;
  
    /* take semaphore to prevent a task switch during this call NOTE:is this necessary for sbn_client?*/
  
    /* get task id for events NOTE: probably not necessary for sbn_client*/
  
    /* get the callers Application Id  NOTE: we already have this locally*/
  
    /* check that the pipe has been created */
    PipeIdx = CFE_SBN_Client_GetPipeIdx(PipeId);
  
    if (PipeIdx == CFE_SBN_CLIENT_INVALID_PIPE)
    {
      //TODO:Error here
      return CFE_SBN_CLIENT_BAD_ARGUMENT;
    }
  
    /* check that the requestor is the owner of the pipe NOTE: not necessary because there can be only 1 app? */
  
    /* check message id key and scope NOTE: do the same as cfe_sb_api?*/
  
    /* Convert the API MsgId into the SB internal representation MsgKey NOTE: not sure what this does yet*/
  
    /* check for duplicate subscription */  
  
    /* check for multiple subscriptions to same pipe? TODO: not sure how this is done */
  
    /* Get the index to the first available element in the routing table NOTE:how does routing table work? do I need it?*/
  
    MsgIdIdx = CFE_SBN_Client_GetMessageSubscribeIndex(PipeId);
    printf("MsgIdIdx = %d\n", MsgIdIdx);
    if (MsgIdIdx == CFE_SBN_CLIENT_MAX_MSG_IDS_MET)
    {
        //TODO:Error here
        return CFE_SBN_CLIENT_BAD_ARGUMENT;
    }
    
    PipeTbl[PipeIdx].SubscribedMsgIds[MsgIdIdx] = MsgId;
    
    return CFE_SUCCESS;
}

int32 __wrap_CFE_SB_SubscribeEx(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    printf ("SBN_Client:CFE_SB_SubscribeEx not yet implemented\n");
    return -1;
}

int32 __wrap_CFE_SB_SubscribeLocal(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId, uint16 MsgLim)
{
    printf ("SBN_Client:CFE_SB_SubscribeLocal not yet implemented\n");
    return -1;
}

int32 __wrap_CFE_SB_Unsubscribe(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
    printf ("SBN_Client:CFE_SB_Unsubscribe not yet implemented\n");
    return -1;
}

int32 __wrap_CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
    printf ("SBN_Client:CFE_SB_UnsubscribeLocal not yet implemented\n");
    return -1;
}

uint32 __wrap_CFE_SB_SendMsg(CFE_SB_Msg_t *msg)
{
    printf("SBN_Client:Sending Message...\n");
    char *buffer;
    uint16 msg_size = CFE_SB_GetTotalMsgLength(msg);

    size_t write_result, total_size = msg_size + SBN_PACKED_HDR_SZ;
    Pack_t Pack;

    if (total_size > CFE_SB_MAX_SB_MSG_SIZE)
    {
        return CFE_SB_MSG_TOO_BIG;
    }

    buffer = malloc(total_size);

    Pack_Init(&Pack, buffer, total_size, 0);

    Pack_UInt16(&Pack, msg_size);
    Pack_UInt8(&Pack, SBN_APP_MSG);
    Pack_UInt32(&Pack, cpuId);

    memcpy(buffer + SBN_PACKED_HDR_SZ, msg, msg_size);

    write_result = write(sockfd, buffer, total_size);

    if (write_result != total_size)
    {
        // TODO: This isn't an allocation error...
        return CFE_SB_BUF_ALOC_ERR;
    }

    free(buffer);

    return CFE_SUCCESS;
}

int32 __wrap_CFE_SB_RcvMsg(CFE_SB_MsgPtr_t *BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut)
{
    // Oh my.
    // Need to have multiple pipes... so the subscribe thing
    // Need to coordinate with the recv_msg thread... so locking?
    // Also, what about messages that get split? Is that an issue?
    int8   pipe_idx;
    time_t entry_time = time(NULL);
    
    //TODO: TimeOut is in milliseconds.  do a better job of timing on this instead of just seconds.
    while (entry_time + (TimeOut / 1000) > time(NULL))
    {
        pipe_idx = CFE_SBN_Client_GetPipeIdx(PipeId);
        if (pipe_idx == CFE_SBN_CLIENT_INVALID_PIPE)
        {
            puts("BAD!! INVALID PIPE ERROR!");
            //TODO: don't know if this is a valid error return value;
            return CFE_SBN_CLIENT_INVALID_PIPE;
        }
        else
        {
            CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_idx];
            uint32 next_msg = pipe->NextMessage;
            uint16 msg_size;
            
            if (pipe->NumberOfMessages > 0)
            {
                msg_size = CFE_SB_GetTotalMsgLength(pipe->Messages[next_msg]);
                memcpy(*BufPtr, pipe->Messages[next_msg], msg_size);
                
                pipe->NextMessage = (next_msg + 1) % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
                return CFE_SUCCESS;
            }
        
        }/* end if */
        
    }/* end while */
    
} /* end __wrap_CFE_SB_RcvMsg */

int32 __wrap_CFE_SB_ZeroCopySend(CFE_SB_Msg_t *MsgPtr, CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    printf ("SBN_Client:CFE_SB_ZeroCopySend not yet implemented\n");
    return -1;
}
// Receiving messages

void *receiveMinder(void *vargp)
{
    while(1) // TODO: check run state?
    {
        printf("SBN_Client: Checking messages\n");
        recv_msg(sockfd); // TODO: pass message pointer?
        // On heartbeats, need to update known liveness state of SBN
        // On other messages, need to make available for next CFE_SB_RcvMsg call
    }
}

int recv_msg(int sockfd)
{
    int bytes_received = 0;
    int total_bytes_recd = 0;
    unsigned char sbn_hdr_buffer[SBN_PACKED_HDR_SZ];
    unsigned char msg[CFE_SB_MAX_SB_MSG_SIZE];
    SBN_MsgSz_t MsgSz;
    SBN_MsgType_t MsgType;
    SBN_CpuID_t CpuID;
    
    int status = CFE_SBN_CLIENT_ReadBytes(sockfd, sbn_hdr_buffer, SBN_PACKED_HDR_SZ);
    
    //TODO: status check goes here

    // TODO: error checking (-1 returned, perror)

    Unpack_t Unpack;
    Unpack_Init(&Unpack, sbn_hdr_buffer, SBN_PACKED_HDR_SZ);
    Unpack_UInt16(&Unpack, &MsgSz);
    Unpack_UInt8(&Unpack, &MsgType);
    Unpack_UInt32(&Unpack, &CpuID);

    for (int i = 0; i < SBN_PACKED_HDR_SZ; i++)
    {
        printf("0x%02X ", sbn_hdr_buffer[i]);
    }
    printf("\n");

    printf("Msg Size: %d\t Msg Type: %d\t Processor_ID: %d\n", MsgSz, MsgType, CpuID);

    //TODO: check cpuID to see if it is correct for this location?

    switch(MsgType)
    {
        case SBN_NO_MSG:
            printf("SBN_Client recv_msg: SBN_NO_MSG\n");
            CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
            break;
        case SBN_SUB_MSG:
            printf("SBN_Client recv_msg: SBN_SUB_MSG\n");
            CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
            break;
        case SBN_UNSUB_MSG:
            printf("SBN_Client recv_msg: SBN_UNSUB_MSG\n");
            CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
            break;
        case SBN_APP_MSG:
            printf("SBN_Client recv_msg: SBN_APP_MSG\n");
            int status = 0;
            ingest_app_message(sockfd, MsgSz);
            return status;
        case SBN_PROTO_MSG:
            printf("SBN_Client recv_msg: SBN_PROTO_MSG\n");      
            CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
            break;
        case SBN_RECVD_HEARTBEAT_MSG:
            printf("SBN_Client recv_msg: Heartbeat received!\n");
            CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
            break;

        default:
            printf("SBN_Client recv_msg: ERROR - unrecognized type %d\n", MsgType);
    }
    
    puts("Received Message:");
    for (int i = 0; i < MsgSz; i++)
    {
        printf("0x%02X ", msg[i]);
    }
    printf("\n");
}

