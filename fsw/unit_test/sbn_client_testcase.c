

/*
 * Includes
 */

#include <time.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <limits.h>

#include "cfe.h"
#include "utassert.h"
#include "uttest.h"
#include "utlist.h"
#include "sbn_client.h"

#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_tbl_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_sb_hooks.h"
#include "ut_cfe_es_stubs.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_fs_stubs.h"

#include "sbn_client_version.h"

#define MAX_ERROR_MESSAGE_SIZE 100


void SBN_Client_Setup(void);
void SBN_Client_Teardown(void);


// Global variables for testing
char em[MAX_ERROR_MESSAGE_SIZE];
CFE_SB_PipeId_t pipePtr;
uint16 depth;
const char *pipeName = "TestPipe";
time_t random_gen;

// Real Functions
int __real_connect_to_server(const char *, uint16_t);

// Use real function hooks
boolean use_real_connect_to_server = TRUE;

// Wrapped Functions
int __wrap_socket(int, int, int);
uint16_t __wrap_htons(uint16_t);
int __wrap_inet_pton(int, const char *, void*);
int __wrap_connect(int, const struct sockaddr *, socklen_t);
int __wrap_connect_to_server(const char *, uint16_t);

// Wrapped Functions return values
int wrap_socket_return_value;
uint16_t wrap_htons_return_value;
int wrap_inet_pton_return_value;
int wrap_connect_return_value;
int wrap_connect_to_server_return_value;


// SBN Client variable accessors
extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
extern MsgId_to_pipes_t MsgId_Subscriptions[CFE_SBN_CLIENT_MSG_ID_TO_PIPE_ID_MAP_SIZE];


// UT external functions
extern Ut_CFE_FS_ReturnCodeTable_t      Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_MAX_INDEX];
extern Ut_OSFILEAPI_HookTable_t         Ut_OSFILEAPI_HookTable;
extern Ut_CFE_ES_HookTable_t            Ut_CFE_ES_HookTable;
extern Ut_OSAPI_HookTable_t             Ut_OSAPI_HookTable;

void Test_Group_Setup(void)
{
    random_gen = time(NULL);
    srand(random_gen);
    
    printf("Random Seed = %d\n", random_gen);
}

void Test_Group_Teardown(void)
{
    printf("Random Seed = %d\n", random_gen);
}

void SBN_Client_Setup(void)
{
  // SBN_Client resets
  pipePtr = 0;
  depth = 5;
  wrap_socket_return_value = (rand() % INT_MIN) * -1;
  wrap_htons_return_value = 0;
  wrap_inet_pton_return_value = 1;
  wrap_connect_return_value = -1;
  
  memset(PipeTbl, 0, sizeof(PipeTbl));
    
  // Global UT CFE resets -- NOTE: not sure if these are required for sbn_client
  Ut_OSAPI_Reset();
  Ut_CFE_SB_Reset();
  Ut_CFE_ES_Reset();
  Ut_CFE_EVS_Reset();
  Ut_CFE_TBL_Reset();
}

void SBN_Client_Teardown(void)
{
  ;
}

char *ErrorMessage(const char *format, ...)
{
  va_list vl;
  va_start(vl, format);
   
  vsnprintf(em, MAX_ERROR_MESSAGE_SIZE, format, vl);
  
  va_end(vl);
  
  return em;
}

/*******************************************************************************
**
**  Wrapped Functions
**
*******************************************************************************/

__wrap_socket(int domain, int type, int protocol)
{
  return wrap_socket_return_value;
}

uint16_t __wrap_htons(uint16_t hostshort)
{
  return wrap_htons_return_value;
}

int __wrap_inet_pton(int af, const char *src, void *dst)
{
  return wrap_inet_pton_return_value;
}

int __wrap_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  return wrap_connect_return_value;
}



/*******************************************************************************
**
**  SBN_Client Tests
**
*******************************************************************************/


/* connect_to_server Tests */

