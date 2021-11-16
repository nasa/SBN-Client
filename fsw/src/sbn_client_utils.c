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

#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "sbn_client_utils.h"

extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];

struct sockaddr_in server_address;


int32 check_pthread_create_status(int status, int32 errorId)
{
    int32 thread_status;
    
    if (status == 0)
    {
        thread_status = SBN_CLIENT_SUCCESS; 
    }
    else
    {
        switch(status)
        {
            case EAGAIN:
            log_message("Create thread error = EAGAIN");
            break;
            
            case EINVAL:
            log_message("Create thread error = EINVAL");
            break;
            
            case EPERM:
            log_message("Create thread error = EPERM");
            break;
            
            default:
            printf("Unknown thread creation error = %d\n", status);        
        }
        
        perror("pthread_create error");
        
        thread_status = errorId;
    }/* end if */
    
    return thread_status;
}

/* message_entry_point determines which slot a new message enters the pipe.
 * the mod allows it to go around the bend easily, i.e. 2 + 4 % 5 = 1, 
 * slots 2,3,4,0 are taken so 1 is entry */
int message_entry_point(CFE_SBN_Client_PipeD_t pipe)
{
    return (pipe.ReadMessage + pipe.NumberOfMessages) % 
        CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
}

int CFE_SBN_CLIENT_ReadBytes(int sockfd, unsigned char *msg_buffer, 
                             size_t MsgSz)
{
    int bytes_received = 0;
    int total_bytes_recd = 0;
    
    /* TODO:Some kind of timeout on this? */
    while (total_bytes_recd != MsgSz)
    {
        bytes_received = read(sockfd, msg_buffer + total_bytes_recd, 
                              MsgSz - total_bytes_recd);
        
        if (bytes_received < 0)
        {
            /* TODO:ERROR socket is dead somehow */       
            log_message("SBN_CLIENT: ERROR CFE_SBN_CLIENT_PIPE_BROKEN_ERR\n");
            return CFE_SBN_CLIENT_PIPE_BROKEN_ERR;
        }
        else if (bytes_received == 0)
        {
            /* TODO:ERROR closed remotely */
            log_message("SBN_CLIENT: ERROR CFE_SBN_CLIENT_PIPE_CLOSED_ERR: %s\n", strerror(errno));
            return CFE_SBN_CLIENT_PIPE_CLOSED_ERR;
        }
        
        total_bytes_recd += bytes_received;
    }
    // 
    // log_message("CFE_SBN_CLIENT_ReadBytes THIS MESSAGE:");
    // int i =0;
    // for (i = 0; i < MsgSz; i++)
    // {
    //     printf("0x%02X ", msg_buffer[i]);
    // }
    // printf("\n");
    
    return CFE_SUCCESS;
}

void invalidate_pipe(CFE_SBN_Client_PipeD_t *pipe)
{
    int i;
    
    pipe->InUse         = CFE_SBN_CLIENT_NOT_IN_USE;
    pipe->SysQueueId    = CFE_SBN_CLIENT_UNUSED_QUEUE;
    pipe->PipeId        = CFE_SBN_CLIENT_INVALID_PIPE;
    /* SB always holds one message so Number of messages should always be a minimum of 1 */
    pipe->NumberOfMessages = 1;
    /* Message to be read will be incremented after receive is called */
    /* Therefor initial next message is the last in the chain */
    pipe->ReadMessage = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1;
    memset(&pipe->PipeName[0],0,OS_MAX_API_NAME);
    
    for(i = 0; i < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; i++)
    {
        pipe->SubscribedMsgIds[i] = CFE_SBN_CLIENT_INVALID_MSG_ID;
    }
    
    
}

size_t write_message(int sockfd, char *buffer, size_t size)
{
  size_t result;
  
  result = write(sockfd, buffer, size);
  
  return result;
}
    
