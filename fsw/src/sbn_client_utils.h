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

#ifndef _sbn_client_utils_h_
#define _sbn_client_utils_h_

#include <stdint.h>
#include <stdarg.h>

#include "sbn_pack.h"
#include "sbn_client.h"
#include "sbn_client_logger.h"
#include "sbn_client_defs.h"

/************************************************************************
** Type Definitions
*************************************************************************/

/* TODO: Doxygen comments */
typedef struct {
    uint8             InUse;
    CFE_SB_PipeId_t   PipeId;
    char              PipeName[OS_MAX_API_NAME];
    char              AppName[OS_MAX_API_NAME];
    uint8             Opts;
    uint8             Spare;
    uint32            AppId;
    uint32            SysQueueId;
    uint32            LastSender;
    uint16            QueueDepth;
    uint16            SendErrors;
    uint32            NumberOfMessages;
    uint32            ReadMessage;
    unsigned char     Messages[CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH][CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];
    CFE_SB_MsgId_t    SubscribedMsgIds[CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE];
} CFE_SBN_Client_PipeD_t;

/* TODO: Doxygen comments */
typedef struct {
  int  msgId;
  int  pipeIds[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
} MsgId_to_pipes_t;



int32 check_pthread_create_status(int, int32);
int message_entry_point(CFE_SBN_Client_PipeD_t);
int CFE_SBN_CLIENT_ReadBytes(int, unsigned char *, size_t);
void invalidate_pipe(CFE_SBN_Client_PipeD_t *);
size_t write_message(int, char *, size_t);
uint8 CFE_SBN_Client_GetPipeIdx(CFE_SB_PipeId_t);
uint8 CFE_SBN_Client_GetMessageSubscribeIndex(CFE_SB_PipeId_t);
CFE_SB_MsgId_t CFE_SBN_Client_GetMsgId(CFE_MSG_Message_t *);
int send_heartbeat(int);
CFE_MSG_Size_t CFE_SBN_Client_GetTotalMsgLength(const CFE_MSG_Message_t *);
int connect_to_server(const char *, uint16_t);

#endif /* _sbn_client_utils_h_ */