void Test_connect_to_server_returns_sockfd_when_successful(void)
{
  /* Arrange */
  wrap_socket_return_value = rand() % INT_MAX;
  wrap_htons_return_value = 0;
  wrap_inet_pton_return_value = 1;
  wrap_connect_return_value = 0;
    
  /* Act */ 
  /* NULL can be used in the test call because all usages in the CUT are wrapped
  ** calls */
  int result = connect_to_server(NULL, NULL);
  
  /* Assert */
  UtAssert_True(result == wrap_socket_return_value, 
    ErrorMessage("sockfd returned should have been %d, but was %d", 
    wrap_socket_return_value, result));
}

void Test_connect_to_server_returns_error_when_socket_fails(void)
{
  /* Arrange */
  /* once socket fails CUT returns error */
  wrap_socket_return_value = (rand() % INT_MIN) * -1;
    
  /* Act */ 
  /* NULL can be used in the test call because all usages in the CUT are wrapped
  ** calls */
  int result = connect_to_server(NULL, NULL);
  
  /* Assert */
  UtAssert_True(result == -1, 
    ErrorMessage("error returned should have been %d, but was %d", -1, result));
}

void Test_connect_to_server_returns_error_when_inet_pton_src_is_invalid(void)
{
  /* Arrange */
  /* once inet_pton fails CUT returns error */
  wrap_socket_return_value = rand() % INT_MAX;
  wrap_htons_return_value = 0;
  wrap_inet_pton_return_value = 0;
    
  /* Act */ 
  /* NULL can be used in the test call because all usages in the CUT are wrapped
  ** calls */
  int result = connect_to_server(NULL, NULL);
  
  /* Assert */
  UtAssert_True(result == -2, 
    ErrorMessage("error returned should have been %d, but was %d", -2, result));
}

void Test_connect_to_server_returns_error_when_inet_pton_af_is_invalid(void)
{
  /* Arrange */
  /* once inet_pton fails CUT returns error */
  wrap_socket_return_value = rand() % INT_MAX;
  wrap_htons_return_value = 0;
  wrap_inet_pton_return_value = -1;
    
  /* Act */ 
  /* NULL can be used in the test call because all usages in the CUT are wrapped
  ** calls */
  int result = connect_to_server(NULL, NULL);
  
  /* Assert */
  UtAssert_True(result == -3, 
    ErrorMessage("error returned should have been %d, but was %d", -3, result));
}

void Test_connect_to_server_returns_error_when_connect_fails(void)
{
  /* Arrange */
  wrap_socket_return_value = rand() % INT_MAX;
  wrap_htons_return_value = 0;
  wrap_inet_pton_return_value = 1;
  wrap_connect_return_value = (rand() % INT_MIN) * -1;
    
  /* Act */ 
  /* NULL can be used in the test call because all usages in the CUT are wrapped
  ** calls */
  int result = connect_to_server(NULL, NULL);
  
  /* Assert */
  UtAssert_True(result == -4, 
    ErrorMessage("error returned should have been %d, but was %d", -4, result));
}


/* CFE_SBN_Client_InitPipeTbl Tests */

void Test_CFE_SBN_Client_InitPipeTblFullyIniitializesPipes(void)
{
    /* Arrange */
    int i, j;
    
    /* Act */ 
    CFE_SBN_Client_InitPipeTbl();
    
    /* Assert */
    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
    {
        CFE_SBN_Client_PipeD_t test_pipe = PipeTbl[i];
        
        UtAssert_True(test_pipe.InUse == CFE_SBN_CLIENT_NOT_IN_USE, ErrorMessage("PipeTbl[%d].InUse should equal %d, but was %d", i, CFE_SBN_CLIENT_NOT_IN_USE, PipeTbl[i].InUse));
        UtAssert_True(test_pipe.SysQueueId == CFE_SBN_CLIENT_UNUSED_QUEUE, ErrorMessage("PipeTbl[%d].SysQueueId should equal %d, but was %d", i, CFE_SBN_CLIENT_UNUSED_QUEUE, PipeTbl[i].SysQueueId));
        UtAssert_True(test_pipe.PipeId == CFE_SBN_CLIENT_INVALID_PIPE, ErrorMessage("PipeTbl[%d].PipeId should equal %d, but was %d", i, CFE_SBN_CLIENT_INVALID_PIPE, PipeTbl[i].PipeId));
        UtAssert_True(test_pipe.NumberOfMessages == 1, ErrorMessage("PipeTbl[%d].NumberOfMessages should equal %d, but was %d", i, 1, PipeTbl[i].NumberOfMessages));
        UtAssert_True(test_pipe.ReadMessage == (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1), ErrorMessage("PipeTbl[%d].NumberOfMessages should equal %d, but was %d", i, (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1), PipeTbl[i].ReadMessage));
        UtAssert_True(strcmp(test_pipe.PipeName, "") == 0, ErrorMessage("PipeTbl[%d].PipeId should equal 'empty string', but was %s", i, PipeTbl[i].PipeName));  
    
        for(j = 0; j < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; j++)
        {
            UtAssert_True(test_pipe.SubscribedMsgIds[j] == CFE_SBN_CLIENT_INVALID_MSG_ID, ErrorMessage("PipeTbl[%d].SubscribedMsgIds[%d] should be %d, but was %d", i, j, CFE_SBN_CLIENT_INVALID_MSG_ID, test_pipe.SubscribedMsgIds[j]));
        }
        
    }

}