uint8 CFE_SBN_Client_GetPipeIdx(CFE_SB_PipeId_t PipeId)
{
  /* Quick check because PipeId should match PipeIdx */
    if (PipeTbl[PipeId].PipeId == PipeId && PipeTbl[PipeId].InUse == CFE_SBN_CLIENT_IN_USE)
    {
        return PipeId;
    }
    else
    {
        int i;
    
        for(i=0;i<CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;i++)
        {

            if(PipeTbl[i].PipeId == PipeId && PipeTbl[i].InUse == 
                CFE_SBN_CLIENT_IN_USE)
            {
                return i;
            }/* end if */

        } /* end for */
    
        /* Pipe ID not found */
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

CFE_SB_MsgId_t CFE_SBN_Client_GetMsgId(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_MsgId_t MsgId = 0;

#ifndef MESSAGE_FORMAT_IS_CCSDS_VER_2
    MsgId = CCSDS_RD_SID(MsgPtr->Hdr);
#else

    uint32            SubSystemId;

    MsgId = CCSDS_RD_APID(MsgPtr->Hdr); /* Primary header APID  */
     
    if ( CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_CMD)
      MsgId = MsgId | CFE_SB_CMD_MESSAGE_TYPE;  

    /* Add in the SubSystem ID as needed */
    SubSystemId = CCSDS_RD_SUBSYSTEM_ID(MsgPtr->SpacePacket.ApidQ);
    MsgId = (MsgId | (SubSystemId << 8));
#endif

return MsgId;

}/* end CFE_SBN_Client_GetMsgId */

// TODO: return value?
int send_heartbeat(int sockfd)
{
    int retval;
    char sbn_header[SBN_PACKED_HDR_SZ] = {0};
    
    Pack_t Pack;
    Pack_Init(&Pack, sbn_header, 0 + SBN_PACKED_HDR_SZ, 0);
    
    Pack_UInt16(&Pack, 0);
    Pack_UInt8(&Pack, SBN_HEARTBEAT_MSG);
    Pack_UInt32(&Pack, 2);
    
    retval = write(sockfd, sbn_header, sizeof(sbn_header));
    
    return retval;
}

uint16 CFE_SBN_Client_GetTotalMsgLength(CFE_SB_MsgPtr_t MsgPtr)
{
    return CCSDS_RD_LEN(MsgPtr->Hdr);
}/* end CFE_SBN_Client_GetTotalMsgLength */

int connect_to_server(const char *server_ip, uint16_t server_port)
{
    int sockfd, address_converted, connection;
        
    sleep(5);

    /* Create an ipv4 TCP socket */
    sockfd = socket(AF_INET, SOCK_STREAM, CFE_SBN_CLIENT_NO_PROTOCOL);

    /* Socket error */
    if (sockfd < 0)
    {
        switch(errno)
        {
            case EACCES:
            case EAFNOSUPPORT:
            case EINVAL:
            case EMFILE:
            case ENOBUFS:
            case ENOMEM:
            case EPROTONOSUPPORT:
                log_message("Socket err = %s", strerror(errno));
                break;  
            
            default:
                log_message("Unknown socket error = %s", strerror(errno));  
        }
        
        return SERVER_SOCKET_ERROR;
    }
    
    memset(&server_address, '0', sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    address_converted = inet_pton(AF_INET, server_ip, &server_address.sin_addr);
    
    /* inet_pton can have two separate errors, a value of 1 is success. */
    if (address_converted == 0)
    {
        perror("connect_to_server inet_pton 0 error");
        return SERVER_INET_PTON_SRC_ERROR;
    }

    if (address_converted == -1)
    {
        perror("connect_to_server inet_pton -1 error");
        return SERVER_INET_PTON_INVALID_AF_ERROR;
    }

    connection = connect(sockfd, (struct sockaddr *)&server_address,
                         sizeof(server_address));
    
    /* Connect error */
    if (connection < 0)
    {
        switch(errno)
        {
            case EACCES:
            case EPERM:
            case EADDRINUSE:
            case EADDRNOTAVAIL:
            case EAFNOSUPPORT:
            case EAGAIN:
            case EALREADY:
            case EBADF:
            case ECONNREFUSED:
            case EFAULT:
            case EINPROGRESS:
            case EINTR:
            case EISCONN:
            case ENETUNREACH:
            case ENOTSOCK:
            case EPROTOTYPE:
            case ETIMEDOUT:
                log_message("connect err = %s", strerror(errno));
                break;
            
            default:
                log_message("Unknown connect error = %s", strerror(errno)); 
        }
        
        log_message("SERVER_CONNECT_ERROR: Connect failed error: %d\n", connection);
        return SERVER_CONNECT_ERROR;
    }

    return sockfd;
}
