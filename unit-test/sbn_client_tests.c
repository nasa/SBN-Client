#include "sbn_client_tests_includes.h"

/*******************************************************************************
**
**  SBN_Client_Tests Setup and Teardown
**
*******************************************************************************/

void SBN_Client_Tests_Setup(void)
{
    SBN_Client_Setup();
}

void SBN_Client_Tests_Teardown(void)
{
    SBN_Client_Teardown();
}

/*******************************************************************************
**
**  CFE_SBN_Client_InitPipeTbl Tests
**
*******************************************************************************/

void Test_CFE_SBN_Client_InitPipeTblFullyInitializesPipes(void)
{
    /* Arrange */
    int i, j;
    
    /* Act */ 
    CFE_SBN_Client_InitPipeTbl();
    
    /* Assert */
    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
    {
        CFE_SBN_Client_PipeD_t test_pipe = PipeTbl[i];
        
        UtAssert_True(test_pipe.InUse == CFE_SBN_CLIENT_NOT_IN_USE, 
          TestResultMsg("PipeTbl[%d].InUse should equal %d and was %d", i, 
          CFE_SBN_CLIENT_NOT_IN_USE, PipeTbl[i].InUse));
        UtAssert_True(test_pipe.SysQueueId == CFE_SBN_CLIENT_UNUSED_QUEUE, 
          TestResultMsg("PipeTbl[%d].SysQueueId should equal %d and was %d", i, 
          CFE_SBN_CLIENT_UNUSED_QUEUE, PipeTbl[i].SysQueueId));
        UtAssert_True(test_pipe.PipeId == CFE_SBN_CLIENT_INVALID_PIPE, 
          TestResultMsg("PipeTbl[%d].PipeId should equal %d and was %d", i, 
          CFE_SBN_CLIENT_INVALID_PIPE, PipeTbl[i].PipeId));
        UtAssert_True(test_pipe.NumberOfMessages == 1, TestResultMsg(
          "PipeTbl[%d].NumberOfMessages should equal %d and was %d", i, 1, 
          PipeTbl[i].NumberOfMessages));
        UtAssert_True(test_pipe.ReadMessage == 
          (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1), TestResultMsg(
          "PipeTbl[%d].NumberOfMessages should equal %d and was %d", i, 
          (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1), 
          PipeTbl[i].ReadMessage));
        UtAssert_True(strcmp(test_pipe.PipeName, "") == 0, 
          TestResultMsg("PipeTbl[%d].PipeId should equal '' and was '%s'", i, 
          PipeTbl[i].PipeName));  
    
        for(j = 0; j < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; j++)
        {
            UtAssert_True(test_pipe.SubscribedMsgIds[j] == 
              CFE_SBN_CLIENT_INVALID_MSG_ID, TestResultMsg(
              "PipeTbl[%d].SubscribedMsgIds[%d] should be %d and was %d", i, j, 
              CFE_SBN_CLIENT_INVALID_MSG_ID, test_pipe.SubscribedMsgIds[j]));
        }
        
    }

}
/* end CFE_SBN_Client_InitPipeTbl Tests */


/*******************************************************************************
**
**  CFE_SBN_Client_GetAvailPipeIdx Tests
**
*******************************************************************************/

void Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsErrorWhenAllPipesUsed(void)
{
    /* Arrange */
    int i;
    CFE_SB_PipeId_t result;
    for(i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
    {
        PipeTbl[i].InUse = CFE_SBN_CLIENT_IN_USE;
    }
    
    /* Act */ 
    result = CFE_SBN_Client_GetAvailPipeIdx();
    
    /* Assert */
    UtAssert_True(result == CFE_SBN_CLIENT_INVALID_PIPE, 
        "CFE_SBN_Client_GetAvailPipeIdx returned CFE_SBN_CLIENT_INVALID_PIPE");
    
}

void Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsIndexForFirstOpenPipe(void)
{
    /* Arrange */
    int i;
    /* 0 to CFE_PLATFORM_SBN_CLIENT_MAX_PIPES */
    int available_index = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; 
    CFE_SB_PipeId_t result;
    for(i = 0; i < available_index; i++)
    {
        PipeTbl[i].InUse = CFE_SBN_CLIENT_IN_USE;
    }
    
    /* Act */ 
    result = CFE_SBN_Client_GetAvailPipeIdx();
    
    /* Assert */
    UtAssert_True(result == available_index, TestResultMsg(
      "CFE_SBN_Client_GetAvailPipeIdx should have returned %d and returned %d", 
      available_index, result));
}

/*************************************************/

void UtTest_Setup(void)
{
    /* CFE_SBN_Client_InitPipeTbl Tests */
    UtTest_Add(Test_CFE_SBN_Client_InitPipeTblFullyInitializesPipes, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_CFE_SBN_Client_InitPipeTblFullyInitializesPipes");
    
    /* CFE_SBN_Client_GetAvailPipeIdx Tests*/
    UtTest_Add(Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsErrorWhenAllPipesUsed, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsErrorWhenAllPipesUsed");
    UtTest_Add(Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsIndexForFirstOpenPipe, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsIndexForFirstOpenPipe");

}