/* CFE_SBN_Client_GetPipeIdx Tests */

void Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx(void)
{
    /* Arrange */
    CFE_SB_PipeId_t pipe = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    PipeTbl[pipe].InUse = CFE_SBN_CLIENT_IN_USE;    
    PipeTbl[pipe].PipeId = pipe;
  
    /* Act */ 
    uint8 result = CFE_SBN_Client_GetPipeIdx(pipe);
  
    /* Assert */
    UtAssert_True(result == pipe, ErrorMessage("CFE_SBN_Client_GetPipeIdx should have returned %d, but was %d", pipe, result));
}

void Test_CFE_SBN_Client_GetPiTest_connect_to_server_returns_error_when_connect_failspeIdxSuccessPipeIdDoesNotEqualPipeIdx(void)  //NOTE:not sure if this can really ever occur
{
    /* Arrange */
    CFE_SB_PipeId_t pipe = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    PipeTbl[pipe].InUse = CFE_SBN_CLIENT_NOT_IN_USE;    
    PipeTbl[pipe].PipeId = CFE_SBN_CLIENT_INVALID_PIPE;
    uint8 tblIdx = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    
    while (tblIdx == pipe) // get a pipe that is not the same as index
    {
      tblIdx = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    }
    
    PipeTbl[tblIdx].InUse = CFE_SBN_CLIENT_IN_USE;    
    PipeTbl[tblIdx].PipeId = pipe;
    
    /* Act */ 
    uint8 result = CFE_SBN_Client_GetPipeIdx(pipe);
  
    /* Assert */
    UtAssert_True(result == tblIdx, ErrorMessage("CFE_SBN_Client_GetPipeIdx for pipeId %d should have returned %d, but was %d", pipe, tblIdx, result));
}

/* Wrap_CFE_SB_CreatePipe Tests */

void Test_Wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS(void)
{
  /* Arrange */
  
  
  /* Act */ 
  int32 result = CFE_SB_CreatePipe(&pipePtr, depth, pipeName);
  
  /* Assert */
  UtAssert_True(result == CFE_SUCCESS, ErrorMessage("Pipe creation should have succeeded (= %d), but failed (= %d)", CFE_SUCCESS, result));
}

void Test_Wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly(void)
{
  /* Arrange */
  
  
  /* Act */ 
  CFE_SB_CreatePipe(&pipePtr, depth, pipeName);
  
  /* Assert */
  UtAssert_True(pipePtr == 0, ErrorMessage("PipePtr should point to pipe 0 (initial pipe), but instead points to pipe %d.", pipePtr));
  UtAssert_True(PipeTbl[0].InUse == CFE_SBN_CLIENT_IN_USE, ErrorMessage("PipeTbl[0].InUse should be %d, but was %d", CFE_SBN_CLIENT_IN_USE, PipeTbl[0].InUse));
  UtAssert_True(PipeTbl[0].PipeId == 0, ErrorMessage("PipeTbl[0].PipeID should be %d, but was %d", 0, PipeTbl[0].PipeId));
  UtAssert_True(PipeTbl[0].SendErrors == 0, ErrorMessage("PipeTbl[0].SendErrors should be %d, but was %d", 0, PipeTbl[0].SendErrors));
  UtAssert_True(strcmp(&PipeTbl[0].PipeName[0], pipeName) == 0, ErrorMessage("PipeTbl[0].PipeName should be %s, but was %s", pipeName, PipeTbl[0].PipeName));
  UtAssert_True(PipeTbl[0].NumberOfMessages == 0, ErrorMessage("PipeTbl[0].NumberOfMessages should be %d, but was %d", 0, PipeTbl[0].NumberOfMessages));
  UtAssert_True(PipeTbl[0].ReadMessage == 0, ErrorMessage("PipeTbl[0].ReadMessage should be %d, but was %d", 0, PipeTbl[0].ReadMessage));  
}

