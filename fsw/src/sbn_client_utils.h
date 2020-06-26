#ifndef _sbn_client_utils_h_
#define _sbn_client_utils_h_

#include <stdint.h>
#include <stdarg.h>

#include "sbn_pack.h"
#include "sbn_client.h"


#define MAX_LOG_MESSAGE_SIZE   80

/************************************************************************
** Type Definitions
*************************************************************************/

/******************************************************************************
**  Typedef:  CFE_SB_BufferD_t
**
**  Purpose:
**     This structure defines a BUFFER DESCRIPTOR used to specify the MsgId
**     and address of each packet buffer.
**
**     Note: Changing the size of this structure may require the memory pool
**     block sizes to change.
*/

typedef struct {
     CFE_SB_MsgId_t    MsgId;
     uint16            UseCount;
     uint32            Size;
     void              *Buffer;
     CFE_SB_SenderId_t Sender;
} CFE_SBN_Client_BufferD_t;

typedef struct {
    uint8              InUse;
    CFE_SB_PipeId_t    PipeId;
    char               PipeName[OS_MAX_API_NAME];
    char               AppName[OS_MAX_API_NAME];
    uint8              Opts;
    uint8              Spare;
    uint32             AppId;
    uint32             SysQueueId;
    uint32             LastSender;
    uint16             QueueDepth;
    uint16             SendErrors;
    uint32     NumberOfMessages;
    uint32     ReadMessage;
    unsigned char  Messages[CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH][CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];
    CFE_SB_MsgId_t     SubscribedMsgIds[CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE];
} CFE_SBN_Client_PipeD_t;

// SBN header // TODO: from include?
typedef struct
{
    uint16 SBN_MsgSz;
    uint8  SBN_MsgType;
    uint32 SBN_ProcessorID;
} SBN_Hdr_t;

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
CFE_SB_MsgId_t CFE_SBN_Client_GetMsgId(CFE_SB_MsgPtr_t);
int send_heartbeat(int);
uint16 CFE_SBN_Client_GetTotalMsgLength(CFE_SB_MsgPtr_t);
int connect_to_server(const char *, uint16_t);
int log_message(const char *, ...);

#endif /* _sbn_client_utils_h_ */

