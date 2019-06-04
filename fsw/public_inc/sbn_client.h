#ifndef _sbn_client_h_
#define _sbn_client_h_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/************************************************************************
** Type Definitions
*************************************************************************/
#define CFE_SBN_CLIENT_NOT_IN_USE               0
#define CFE_SBN_CLIENT_IN_USE                   1
#define CFE_SBN_CLIENT_UNUSED_QUEUE             0xFFFF
#define CFE_SBN_CLIENT_MAX_MESSAGE_SIZE         32767
#define CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE     4
#define CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH  32
#define CFE_PLATFORM_SBN_CLIENT_MAX_PIPES       5 //CFE_PLATFORM_SB_MAX_PIPES
#define CFE_SBN_CLIENT_BAD_ARGUMENT             CFE_SB_BAD_ARGUMENT
#define CFE_SBN_CLIENT_INVALID_PIPE             0xFF
#define CFE_SBN_CLIENT_CR_PIPE_BAD_ARG_EID      1002
#define CFE_SBN_CLIENT_MAX_PIPES_MET            1003
#define CFE_SBN_CLIENT_MAX_PIPES_MET_EID        1004
#define CFE_SBN_CLIENT_CR_PIPE_ERR_EID          1005
#define CFE_SBN_CLIENT_PIPE_ADDED_EID           1006
#define CFE_SBN_CLIENT_PIPE_DELETED_EID         1007
#define CFE_SBN_CLIENT_MAX_MSG_IDS_MET          0xFF
#define CFE_SBN_CLIENT_MAX_MSG_IDS_MET_EID      1009
#define CFE_SBN_CLIENT_PIPE_CR_ERR              ((int32)0xca001005)
#define CFE_SBN_CLIENT_INVALID_MSG_ID           0

#define CFE_SBN_CLIENT_MSG_ID_TO_PIPE_ID_MAP_SIZE  32

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
    uint32             NumberOfMessages;
    uint32             NextMessage;
    char               Messages[CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH][CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];
    CFE_SB_MsgId_t     SubscribedMsgIds[CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE]
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

typedef struct {
  
} SubscriptionTable_t;



// defines to replace items normally created in CFE  TODO:figure out the best way to get these defined per app using sbn_client
#define APP_ID  99
#define APP_NAME "Test_Peer"

/*************************************************************************
** Exported Functions
*************************************************************************/
int32 SBN_ClientInit(void);
void CFE_SBN_Client_InitPipeTbl(void);
void InvalidatePipe(CFE_SBN_Client_PipeD_t *pipe);

#endif /* _sbn_client_h_ */

/************************/
/*  End of File Comment */
/************************/
