#ifndef _sbn_client_h_
#define _sbn_client_h_

/************************************************************************
** Includes
*************************************************************************/

#include "sbn_interfaces.h"

/************************************************************************
** Comstants Definitions
*************************************************************************/
#define SBN_CLIENT_SUCCESS                      OS_SUCCESS

#define CFE_SBN_CLIENT_NOT_IN_USE               0
#define CFE_SBN_CLIENT_IN_USE                   1
#define CFE_SBN_CLIENT_UNUSED_QUEUE             0xFFFF
#define CFE_SBN_CLIENT_MAX_MESSAGE_SIZE         CFE_SB_MAX_SB_MSG_SIZE
#define CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE     4
#define CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH  32
#define CFE_PLATFORM_SBN_CLIENT_MAX_PIPES       5 //CFE_PLATFORM_SB_MAX_PIPES
#define CFE_SBN_CLIENT_BAD_ARGUMENT             CFE_SB_BAD_ARGUMENT
#define CFE_SBN_CLIENT_INVALID_PIPE             0xFF
#define SBN_CLIENT_NO_STATUS_SET                0xFFFF
#define SBN_CLIENT_BAD_SOCK_FD_EID              0x0100
#define CFE_SBN_CLIENT_CR_PIPE_BAD_ARG_EID      1002
#define CFE_SBN_CLIENT_MAX_PIPES_MET            1003
#define CFE_SBN_CLIENT_MAX_PIPES_MET_EID        1004
#define CFE_SBN_CLIENT_CR_PIPE_ERR_EID          1005
#define CFE_SBN_CLIENT_PIPE_ADDED_EID           1006
#define CFE_SBN_CLIENT_PIPE_DELETED_EID         1007
#define CFE_SBN_CLIENT_MAX_MSG_IDS_MET          0xFF
#define CFE_SBN_CLIENT_MAX_MSG_IDS_MET_EID      1009
#define CFE_SBN_CLIENT_PIPE_BROKEN_ERR          1010
#define CFE_SBN_CLIENT_PIPE_CLOSED_ERR          1011
#define CFE_SBN_CLIENT_PIPE_CR_ERR              ((int32)0xca001005)
#define SBN_CLIENT_HEART_THREAD_CREATE_EID      1012
#define SBN_CLIENT_RECEIVE_THREAD_CREATE_EID    1013


#define CFE_SBN_CLIENT_INVALID_MSG_ID           0

#define CFE_SBN_CLIENT_MSG_ID_TO_PIPE_ID_MAP_SIZE  32

#define SBN_RECVD_HEARTBEAT_MSG                 0xA0

#define SERVER_SOCKET_ERROR        -1
#define SERVER_INET_PTON_SRC_ERROR   -2
#define SERVER_INET_PTON_INVALID_AF_ERROR   -3
#define SERVER_CONNECT_ERROR       -4

// defines to replace items normally created in CFE  TODO:figure out the best way to get these defined per app using sbn_client
#define APP_ID  99
#define APP_NAME "Test_Peer"

#define  CFE_SBN_CLIENT_NO_PROTOCOL    0
#define  SBN_TCP_HEARTBEAT_MSG         0xA0

/*************************************************************************
** Exported Functions
*************************************************************************/

void CFE_SBN_Client_InitPipeTbl(void);
CFE_SB_PipeId_t CFE_SBN_Client_GetAvailPipeIdx(void);
void ingest_app_message(int, SBN_MsgSz_t);
int32 recv_msg(int32);
void SendSubToSbn(int, CFE_SB_MsgId_t, CFE_SB_Qos_t);


/*************************************************************************
** Exported Globals
*************************************************************************/

#endif /* _sbn_client_h_ */

/************************/
/*  End of File Comment */
/************************/
