

/*
 * Includes
 */

#include <time.h>

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


// SBN Client variable accessors
extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
extern MsgId_to_pipes_t MsgId_Subscriptions[CFE_SBN_CLIENT_MSG_ID_TO_PIPE_ID_MAP_SIZE];


// UT external functions

extern Ut_CFE_FS_ReturnCodeTable_t      Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_MAX_INDEX];
extern Ut_OSFILEAPI_HookTable_t         Ut_OSFILEAPI_HookTable;
extern Ut_CFE_ES_HookTable_t            Ut_CFE_ES_HookTable;
extern Ut_OSAPI_HookTable_t             Ut_OSAPI_HookTable;


void SBN_Client_Setup(void)
{
  // SBN_Client resets
  pipePtr = 0;
  depth = 5;
  memset(PipeTbl, 0, CFE_PLATFORM_SBN_CLIENT_MAX_PIPES);
    
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
**  SBN_Client Tests
**
*******************************************************************************/

/* CFE_SBN_Client_InitPipeTbl Tests */

void Test_CFE_SBN_Client_InitPipeTblFullyIniitializesPipes(void)
{
    /* Arrange */
    int i;
    
    /* Act */ 
    CFE_SBN_Client_InitPipeTbl();
    
    /* Assert */
    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
    {
        UtAssert_True(PipeTbl[i].InUse == CFE_SBN_CLIENT_NOT_IN_USE, ErrorMessage("PipeTbl[%d].InUse should equal %d, but was %d", i, CFE_SBN_CLIENT_NOT_IN_USE, PipeTbl[i].InUse));
        UtAssert_True(PipeTbl[i].SysQueueId == CFE_SBN_CLIENT_UNUSED_QUEUE, ErrorMessage("PipeTbl[%d].SysQueueId should equal %d, but was %d", i, CFE_SBN_CLIENT_UNUSED_QUEUE, PipeTbl[i].SysQueueId));
        UtAssert_True(PipeTbl[i].PipeId == CFE_SBN_CLIENT_INVALID_PIPE, ErrorMessage("PipeTbl[%d].PipeId should equal %d, but was %d", i, CFE_SBN_CLIENT_INVALID_PIPE, PipeTbl[i].PipeId));
        UtAssert_True(strcmp(PipeTbl[i].PipeName, "") == 0, ErrorMessage("PipeTbl[%d].PipeId should equal 'empty string', but was %s", i, PipeTbl[i].PipeName));  
    }

}

void Test_CFE_SBN_Client_InitPipeTblFullyIniitializesMsgIdMap(void)
{
    /* Arrange */
    int i, j;
  
    /* Act */ 
    CFE_SBN_Client_InitPipeTbl();
    
    /* Assert */
    for(i = 0; i < CFE_SBN_CLIENT_MSG_ID_TO_PIPE_ID_MAP_SIZE; i++)
    {
        unsigned int msg_id = MsgId_Subscriptions[i].msgId;
        UtAssert_True(msg_id == CFE_SBN_CLIENT_INVALID_MSG_ID, ErrorMessage("MsgId_Subscriptions[%d] should equal %d, but was %d", i, CFE_SBN_CLIENT_INVALID_MSG_ID, msg_id));
        
        for(j = 0; j < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; j++)
        {
            int pipe_Id = MsgId_Subscriptions[i].pipeIds[j];
            UtAssert_True(pipe_Id == CFE_SBN_CLIENT_INVALID_PIPE, ErrorMessage("MsgId_Subscriptions[%d].pipeIds[%d] should equal %d, but was %d", i, j, CFE_SBN_CLIENT_INVALID_PIPE, pipe_Id));
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

void Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx(void)  //NOTE:not sure if this can really ever occur
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
  UtAssert_True(PipeTbl[0].NextMessage == 0, ErrorMessage("PipeTbl[0].NextMessage should be %d, but was %d", 0, PipeTbl[0].NextMessage));  
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



void Test_WRAP_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse(void)
{
  /* Arrange */
  srand(time(NULL));
  int pipeIdToDelete = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
  PipeTbl[pipeIdToDelete].PipeId = pipeIdToDelete;  
  PipeTbl[pipeIdToDelete].InUse = CFE_SBN_CLIENT_IN_USE;
  
  /* Act */ 
  int32 result = CFE_SB_DeletePipe(pipeIdToDelete);
  
  /* Assert */
  UtAssert_True(result == CFE_SUCCESS, ErrorMessage("Call to CFE_SB_DeletePipe to delete pipe#%d result should be %d, but was %d", pipeIdToDelete, CFE_SUCCESS, result));
}

void Test_starter(void)
{
    /* Arrange */
    
  
    /* Act */ 
    
  
    /* Assert */
    
}


/*************************************************/

void SBN_Client_Test_AddTestCases(void)
{
    /* CFE_SBN_Client_InitPipeTbl Tests */
    UtTest_Add(Test_CFE_SBN_Client_InitPipeTblFullyIniitializesPipes, SBN_Client_Setup, SBN_Client_Teardown, "Test_CFE_SBN_Client_InitPipeTblFullyIniitializesPipes");
    UtTest_Add(Test_CFE_SBN_Client_InitPipeTblFullyIniitializesMsgIdMap, SBN_Client_Setup, SBN_Client_Teardown, "Test_CFE_SBN_Client_InitPipeTblFullyIniitializesMsgIdMap");

    /* CFE_SBN_Client_GetPipeIdx Tests */
    UtTest_Add(Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx, SBN_Client_Setup, SBN_Client_Teardown, "Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx");
    UtTest_Add(Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx, SBN_Client_Setup, SBN_Client_Teardown, "Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx");

    /* Wrap_CFE_SB_CreatePipe Tests */
    UtTest_Add(Test_Wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS");
    UtTest_Add(Test_Wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly");
    UtTest_Add(Test_Wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull, SBN_Client_Setup, SBN_Client_Teardown, "Test_Wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull");
    
    /* WRAP_CFE_SB_DeletePipe Tests */
    UtTest_Add(Test_WRAP_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse, SBN_Client_Setup, SBN_Client_Teardown, "Test_WRAP_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse");
    
}