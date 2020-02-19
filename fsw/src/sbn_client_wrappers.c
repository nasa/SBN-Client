#include <pthread.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#include "sbn_client.h"
#include "sbn_client_utils.h"

extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
extern int sbn_client_sockfd;
extern int sbn_client_cpuId;
extern pthread_mutex_t receive_mutex;
extern pthread_cond_t  received_condition;
// Pipe creation / subscription

int32 __wrap_CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, const char *PipeName)
{
    uint8 i;
    int32 status = CFE_SBN_CLIENT_MAX_PIPES_MET;
    
    //SBN_Client_Init();
    
    /* AppId is static for now */
    
    /* caller name is static for now */
    
    /* name will not require NULL terminator */
    
    /* TODO: determine if semaphore is necessary */
    
    /* TODO: determine if taskId is necessary */
    
    /* set user's pipe id value to 'invalid' for error cases below */
    if(PipeIdPtr != NULL)
    {
        *PipeIdPtr = CFE_SBN_CLIENT_INVALID_PIPE;
    }/* end if */
    
    /* verify input parameters are valid */
    if((PipeIdPtr == NULL)||(Depth > CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)||(Depth == 0))
    {
        status = CFE_SBN_CLIENT_BAD_ARGUMENT;
    }
    else
    {
        
        for(i = 0; i<CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
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
                //TODO: init Messages to empty?

                *PipeIdPtr = i;

                status = SBN_CLIENT_SUCCESS;
                break;
            }/* end if */
            
        }/* end for */
        
    }/* end if */
        
    return status;
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
                invalidate_pipe(&PipeTbl[i]);
                // CFE_EVS_SendEvent(CFE_SBN_CLIENT_PIPE_DELETED_EID, 
                // CFE_EVS_EventType_DEBUG, "Pipe Deleted:id %d,owner %s",
                // (int)PipeId, APP_NAME);
                return CFE_SUCCESS;
            }
            else
            {
                //TODO:error
                return -1;
            }
            
        }
        
    }
    
    //TODO: if we get here no pipes matched, error
    
    return -2;
}

int32 __wrap_CFE_SB_Subscribe(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
    uint8 PipeIdx;
    uint8 MsgIdIdx;
    CFE_SB_Qos_t QoS;
  
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
    //printf("MsgIdIdx = %d\n", MsgIdIdx);
    if (MsgIdIdx == CFE_SBN_CLIENT_MAX_MSG_IDS_MET)
    {
        //TODO:Error here
        return CFE_SBN_CLIENT_BAD_ARGUMENT;
    }
    
    PipeTbl[PipeIdx].SubscribedMsgIds[MsgIdIdx] = MsgId;
    
    QoS.Priority = 0x00;
    QoS.Reliability = 0x00;
    
    SendSubToSbn(SBN_SUB_MSG, MsgId, QoS);
    
    return CFE_SUCCESS;
}

int32 __wrap_CFE_SB_SubscribeEx(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId, 
                                CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    printf ("SBN_CLIENT: ERROR CFE_SB_SubscribeEx not yet implemented\n");
    return -1;
}

int32 __wrap_CFE_SB_SubscribeLocal(CFE_SB_MsgId_t  MsgId, 
                                   CFE_SB_PipeId_t PipeId, 
                                   uint16 MsgLim)
{
    printf ("SBN_CLIENT: ERROR CFE_SB_SubscribeLocal not yet implemented\n");
    return -1;
}

int32 __wrap_CFE_SB_Unsubscribe(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
    printf ("SBN_CLIENT: ERROR CFE_SB_Unsubscribe not yet implemented\n");
    return -1;
}

int32 __wrap_CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t  MsgId, 
                                     CFE_SB_PipeId_t PipeId)
{
    printf ("SBN_CLIENT: ERROR CFE_SB_UnsubscribeLocal not yet implemented\n");
    return -1;
}

