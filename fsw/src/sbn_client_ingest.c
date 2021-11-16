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

#include <pthread.h>
#include <string.h>

#include "sbn_client_ingest.h"

pthread_mutex_t receive_mutex      = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  received_condition = PTHREAD_COND_INITIALIZER;

/* TODO: Using memcpy to move message into pipe. What about pointer passing?
 *    Can we only look to msgId then memcpy only that then read directly
 *    into pipe? This could speed things up... 
 * passing pointers will only work here if it is guaranteed that the message 
 * will not be destroyed.  SBN may not be able to provide that assurance */

void ingest_app_message(int SockFd, SBN_MsgSz_t MsgSz)
{
    int            status, i;
    boolean        at_least_1_pipe_is_in_use = FALSE;    
    unsigned char  msg_buffer[CFE_SB_MAX_SB_MSG_SIZE];
    CFE_SB_MsgId_t MsgId;
    
    status = CFE_SBN_CLIENT_ReadBytes(SockFd, msg_buffer, MsgSz);
    
    if (status != CFE_SUCCESS)
    {
        char error_message[61];
        
        snprintf(error_message, sizeof(error_message), 
          "CFE_SBN_CLIENT_ReadBytes returned a bad status = 0x%08X\n", status);
        log_message(error_message);
        
        return;
    }

    MsgId = CFE_SBN_Client_GetMsgId((CFE_SB_MsgPtr_t)msg_buffer);
    
    pthread_mutex_lock(&receive_mutex);
    
    /* Put message into pipe */    
    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
    {    
        if (PipeTbl[i].InUse == CFE_SBN_CLIENT_IN_USE)
        {
            int j;
            
            at_least_1_pipe_is_in_use = TRUE;
            
            for(j = 0; j < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; j++)
            {
                if (PipeTbl[i].SubscribedMsgIds[j] == MsgId)
                {
                    if (PipeTbl[i].NumberOfMessages == CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
                    {
                        /* TODO: handle error pipe overflow */
                        log_message("SBN_CLIENT: ERROR pipe overflow");
                        
                        pthread_mutex_unlock(&receive_mutex);
                        return;
                    }
                    else /* message is put into pipe */
                    {    
                        log_message("App message received: MsgId 0x%08X", MsgId);
                        
                        memcpy(PipeTbl[i].Messages[message_entry_point(PipeTbl[i])], msg_buffer, MsgSz);
                        PipeTbl[i].NumberOfMessages++;
                        
                        pthread_mutex_unlock(&receive_mutex);

                        /* only a received message should send signal */
                        pthread_cond_signal(&received_condition);
                        
                        return;
                    } /* end if */
                    
                }/* end if */
                 
            } /* end for */
            
        } /* end if */
    
    } /* end for */
    
    if (at_least_1_pipe_is_in_use)
    {
        log_message("SBN_CLIENT: ERROR no subscription for this msgid");  
    }
    else
    {
        log_message("SBN_CLIENT: No pipes are in use");
    }
    
    pthread_mutex_unlock(&receive_mutex);
}