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

#include "sbn_client_tests_includes.h"

/*******************************************************************************
**
**  SBN_Client_Tests Setup and Teardown
**
*******************************************************************************/

void SBN_Client_Tests_Setup(void)
{
    SBN_Client_Setup();
} /* end SBN_Client_Tests_Setup */

void SBN_Client_Tests_Teardown(void)
{
    SBN_Client_Teardown();
} /* end SBN_Client_Tests_Setup */

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
          "PipeTbl[%d].InUse should equal %d and was %d", i, 
          CFE_SBN_CLIENT_NOT_IN_USE, test_pipe.InUse);
        UtAssert_True(test_pipe.SysQueueId == CFE_SBN_CLIENT_UNUSED_QUEUE, 
          "PipeTbl[%d].SysQueueId should equal %d and was %d", i, 
          CFE_SBN_CLIENT_UNUSED_QUEUE, test_pipe.SysQueueId);
        UtAssert_True(test_pipe.PipeId == CFE_SBN_CLIENT_INVALID_PIPE, 
          "PipeTbl[%d].PipeId should equal %d and was %d", i, 
          CFE_SBN_CLIENT_INVALID_PIPE, test_pipe.PipeId);
        UtAssert_True(test_pipe.NumberOfMessages == 1, 
          "PipeTbl[%d].NumberOfMessages should equal %d and was %d", i, 1, 
          test_pipe.NumberOfMessages);
        UtAssert_True(test_pipe.ReadMessage == 
          (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1), 
          "PipeTbl[%d].ReadMessage should equal %d and was %d", i, 
          CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1, 
          test_pipe.ReadMessage);
        UtAssert_True(strcmp(test_pipe.PipeName, "") == 0, 
          "PipeTbl[%d].PipeId should equal '' and was '%s'", i, 
          test_pipe.PipeName);  
    
        for(j = 0; j < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; j++)
        {
            UtAssert_True(test_pipe.SubscribedMsgIds[j] == 
              CFE_SBN_CLIENT_INVALID_MSG_ID, 
              "PipeTbl[%d].SubscribedMsgIds[%d] should be %d and was %d", i, j, 
              CFE_SBN_CLIENT_INVALID_MSG_ID, test_pipe.SubscribedMsgIds[j]);
        }
        
    }

} /* end Test_CFE_SBN_Client_InitPipeTblFullyInitializesPipes */

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
} /* end Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsErrorWhenAllPipesUsed */

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
    UtAssert_True(result == available_index, 
      "CFE_SBN_Client_GetAvailPipeIdx should have returned %d and returned %d", 
      available_index, result);
} /* end Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsIndexForFirstOpenPipe */

/* end CFE_SBN_Client_GetAvailPipeIdx Tests */

/*******************************************************************************
**
**  add test group functions
**
*******************************************************************************/

void add_CFE_SBN_Client_InitPipeTbl_tests(void)
{
    UtTest_Add(Test_CFE_SBN_Client_InitPipeTblFullyInitializesPipes, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_CFE_SBN_Client_InitPipeTblFullyInitializesPipes");
} /* end add_CFE_SBN_Client_InitPipeTbl_tests */

void add_CFE_SBN_Client_GetAvailPipeIdx(void)
{
    UtTest_Add(Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsErrorWhenAllPipesUsed, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsErrorWhenAllPipesUsed");
    UtTest_Add(Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsIndexForFirstOpenPipe, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsIndexForFirstOpenPipe");
} /* end add_CFE_SBN_Client_GetAvailPipeIdx */

/* end add test group functions */

/*******************************************************************************
**
**  Required UtTest_Setup function for ut-assert framework
**
*******************************************************************************/

void UtTest_Setup(void)
{
    add_CFE_SBN_Client_InitPipeTbl_tests();
    
    add_CFE_SBN_Client_GetAvailPipeIdx();
} /* end UtTest_Setup */

/* end Required UtTest_Setup function for ut-assert framework */
