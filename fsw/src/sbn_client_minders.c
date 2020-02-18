
#include <unistd.h>

#include "sbn_client_minders.h"
#include "sbn_client.h"

extern int sbn_client_sockfd;


/* Deal with sending out heartbeat messages */
void *heartbeatMinder(void *vargp)
{
    while(1) // TODO: check run state?
    {
        if (sbn_client_sockfd != 0)
        {
            send_heartbeat(sbn_client_sockfd);
        }
        
        sleep(3);
    }
    
    return NULL;
}


void *receiveMinder(void *vargp)
{
    int32 status;
    
    while(1) // TODO: check run state?
    {
        status = recv_msg(sbn_client_sockfd); // TODO: pass message pointer?
        // On heartbeats, need to update known liveness state of SBN
        // On other messages, need to make available for next CFE_SB_RcvMsg call
        if (status != CFE_SUCCESS)
        {
            printf("Recieve message returned error 0x%08X\n", status);
        }/* end if */
        
    }
    
}