void Test_Wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull(void)
{
  /* Arrange */
  int i;
  uint32 initial_event_q_depth = Ut_CFE_EVS_GetEventQueueDepth();
  
  for (i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
  {
    PipeTbl[i].InUse = CFE_SBN_CLIENT_IN_USE;
  }
  
  /* Act */ 
  int32 result = CFE_SB_CreatePipe(&pipePtr, depth, pipeName);
  uint32 current_event_q_depth = Ut_CFE_EVS_GetEventQueueDepth();
  char *expected_error_msg = ErrorMessage("CreatePipeErr:Max Pipes(%d)In Use.app %s", CFE_PLATFORM_SBN_CLIENT_MAX_PIPES, APP_NAME);
  
  
  /* Assert */
  UtAssert_True(result == CFE_SBN_CLIENT_MAX_PIPES_MET, ErrorMessage("Call to CFE_SB_CreatePipe result should be %d, but was %d", CFE_SBN_CLIENT_CR_PIPE_ERR_EID, result));
  //TODO:set stubs to intercept wraps on CFE calls
  //UtAssert_True(current_event_q_depth == initial_event_q_depth + 1, ErrorMessage("Event queue count should be %d, but was %d", initial_event_q_depth + 1, current_event_q_depth));
  //UtAssert_EventSent(CFE_SBN_CLIENT_MAX_PIPES_MET, CFE_EVS_ERROR, expected_error_msg, 
  //  ErrorMessage("Error event as expected was not sent. Expected: Error = %d, ErrorType=%d, Error Message = %s", CFE_SBN_CLIENT_MAX_PIPES_MET, CFE_EVS_ERROR, expected_error_msg));
}

/* WRAP_CFE_SB_DeletePipe Tests */
void Test_WRAP_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse(void)
{
  /* Arrange */
  int pipeIdToDelete = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
  PipeTbl[pipeIdToDelete].PipeId = pipeIdToDelete;  
  PipeTbl[pipeIdToDelete].InUse = CFE_SBN_CLIENT_IN_USE;
  
  /* Act */ 
  int32 result = CFE_SB_DeletePipe(pipeIdToDelete);
  
  /* Assert */
  UtAssert_True(result == CFE_SUCCESS, ErrorMessage("Call to CFE_SB_DeletePipe to delete pipe#%d result should be %d, but was %d", pipeIdToDelete, CFE_SUCCESS, result));
}

/* Wrap_CFE_SB_Subscribe Tests */
void Test_Wrap_CFE_SB_SubscribeSuccessWhenPipeIsValidAndMsgIdUnsubscribedAndNotAtMaxMsgIds(void)
{
    /* Arrange */
    int i;
    int num_msgIds_subscribed = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    int pipe_id = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    int msg_id = 0x1800;
    int other_msg_id = 0x1801;
    PipeTbl[pipe_id].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_id].PipeId = pipe_id;
    
    for (i = 0; i < num_msgIds_subscribed; i++)
    {
        PipeTbl[pipe_id].SubscribedMsgIds[i] = other_msg_id + i;
    }  
      
    for (i = num_msgIds_subscribed; i < (CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE - num_msgIds_subscribed); i++)
    {
        PipeTbl[pipe_id].SubscribedMsgIds[i] == CFE_SBN_CLIENT_INVALID_MSG_ID;
    }
    
    /* Act */ 
    int32 result = CFE_SB_Subscribe(msg_id, pipe_id);
    
    /* Assert */
    UtAssert_True(result == CFE_SUCCESS, ErrorMessage("Call to CFE_SB_Subscribe should return %d, but was %d", CFE_SUCCESS, result));
    UtAssert_True(PipeTbl[pipe_id].SubscribedMsgIds[num_msgIds_subscribed] == msg_id, ErrorMessage("PipeTble[%d].SubscribedMsgIds[%d] should be %d, but was %d", pipe_id, num_msgIds_subscribed, msg_id, PipeTbl[pipe_id].SubscribedMsgIds[num_msgIds_subscribed]));
    
}