uint32 __wrap_CFE_SB_SendMsg(CFE_SB_Msg_t *msg)
{
    //printf("SBN_Client:Sending Message...\n");
    char *buffer;
    uint16 msg_size = CFE_SBN_Client_GetTotalMsgLength(msg);

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
    Pack_UInt32(&Pack, sbn_client_cpuId);

    memcpy(buffer + SBN_PACKED_HDR_SZ, msg, msg_size);
    
    // int i = 0;
    // puts("SUB MSG: ");
    // for (i;i < Pack.BufUsed; i++)
    // {
    //   printf("0x%02x ", (unsigned char)Buf[i]);
    // }
    // printf("i = %d\n", i);
    // puts("");

    write_result = write_message(sbn_client_sockfd, buffer, total_size);

    if (write_result != total_size)
    {
        // TODO: This isn't an allocation error...
        return CFE_SB_BUF_ALOC_ERR;
    }

    free(buffer);

    return CFE_SUCCESS;
}

int32 __wrap_CFE_SB_RcvMsg(CFE_SB_MsgPtr_t *BufPtr, CFE_SB_PipeId_t PipeId, 
                           int32 TimeOut)
{
    //puts("SBN_CLIENT: Checking for messages...");
    // Oh my.
    // Need to have multiple pipes... so the subscribe thing
    // Need to coordinate with the recv_msg thread... so locking?
    // Also, what about messages that get split? Is that an issue?
    int8            pipe_idx;
    int32           status;
    struct timespec enter_time;
    
    clock_gettime(CLOCK_REALTIME, &enter_time);
      
    pipe_idx = CFE_SBN_Client_GetPipeIdx(PipeId);
    
    if (pipe_idx == CFE_SBN_CLIENT_INVALID_PIPE)
    {
        puts("SBN_CLIENT: ERROR INVALID PIPE ERROR!");
        //TODO: don't know if this is a valid error return value;
        status = CFE_SBN_CLIENT_INVALID_PIPE;
    }
    else
    {
        CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_idx];
        
        pthread_mutex_lock(&receive_mutex);
        
        if (pipe->NumberOfMessages > 1)
        {
            /* must progress to next message in pipe */
            uint32 next_msg = (pipe->ReadMessage + 1) % 
              CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
            pipe->ReadMessage = next_msg;
            
            *BufPtr = (CFE_SB_MsgPtr_t)(&(pipe->Messages[next_msg]));
            
            pipe->NumberOfMessages -= 1;
            status = CFE_SUCCESS;
        }
        else
        {
          int wait_result;
          struct timespec future_timeout;
          
          /* set future time for timeout check to entry time + timeout 
           * milliseconds */
          future_timeout.tv_sec = enter_time.tv_sec;
          future_timeout.tv_nsec = enter_time.tv_nsec + (TimeOut * pow(10, 6));

          /* when nsec greater than 1 second perform update to seconds and 
           * nanoseconds */
          if (future_timeout.tv_nsec >= pow(10, 9))
          {
            future_timeout.tv_sec += future_timeout.tv_nsec / pow(10, 9);
            future_timeout.tv_nsec = future_timeout.tv_nsec % (long) pow(10, 9);
          }
          
          wait_result = pthread_cond_timedwait(&received_condition, 
                                               &receive_mutex, 
                                               &future_timeout);
               
          if (wait_result == ETIMEDOUT)
          {
            status = CFE_EVS_EventType_ERROR;
          } /* end if */
          
        } /* end if */
    
    } /* end if */
    
    int pmu = pthread_mutex_unlock(&receive_mutex);
    
    if (pmu != 0)
    {
      status =  CFE_EVS_ERROR;
    } /* end if */
    
    return status;
} /* end __wrap_CFE_SB_RcvMsg */

int32 __wrap_CFE_SB_ZeroCopySend(CFE_SB_Msg_t *MsgPtr, 
                                 CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    printf ("SBN_CLIENT: ERROR CFE_SB_ZeroCopySend not yet implemented\n");
    return -1;
}

