#include <pthread.h>

#include "sbn_client.h"
#include "sbn_client_minders.h"
#include "sbn_client_utils.h"

/* Refer to sbn_cont_tbl.c to make sure port and ip_addr match
 * SBN is running here: <- Should be in the platform config */
#define SBN_CLIENT_PORT    1234
#define SBN_CLIENT_IP_ADDR "127.0.0.1"

extern int sbn_client_sockfd;
extern int sbn_client_cpuId;

pthread_t receive_thread_id;
pthread_t heart_thread_id;


int32 SBN_Client_Init(void)
{
     /* Gets socket file descriptor */
    int32 status = SBN_CLIENT_NO_STATUS_SET;
    int heart_thread_status = 0;
    int receive_thread_status = 0;
    
    printf("SBN_Client Connecting to %s, %d\n", SBN_CLIENT_IP_ADDR, 
        SBN_CLIENT_PORT);
    
    sbn_client_sockfd = connect_to_server(SBN_CLIENT_IP_ADDR, SBN_CLIENT_PORT);
    sbn_client_cpuId = 2; /* TODO: this is hardcoded, but should be set by cFS's SBN ??*/

    if (sbn_client_sockfd < 0)
    {
        puts("SBN_CLIENT: ERROR Failed to get sbn_client_sockfd, cannot continue.");
        status = SBN_CLIENT_BAD_SOCK_FD_EID;
    }
    else
    {
        /* Create pipe table */
        CFE_SBN_Client_InitPipeTbl();

        /* Create thread for watchdog and receive */
        heart_thread_status = pthread_create(&heart_thread_id, NULL, 
            SBN_Client_HeartbeatMinder, NULL);
            
        status = check_pthread_create_status(heart_thread_status, 
            SBN_CLIENT_HEART_THREAD_CREATE_EID);
        
        if (status == SBN_CLIENT_SUCCESS)
        {    
            receive_thread_status = pthread_create(&receive_thread_id, NULL, 
            SBN_Client_ReceiveMinder, NULL);
        
            status = check_pthread_create_status(receive_thread_status, 
                SBN_CLIENT_RECEIVE_THREAD_CREATE_EID);
        }/* end if */ 
        
    }/* end if */ 
    
    if (status != SBN_CLIENT_SUCCESS)
    {
        printf("SBN_Client_Init error %d\n", status);
        exit(status);
    }/* end if */ 
    
    return status;
}/* end SBN_Client_Init */