void Test_Wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid(void)
{
    /* Arrange */
    int pipe_id = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    int msg_id = 0x1800;
    PipeTbl[pipe_id].InUse = CFE_SBN_CLIENT_NOT_IN_USE;
    PipeTbl[pipe_id].PipeId = CFE_SBN_CLIENT_INVALID_PIPE;
    
    /* Act */ 
    int32 result = CFE_SB_Subscribe(msg_id, pipe_id);
        
    /* Assert */
    UtAssert_True(result == CFE_SBN_CLIENT_BAD_ARGUMENT, ErrorMessage("Call to CFE_SB_Subscribe with pipeId %d should return error %d, but was %d", pipe_id, CFE_SBN_CLIENT_BAD_ARGUMENT, result));
    
}

void Test_Wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded(void)
{
    /* Arrange */
    int i;
    int pipe_id = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    int msg_id = 0x1800;
    int other_msg_id = 0x1801;
    PipeTbl[pipe_id].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_id].PipeId = pipe_id;

    for (i = 0; i < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; i++)
    {
        PipeTbl[pipe_id].SubscribedMsgIds[i] = other_msg_id + i;
        printf("PipeTbl[%d].SubscribedMsgIds[%d] = %d\n", pipe_id,i,PipeTbl[pipe_id].SubscribedMsgIds[i]);
    }
    
    /* Act */ 
    int32 result = CFE_SB_Subscribe(msg_id, pipe_id);
        
    /* Assert */
    UtAssert_True(result == CFE_SBN_CLIENT_BAD_ARGUMENT, ErrorMessage("Call to CFE_SB_Subscribe with pipeId %d should return error %d, but was %d", pipe_id, CFE_SBN_CLIENT_BAD_ARGUMENT, result));
    
}

/* ingest_app_message Tests */
void Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft(void)
{
    /* Arrange */
    int p[2]; pipe(p);
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("read_msg = %d\n", read_msg);
    int num_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1; /* 1 slot left */
    printf("num_msg = %d\n", num_msg);
    int msg_slot;
    
    write(p[1], msg, msgSize);
    close(p[1]);
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = 0x1881;
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    msg_slot = read_msg + num_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    printf("msg_slot = %d\n", msg_slot);
    
    /* Act */ 
    ingest_app_message(p[0], msgSize);
    
    /* Assert */
    int i;
    
    for(i = 0; i < msgSize; i++)
    {
        UtAssert_True(PipeTbl[pipe_assigned].Messages[msg_slot][i] == msg[i], ErrorMessage("PipeTbl[%d].Messages[%d][%d] should = %d, but was ", pipe_assigned, msg_slot, i, msg[i], PipeTbl[pipe_assigned].Messages[0][i]));
    }  
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH, ErrorMessage("PipeTbl[%d].NumberOfMessages should = %d, but was ", pipe_assigned, CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, ErrorMessage("PipeTbl[%d].ReadMessage should not have changed from %d, but was ", pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage));  
}

void Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable(void)
{
    /* Arrange */
    int p[2]; pipe(p);
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("read_msg = %d\n", read_msg);
    int num_msg = (rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1) + 1; /* from 1 to CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH */
    printf("num_msg = %d\n", num_msg);
    int msg_slot;
    
    write(p[1], msg, msgSize);
    close(p[1]);
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = 0x1881;
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    msg_slot = read_msg + num_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    printf("msg_slot = %d\n", msg_slot);
    
    /* Act */ 
    ingest_app_message(p[0], msgSize);
    
    /* Assert */
    int i;
    
    for(i = 0; i < msgSize; i++)
    {
        UtAssert_True(PipeTbl[pipe_assigned].Messages[msg_slot][i] == msg[i], ErrorMessage("PipeTbl[%d].Messages[%d][%d] should = %d, but was ", pipe_assigned, msg_slot, i, msg[i], PipeTbl[pipe_assigned].Messages[0][i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg + 1, ErrorMessage("PipeTbl[%d].NumberOfMessages should have increased by 1 to %d, but was ", pipe_assigned, num_msg + 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, ErrorMessage("PipeTbl[%d].ReadMessage should not have changed from %d, but was ", pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage)); 
}

void Test_ingest_app_message_SuccessAllSlotsAvailable(void)
{
    /* Arrange */
    int p[2]; pipe(p);
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("read_msg = %d\n", read_msg);
    int num_msg = 0;
    printf("num_msg = %d\n", num_msg);
    int msg_slot;
    
    write(p[1], msg, msgSize);
    close(p[1]);
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = 0x1881;
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    msg_slot = read_msg + num_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    printf("msg_slot = %d\n", msg_slot);
    
    /* Act */ 
    ingest_app_message(p[0], msgSize);
    
    /* Assert */
    int i;
    
    for(i = 0; i < msgSize; i++)
    {
        UtAssert_True(PipeTbl[pipe_assigned].Messages[msg_slot][i] == msg[i], ErrorMessage("PipeTbl[%d].Messages[%d][%d] should = %d, but was ", pipe_assigned, msg_slot, i, msg[i], PipeTbl[pipe_assigned].Messages[0][i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg + 1, ErrorMessage("PipeTbl[%d].NumberOfMessages should have increased by 1 to %d, but was ", pipe_assigned, num_msg + 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, ErrorMessage("PipeTbl[%d].ReadMessage should not have changed from %d, but was ", pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage)); 
}

void Test_ingest_app_message_FailsWhenNumberOfMessagesIsFull(void)
{
    /* Arrange */
    int p[2]; pipe(p);
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("read_msg = %d\n", read_msg);
    int num_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("num_msg = %d\n", num_msg);
    int msg_slot;
    
    write(p[1], msg, msgSize);
    close(p[1]);
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = 0x1881;
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    msg_slot = read_msg + num_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    printf("msg_slot = %d\n", msg_slot);
    
    /* Act */ 
    ingest_app_message(p[0], msgSize);
    
    /* Assert */
    /* TODO:add failure assert here */
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg, ErrorMessage("PipeTbl[%d].NumberOfMessages %d should not increase because of failure, but was %d", pipe_assigned, num_msg, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, ErrorMessage("PipeTbl[%d].ReadMessage should not have changed from %d, but was ", pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage)); 
}

//void Test_ingest_app_message_SuccessCausesPipeNumberOfMessagesToIncreaseBy1
//void Test_ingest_app_message_FailsWhenNoPipesInUse
//void Test_ingest_app_message_FailsWhenNoPipeLookingForMessageId

/* Wrap_CFE_SB_RcvMsg Tests */
void Test_Wrap_CFE_SB_RcvMsg_SuccessPipeIsFull(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int previous_read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("previous_read_msg = %d\n", previous_read_msg);
    int current_read_msg = (previous_read_msg + 1) % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wrap to 0 if necessary */
    printf("current_read_msg = %d\n", current_read_msg);
    int num_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; 
    printf("num_msg = %d\n", num_msg);
    CFE_SB_MsgPtr_t buffer;

    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = 0x1881;
    pipe->NumberOfMessages = num_msg;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, 5000);

    /* Assert */
    int i = 0;
    
    UtAssert_True(result == CFE_SUCCESS, ErrorMessage("__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", CFE_SUCCESS, result));
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].Messages[current_read_msg][i], ErrorMessage("buffer[%d] should = %d, but was %d", i, PipeTbl[pipe_assigned].Messages[current_read_msg][i], buffer[i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, ErrorMessage("PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d, but is %d", pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, ErrorMessage("PipeTbl[%d].ReadMessage should have progressed to %d from %d, but is %d", pipe_assigned, current_read_msg, previous_read_msg, PipeTbl[pipe_assigned].ReadMessage));
}

void Test_Wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int previous_read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("previous_read_msg = %d\n", previous_read_msg);
    int current_read_msg = (previous_read_msg + 1) % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wrap to 0 if necessary */
    printf("current_read_msg = %d\n", current_read_msg);
    int num_msg = (rand() % (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1)) + 2; 
    printf("num_msg = %d\n", num_msg);
    CFE_SB_MsgPtr_t buffer;

    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = 0x1881;
    pipe->NumberOfMessages = num_msg;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, 5000);

    /* Assert */
    int i = 0;
    
    UtAssert_True(result == CFE_SUCCESS, ErrorMessage("__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", CFE_SUCCESS, result));
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].Messages[current_read_msg][i], ErrorMessage("buffer[%d] should = %d, but was %d", i, PipeTbl[pipe_assigned].Messages[current_read_msg][i], buffer[i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, ErrorMessage("PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d, but is %d", pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, ErrorMessage("PipeTbl[%d].ReadMessage should have progressed to %d from %d, but is %d", pipe_assigned, current_read_msg, previous_read_msg, PipeTbl[pipe_assigned].ReadMessage));
}

void Test_Wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int previous_read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("previous_read_msg = %d\n", previous_read_msg);
    int current_read_msg = (previous_read_msg + 1) % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wrap to 0 if necessary */
    printf("current_read_msg = %d\n", current_read_msg);
    int num_msg = 2; 
    printf("num_msg = %d\n", num_msg);
    CFE_SB_MsgPtr_t buffer;

    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = 0x1881;
    pipe->NumberOfMessages = num_msg;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, 5000);

    /* Assert */
    int i = 0;
    
    UtAssert_True(result == CFE_SUCCESS, ErrorMessage("__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", CFE_SUCCESS, result));
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].Messages[current_read_msg][i], ErrorMessage("buffer[%d] should = %d, but was %d", i, PipeTbl[pipe_assigned].Messages[current_read_msg][i], buffer[i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, ErrorMessage("PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d, but is %d", pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, ErrorMessage("PipeTbl[%d].ReadMessage should have progressed to %d from %d, but is %d", pipe_assigned, current_read_msg, previous_read_msg, PipeTbl[pipe_assigned].ReadMessage));
}

void Test_Wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int previous_read_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1;
    printf("previous_read_msg = %d\n", previous_read_msg);
    int current_read_msg = 0; /* auto wrap to 0 if necessary */
    printf("current_read_msg = %d\n", current_read_msg);
    int num_msg = (rand() % (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1)) + 2; 
    printf("num_msg = %d\n", num_msg);
    CFE_SB_MsgPtr_t buffer;

    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = 0x1881;
    pipe->NumberOfMessages = num_msg;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, 5000);

    /* Assert */
    int i = 0;
    
    UtAssert_True(result == CFE_SUCCESS, ErrorMessage("__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", CFE_SUCCESS, result));
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].Messages[current_read_msg][i], ErrorMessage("buffer[%d] should = %d, but was %d", i, PipeTbl[pipe_assigned].Messages[current_read_msg][i], buffer[i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, ErrorMessage("PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d, but is %d", pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, ErrorMessage("PipeTbl[%d].ReadMessage should have progressed to %d from %d, but is %d", pipe_assigned, current_read_msg, previous_read_msg, PipeTbl[pipe_assigned].ReadMessage));
}

//TODO: Test_Wrap_CFE_SB_RcvMsgSuccess when num messages = 1
//TODO: Test_Wrap_CFE_SB_RcvMsgSuccess when num messages = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH
//TODO: Test_Wrap_CFE_SB_RcvMsgFail when num messages = 0

/* SBN_ClientInit Tests */
/* needs more setup to be done correctly */
// void Test_SBN_ClientInitSuccess(void)
// {
//     /* Arrange */
//     /* connect_to_server call control */
//     wrap_socket_return_value = rand() % INT_MAX;
//     wrap_htons_return_value = 0;
//     wrap_inet_pton_return_value = 1;
//     wrap_connect_return_value = 0;
// 
//     /* Act */ 
//     int32 result = SBN_ClientInit();
// 
//     /* Assert */
//     UtAssert_True(result == OS_SUCCESS, ErrorMessage("SBN_ClientInit result should be %d, but was %d", OS_SUCCESS, result));
// }

void Test_starter(void)
{
    /* Arrange */
    
    
    /* Act */ 
    
    
    /* Assert */
    
}


/*************************************************/

void SBN_Client_Test_AddTestCases(void)
{
    UtTest_Add(Test_Group_Setup, NULL, NULL, "Test_Group_Setup");
    
    /* SBN_ClientInit Tests */
    //UtTest_Add(Test_SBN_ClientInitSuccess, SBN_Client_Setup, SBN_Client_Teardown, "Test_SBN_ClientInitSuccess");
    
    /* connect_to_server Tests */
    add_connect_to_server_tests();
    
    /* CFE_SBN_Client_InitPipeTbl Tests */
    UtTest_Add(Test_CFE_SBN_Client_InitPipeTblFullyIniitializesPipes, SBN_Client_Setup, SBN_Client_Teardown, "Test_CFE_SBN_Client_InitPipeTblFullyIniitializesPipes");
    
    /* CFE_SBN_Client_GetPipeIdx Tests */
    UtTest_Add(Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx, SBN_Client_Setup, SBN_Client_Teardown, "Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx");
    //UtTest_Add(Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx, SBN_Client_Setup, SBN_Client_Teardown, "Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx");
    
    /* Wrap_CFE_SB_CreatePipe Tests */
    /* create pipe tests will not run with SBN_ClientInit enabled, needs more setup */
    UtTest_Add(Test_Wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS");
    UtTest_Add(Test_Wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly");
    UtTest_Add(Test_Wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull");
    
    /* WRAP_CFE_SB_DeletePipe Tests */
    UtTest_Add(Test_WRAP_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse, SBN_Client_Setup, SBN_Client_Teardown, "Test_WRAP_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse");
    
    /* Wrap_CFE_SB_Subscribe Tests */
    UtTest_Add(Test_Wrap_CFE_SB_SubscribeSuccessWhenPipeIsValidAndMsgIdUnsubscribedAndNotAtMaxMsgIds, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_SubscribeSuccessWhenPipeIsValidAndMsgIdUnsubscribedAndNotAtMaxMsgIds");
    UtTest_Add(Test_Wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid");
    UtTest_Add(Test_Wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded");
    
    /* ingest_app_message Tests */
    UtTest_Add(Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft, SBN_Client_Setup, SBN_Client_Teardown, "Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft");
    UtTest_Add(Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable, SBN_Client_Setup, SBN_Client_Teardown, "Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable");
    UtTest_Add(Test_ingest_app_message_SuccessAllSlotsAvailable, SBN_Client_Setup, SBN_Client_Teardown, "Test_ingest_app_message_SuccessAllSlotsAvailable");
    UtTest_Add(Test_ingest_app_message_FailsWhenNumberOfMessagesIsFull, SBN_Client_Setup, SBN_Client_Teardown, "Test_ingest_app_message_FailsWhenNumberOfMessagesIsFull");
    
    /* Wrap_CFE_SB_RcvMsg Tests */
    UtTest_Add(Test_Wrap_CFE_SB_RcvMsg_SuccessPipeIsFull, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_RcvMsg_SuccessPipeIsFull");
    UtTest_Add(Test_Wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe");
    UtTest_Add(Test_Wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe");
    UtTest_Add(Test_Wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe");
    
    /* Group Teardown */
    UtTest_Add(Test_Group_Setup, NULL, NULL, "Test_Group_Setup");
}

/* Helper Functions */

void add_connect_to_server_tests(void)
{
    UtTest_Add(Test_connect_to_server_returns_sockfd_when_successful, SBN_Client_Setup, SBN_Client_Teardown, "Test_connect_to_server_returns_sockfd_when_successful");
    UtTest_Add(Test_connect_to_server_returns_error_when_socket_fails, SBN_Client_Setup, SBN_Client_Teardown, "Test_connect_to_server_returns_error_when_socket_fails");
    UtTest_Add(Test_connect_to_server_returns_error_when_inet_pton_src_is_invalid, SBN_Client_Setup, SBN_Client_Teardown, "Test_connect_to_server_returns_error_when_inet_pton_src_is_invalid");
    UtTest_Add(Test_connect_to_server_returns_error_when_inet_pton_af_is_invalid, SBN_Client_Setup, SBN_Client_Teardown, "Test_connect_to_server_returns_error_when_inet_pton_af_is_invalid");
    UtTest_Add(Test_connect_to_server_returns_error_when_connect_fails, SBN_Client_Setup, SBN_Client_Teardown, "Test_connect_to_server_returns_error_when_connect_fails");  
}