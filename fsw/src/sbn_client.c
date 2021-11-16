/*
** GSC-18396-1, “Software Bus Network Client for External Process”
**
** Copyright © 2019 United States Government as represented by
** the Administrator of the National Aeronautics and Space Administration.
** No copyright is claimed in the United States under Title 17, U.S. Code.
** All Other Rights Reserved.
**
** Licensed under the NASA Open Source Agreement version 1.3
** See "NOSA GSC-18396-1.pdf"
*/

#include "sbn_client.h"
#include "sbn_client_ingest.h"
#include "sbn_client_utils.h"

/* Global variables */
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
    Pack_Data(&Pack, (void *)SBN_IDENT, (size_t)SBN_IDENT_LEN);
    Pack_UInt16(&Pack, 1);

    Pack_MsgID(&Pack, MsgID);
    Pack_Data(&Pack, &QoS, sizeof(QoS)); /* 2 uint8's */
    
    size_t write_result = write_message(sbn_client_sockfd, Buf, Pack.BufUsed);
    
    if (write_result != Pack.BufUsed)
    {
      log_message("SBN_CLIENT: ERROR SendSubToSbn!!\n");
    }
    
}/* end SendLocalSubToPeer */


int32 recv_msg(int32 sockfd)
{
    unsigned char sbn_hdr_buffer[SBN_PACKED_HDR_SZ];
    unsigned char msg[CFE_SB_MAX_SB_MSG_SIZE];
    SBN_MsgSz_t MsgSz;
    SBN_MsgType_t MsgType;
    uint32 CpuID;
    
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
            case SBN_HEARTBEAT_MSG:
                status = CFE_SBN_CLIENT_ReadBytes(sockfd, msg, MsgSz);
                break;

            default:
                log_message("SBN_CLIENT: ERROR - recv_msg unrecognized type %d\n", MsgType);
                status =  CFE_EVS_ERROR; //TODO: change error
        }
        
    }
    
    return status;
}

