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
**  recv_msg Tests
**
*******************************************************************************/

void Test_recv_msg_returns_status_WhenFirst_CFE_SBN_Client_ReadBytes_DoesNotReturn_CFE_SUCCESS(void)
{
    /* Arrange */
    int32 arg_sockfd = 0;
    int32 result;
    int32 forced_readbyte_fail = Any_int32_Except(CFE_SUCCESS);
    unsigned char sbn_hdr_buffer[SBN_PACKED_HDR_SZ];

    wrap_CFE_SBN_Client_ReadBytes_msg_buffer = sbn_hdr_buffer;
    use_wrap_CFE_SBN_Client_ReadBytes = true;
    wrap_CFE_SBN_Client_ReadBytes_return_value = forced_readbyte_fail;
      
    /* Act */
    result = recv_msg(arg_sockfd);
    
    /* Assert */
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_was_called == true,
      "CFE_SBN_Client_ReadBytes was called");
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_call_count == 1,
      "CFE_SBN_Client_ReadBytes was called once");
    UtAssert_True(result == forced_readbyte_fail,
      "Fail result was received expectedly");
}

void Test_recv_msg_returns_CFE_EVS_ERROR_When_MsgType_IsUnrecognized(void)
{
    /* Arrange */
    int32 arg_sockfd = 0;
    int32 result;
    int32 forced_readbyte_return = CFE_SUCCESS;
    unsigned char sbn_hdr_buffer[SBN_PACKED_HDR_SZ];
    int idx = 0;

    /* Pack header oversize message to show never used */ 
    sbn_hdr_buffer[idx++] = 0xFF;
    sbn_hdr_buffer[idx++] = 0xFF;

    // Pack invalid message type
    sbn_hdr_buffer[idx++] = 0xFF;

    wrap_CFE_SBN_Client_ReadBytes_msg_buffer = sbn_hdr_buffer;
    use_wrap_CFE_SBN_Client_ReadBytes = true;
    wrap_CFE_SBN_Client_ReadBytes_return_value = forced_readbyte_return;
      
    /* Act */
    result = recv_msg(arg_sockfd);
    
    /* Assert */
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_was_called == true,
      "CFE_SBN_Client_ReadBytes was called");
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_call_count == 1,
      "CFE_SBN_Client_ReadBytes was called once");
    UtAssert_True(result == CFE_EVS_ERROR,
      "Fail MsgType result was received expectedly");
}


void Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_NO_MSG(void)
{
    /* Arrange */
    int32 arg_sockfd = 0;
    int32 result;
    int32 forced_readbyte_return = CFE_SUCCESS;
    unsigned char sbn_hdr_buffer[CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];
    int idx = 0;

    /* Pack full size message */ 
    sbn_hdr_buffer[idx++] = 0x80;
    sbn_hdr_buffer[idx++] = 0x00;

    // Pack valid message type
    sbn_hdr_buffer[idx++] = SBN_NO_MSG;

    wrap_CFE_SBN_Client_ReadBytes_msg_buffer = sbn_hdr_buffer;
    use_wrap_CFE_SBN_Client_ReadBytes = true;
    wrap_CFE_SBN_Client_ReadBytes_return_value = forced_readbyte_return;
      
    /* Act */
    result = recv_msg(arg_sockfd);
    
    /* Assert */
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_was_called == true,
      "CFE_SBN_Client_ReadBytes was called");
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_call_count == 2,
      "CFE_SBN_Client_ReadBytes was called twice");
    UtAssert_True(result == CFE_SUCCESS,
      "Success result was received expectedly");
}

void Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_SUB_MSG(void)
{
    /* Arrange */
    int32 arg_sockfd = 0;
    int32 result;
    int32 forced_readbyte_return = CFE_SUCCESS;
    unsigned char sbn_hdr_buffer[CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];
    int idx = 0;

    /* Pack full size message */ 
    sbn_hdr_buffer[idx++] = 0x80;
    sbn_hdr_buffer[idx++] = 0x00;

    // Pack valid message type
    sbn_hdr_buffer[idx++] = SBN_SUB_MSG;

    wrap_CFE_SBN_Client_ReadBytes_msg_buffer = sbn_hdr_buffer;
    use_wrap_CFE_SBN_Client_ReadBytes = true;
    wrap_CFE_SBN_Client_ReadBytes_return_value = forced_readbyte_return;
      
    /* Act */
    result = recv_msg(arg_sockfd);
    
    /* Assert */
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_was_called == true,
      "CFE_SBN_Client_ReadBytes was called");
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_call_count == 2,
      "CFE_SBN_Client_ReadBytes was called twice");
    UtAssert_True(result == CFE_SUCCESS,
      "Success result was received expectedly");
}

void Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_UNSUB_MSG(void)
{
    /* Arrange */
    int32 arg_sockfd = 0;
    int32 result;
    int32 forced_readbyte_return = CFE_SUCCESS;
    unsigned char sbn_hdr_buffer[CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];
    int idx = 0;

    /* Pack full size message */ 
    sbn_hdr_buffer[idx++] = 0x80;
    sbn_hdr_buffer[idx++] = 0x00;

    // Pack valid message type
    sbn_hdr_buffer[idx++] = SBN_UNSUB_MSG;

    wrap_CFE_SBN_Client_ReadBytes_msg_buffer = sbn_hdr_buffer;
    use_wrap_CFE_SBN_Client_ReadBytes = true;
    wrap_CFE_SBN_Client_ReadBytes_return_value = forced_readbyte_return;
      
    /* Act */
    result = recv_msg(arg_sockfd);
    
    /* Assert */
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_was_called == true,
      "CFE_SBN_Client_ReadBytes was called");
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_call_count == 2,
      "CFE_SBN_Client_ReadBytes was called twice");
    UtAssert_True(result == CFE_SUCCESS,
      "Success result was received expectedly");
}

void Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_PROTO_MSG(void)
{
    /* Arrange */
    int32 arg_sockfd = 0;
    int32 result;
    int32 forced_readbyte_return = CFE_SUCCESS;
    unsigned char sbn_hdr_buffer[CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];
    int idx = 0;

    /* Pack full size message */ 
    sbn_hdr_buffer[idx++] = 0x80;
    sbn_hdr_buffer[idx++] = 0x00;

    // Pack valid message type
    sbn_hdr_buffer[idx++] = SBN_PROTO_MSG;

    wrap_CFE_SBN_Client_ReadBytes_msg_buffer = sbn_hdr_buffer;
    use_wrap_CFE_SBN_Client_ReadBytes = true;
    wrap_CFE_SBN_Client_ReadBytes_return_value = forced_readbyte_return;
      
    /* Act */
    result = recv_msg(arg_sockfd);
    
    /* Assert */
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_was_called == true,
      "CFE_SBN_Client_ReadBytes was called");
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_call_count == 2,
      "CFE_SBN_Client_ReadBytes was called twice");
    UtAssert_True(result == CFE_SUCCESS,
      "Success result was received expectedly");
}

void Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_HEARTBEAT_MSG(void)
{
    /* Arrange */
    int32 arg_sockfd = 0;
    int32 result;
    int32 forced_readbyte_return = CFE_SUCCESS;
    unsigned char sbn_hdr_buffer[CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];
    int idx = 0;

    /* Pack full size message */ 
    sbn_hdr_buffer[idx++] = 0x80;
    sbn_hdr_buffer[idx++] = 0x00;

    // Pack valid message type
    sbn_hdr_buffer[idx++] = SBN_HEARTBEAT_MSG;

    wrap_CFE_SBN_Client_ReadBytes_msg_buffer = sbn_hdr_buffer;
    use_wrap_CFE_SBN_Client_ReadBytes = true;
    wrap_CFE_SBN_Client_ReadBytes_return_value = forced_readbyte_return;
      
    /* Act */
    result = recv_msg(arg_sockfd);
    
    /* Assert */
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_was_called == true,
      "CFE_SBN_Client_ReadBytes was called");
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_call_count == 2,
      "CFE_SBN_Client_ReadBytes was called twice");
    UtAssert_True(result == CFE_SUCCESS,
      "Success result was received expectedly");
}

void Test_recv_msg_returns_Calls_ingest_app_message_AndReturns_CFE_SUCCESS_When_MsgType_Is_SBN_APP_MSG(void)
{
    /* Arrange */
    int32 arg_sockfd = 0;
    int32 result;
    int32 forced_readbyte_return = CFE_SUCCESS;
    unsigned char sbn_hdr_buffer[CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];
    int idx = 0;

    /* Pack full size message */ 
    sbn_hdr_buffer[idx++] = 0x80;
    sbn_hdr_buffer[idx++] = 0x00;

    // Pack valid message type
    sbn_hdr_buffer[idx++] = SBN_APP_MSG;

    wrap_CFE_SBN_Client_ReadBytes_msg_buffer = sbn_hdr_buffer;
    use_wrap_CFE_SBN_Client_ReadBytes = true;
    wrap_CFE_SBN_Client_ReadBytes_return_value = forced_readbyte_return;
    use_wrap_ingest_app_message = true;
      
    /* Act */
    result = recv_msg(arg_sockfd);
    
    /* Assert */
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_was_called == true,
      "CFE_SBN_Client_ReadBytes was called");
    UtAssert_True(wrap_CFE_SBN_Client_ReadBytes_call_count == 1,
      "CFE_SBN_Client_ReadBytes was called once");
    UtAssert_True(wrap_ingest_app_message_was_called == true,
      "ingest_app_messagewas called");
    UtAssert_True(wrap_ingest_app_message_call_count == 1,
      "ingest_app_message was called once");
    UtAssert_True(result == CFE_SUCCESS,
      "Success result was received expectedly");
}
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

void add_recv_msg(void)
{
    UtTest_Add(Test_recv_msg_returns_status_WhenFirst_CFE_SBN_Client_ReadBytes_DoesNotReturn_CFE_SUCCESS, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_recv_msg_returns_status_WhenFirst_CFE_SBN_Client_ReadBytes_DoesNotReturn_CFE_SUCCESS");
    UtTest_Add(Test_recv_msg_returns_CFE_EVS_ERROR_When_MsgType_IsUnrecognized, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_recv_msg_returns_CFE_EVS_ERROR_When_MsgType_IsUnrecognized");
    UtTest_Add(Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_NO_MSG, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_NO_MSG");
    UtTest_Add(Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_SUB_MSG, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_SUB_MSG");
    UtTest_Add(Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_UNSUB_MSG, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_UNSUB_MSG");
    UtTest_Add(Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_PROTO_MSG, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_PROTO_MSG");
    UtTest_Add(Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_HEARTBEAT_MSG, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_recv_msg_returns_status_From_CFE_SBN_Client_ReadBytes_When_MsgType_Is_SBN_HEARTBEAT_MSG");
    UtTest_Add(Test_recv_msg_returns_Calls_ingest_app_message_AndReturns_CFE_SUCCESS_When_MsgType_Is_SBN_APP_MSG, 
      SBN_Client_Tests_Setup, SBN_Client_Tests_Teardown, 
      "Test_recv_msg_returns_Calls_ingest_app_message_AndReturns_CFE_SUCCESS_When_MsgType_Is_SBN_APP_MSG");
} /* end add_recv_msg */

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

    add_recv_msg();
} /* end UtTest_Setup */

/* end Required UtTest_Setup function for ut-assert framework */
