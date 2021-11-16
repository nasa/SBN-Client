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

#include "sbn_client.h"
#include "sbn_client_minders.h"
#include "sbn_client_utils.h"

#define SECONDS_BETWEEN_HEARTBEATS   3

extern int sbn_client_sockfd;

boolean continue_heartbeat = TRUE;
boolean continue_receive_check = TRUE;


void *SBN_Client_HeartbeatMinder(void *vargp)
{
    while(continue_heartbeat) /* TODO: check run state? */
    {
        
        if (sbn_client_sockfd != 0)
        {
            send_heartbeat(sbn_client_sockfd);
        } /* end if */
        
        sleep(SECONDS_BETWEEN_HEARTBEATS);
    } /* end while */
    
    return NULL;
} /* end SBN_Client_HeartbeatMinder */


void *SBN_Client_ReceiveMinder(void *vargp)
{
    int32 status;
    int32 consec_error_count = 0;
    
    while(continue_receive_check) /* TODO: check run state? */
    {
        status = recv_msg(sbn_client_sockfd); /* TODO: pass message pointer? */
        /* On heartbeats, need to update known liveness state of SBN
        ** On other messages, need to make available for next CFE_SB_RcvMsg call */
        
        if (status != CFE_SUCCESS)
        {
            log_message("Recieve message returned error 0x%08X\n", status);
            consec_error_count++;
        }
        else
        {
            consec_error_count = 0;
        } /* end if */

        if (5 == consec_error_count) {
            continue_heartbeat = FALSE;
            continue_receive_check = FALSE;
        }
        
    } /* end while */
    
    return NULL;
} /* end SBN_Client_ReceiveMinder */