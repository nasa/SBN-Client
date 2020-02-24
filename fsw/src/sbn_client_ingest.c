#include <pthread.h>

#include "sbn_client_ingest.h"
#include "sbn_client_utils.h"

pthread_mutex_t receive_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  received_condition = PTHREAD_COND_INITIALIZER;

extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
/* NOTE: Using memcpy to move message into pipe. What about pointer passing?
 *    Can we only look to msgId then memcpy only that then read directly
 *    into pipe? This could speed things up... */
void ingest_app_message(int sockfd, SBN_MsgSz_t MsgSz)
{
    int status;
    boolean at_least_1_pipe_in_use = FALSE;    
    unsigned char msg_buffer[CFE_SB_MAX_SB_MSG_SIZE];
    CFE_SB_MsgId_t MsgId;
    
    status = CFE_SBN_CLIENT_ReadBytes(sockfd, msg_buffer, MsgSz);
    
    if (status != CFE_SUCCESS)
    {
        char error_message[60];
        
        snprintf(error_message, 60, 
          "CFE_SBN_CLIENT_ReadBytes returned a bad status = 0x%08X\n", status);
        log_message(error_message);
        return;
    }

    MsgId = CFE_SBN_Client_GetMsgId((CFE_SB_MsgPtr_t)msg_buffer);
    
    /* TODO: check that msgid is valid - How? */
    
    /* Take mutex */
    pthread_mutex_lock(&receive_mutex);
    
    /*Put message into pipe */    
    int i;
    
    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
    {    
        if (PipeTbl[i].InUse == CFE_SBN_CLIENT_IN_USE)
        {
            int j;
            
            at_least_1_pipe_in_use = TRUE;
            
            for(j = 0; j < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; j++)
            {
                if (PipeTbl[i].SubscribedMsgIds[j] == MsgId)
                {
                    if (PipeTbl[i].NumberOfMessages == 
                        CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
                    {
                        //TODO: error pipe overflow
                        log_message("SBN_CLIENT: ERROR pipe overflow");
                        
                        pthread_mutex_unlock(&receive_mutex);
                        return;
                    }
                    else /* message is put into pipe */
                    {    
                        log_message("message received");
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
    
    if (at_least_1_pipe_in_use)
    {
        log_message("SBN_CLIENT: ERROR no subscription for this msgid");  
    }
    else
    {
        log_message("SBN_CLIENT: No pipes are in use");
    }
    
    pthread_mutex_unlock(&receive_mutex);
    return;
}