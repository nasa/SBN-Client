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

#ifndef _sbn_client_h_
#define _sbn_client_h_

/************************************************************************
** Includes
*************************************************************************/

#include "sbn_interfaces.h"

/************************************************************************
** Constants Definitions
*************************************************************************/
#define SBN_CLIENT_SUCCESS                      OS_SUCCESS

#define CFE_SBN_CLIENT_NOT_IN_USE               0
#define CFE_SBN_CLIENT_IN_USE                   1
#define CFE_SBN_CLIENT_UNUSED_QUEUE             0xFFFF
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
#define CFE_SBN_CLIENT_NO_PROTOCOL              0

#define SERVER_SOCKET_ERROR                     -1
#define SERVER_INET_PTON_SRC_ERROR              -2
#define SERVER_INET_PTON_INVALID_AF_ERROR       -3
#define SERVER_CONNECT_ERROR                    -4


/*************************************************************************
** Exported Functions
*************************************************************************/

void CFE_SBN_Client_InitPipeTbl(void);
CFE_SB_PipeId_t CFE_SBN_Client_GetAvailPipeIdx(void);
int32 recv_msg(int32);
void SendSubToSbn(int, CFE_SB_MsgId_t, CFE_SB_Qos_t);

#endif /* _sbn_client_h_ */

/************************/
/*  End of File Comment */
/************************/
