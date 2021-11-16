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
**  SBN_Client_Ingest_Tests Setup and Teardown
**
*******************************************************************************/

void SBN_Client_Ingest_Setup(void)
{
    SBN_Client_Setup();
}

void SBN_Client_Ingest_Teardown(void)
{
    SBN_Client_Teardown();
}


/*******************************************************************************
**
**  ingest_app_message Tests
**
*******************************************************************************/

void Test_ingest_app_message_ReadBytesFails(void)
{
    /* Arrange */ 
    char err_msg[60];
    int sockfd = Any_int();
    int msgSize = rand() % CFE_SB_MAX_SB_MSG_SIZE;
    
    use_wrap_CFE_SBN_CLIENT_ReadBytes = TRUE;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = Any_int32_Except(CFE_SUCCESS);
    
    snprintf(err_msg, 60,
      "CFE_SBN_CLIENT_ReadBytes returned a bad status = 0x%08X\n", 
      wrap_CFE_SBN_CLIENT_ReadBytes_return_value);
    
    log_message_expected_string = err_msg;
    
    /* Act */
    ingest_app_message(sockfd, msgSize);
    
    /* Assert */
    UtAssert_True(wrap_pthread_mutex_lock_was_called == FALSE,
      "pthread_mutex_lock should not have been called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called == FALSE,
      "pthread_mutex_unlock should not have been called");
    UtAssert_True(wrap_pthread_cond_signal_was_called == FALSE,
      "pthread_cond_signal should not have been called");
}

void Test_ingest_app_message_FailsWhenNoPipesInUse(void)
{
    /* Arrange */ 
    char err_msg[60] = "SBN_CLIENT: No pipes are in use";
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int sockfd = Any_int();
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    wrap_pthread_cond_signal_should_be_called = FALSE;
    use_wrap_CFE_SBN_Client_GetMsgId = TRUE;
    wrap_CFE_SBN_Client_GetMsgId_return_value = msg[0] << 8 | msg[1];
    use_wrap_CFE_SBN_CLIENT_ReadBytes = TRUE;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = CFE_SUCCESS;
    wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = msg;

    log_message_expected_string = err_msg;

    /* Act */
    ingest_app_message(sockfd, msgSize);

    /* Assert */
    UtAssert_True(wrap_pthread_mutex_lock_was_called == TRUE,
      "pthread_mutex_lock should not have been called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called == TRUE,
      "pthread_mutex_unlock should not have been called");
    UtAssert_True(wrap_pthread_cond_signal_was_called == FALSE,
      "pthread_cond_signal should not have been called");
}

void Test_ingest_app_message_FailsOverflowWhenNumberOfMessagesIsFull(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    int num_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    int msg_slot = read_msg + num_msg;
    int sockfd = Any_int();
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    wrap_pthread_cond_signal_should_be_called = FALSE;
    use_wrap_CFE_SBN_Client_GetMsgId = TRUE;
    wrap_CFE_SBN_Client_GetMsgId_return_value = msg[0] << 8 | msg[1];
    use_wrap_CFE_SBN_CLIENT_ReadBytes = TRUE;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = CFE_SUCCESS;
    wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = msg;
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    
    /* Act */ 
    ingest_app_message(sockfd, msgSize);
    
    /* Assert */
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg, 
      "PipeTbl[%d].NumberOfMessages %d should not increase and was %d", 
      pipe_assigned, num_msg, PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, 
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage);
    UtAssert_True(wrap_pthread_mutex_lock_was_called == TRUE,
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called == TRUE,
      "pthread_mutex_unlock was called");
    UtAssert_True(wrap_pthread_cond_signal_was_called == FALSE,
      "pthread_cond_signal should not have been called");
}

void Test_ingest_app_message_FailsWhenNoPipeLookingForMessageId(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;  
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    int num_msg = 0;
    int msg_slot= read_msg + num_msg;
    int sockfd = Any_int();
    char err_msg[60] = "SBN_CLIENT: ERROR no subscription for this msgid";
    
    use_wrap_CFE_SBN_CLIENT_ReadBytes = TRUE;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = Any_int32_Except(CFE_SUCCESS);
    
    log_message_expected_string = err_msg;
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    wrap_pthread_cond_signal_should_be_called = FALSE;
    use_wrap_CFE_SBN_Client_GetMsgId = TRUE;
    wrap_CFE_SBN_Client_GetMsgId_return_value = msg[0] << 8 | msg[1];
    use_wrap_CFE_SBN_CLIENT_ReadBytes = TRUE;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = CFE_SUCCESS;
    wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = msg;
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = 0x0000;
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    
    /* Act */ 
    ingest_app_message(sockfd, msgSize);
    
    /* Assert */
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == 0, 
      "PipeTbl[%d].NumberOfMessages should = %d and was %d ", pipe_assigned, 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH, 
      PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, 
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage);
    UtAssert_True(wrap_pthread_mutex_lock_was_called == TRUE,
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called == TRUE,
      "pthread_mutex_unlock was called");
    UtAssert_True(wrap_pthread_cond_signal_was_called == FALSE,
      "pthread_cond_signal should not have been called");
}

void Test_ingest_app_message_SuccessAllSlotsAvailable(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    int num_msg = 0;
    int msg_slot = read_msg + num_msg;
    int sockfd = Any_int();
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    wrap_pthread_cond_signal_should_be_called = TRUE;
    use_wrap_CFE_SBN_Client_GetMsgId = TRUE;
    wrap_CFE_SBN_Client_GetMsgId_return_value = msg[0] << 8 | msg[1];
    use_wrap_CFE_SBN_CLIENT_ReadBytes = TRUE;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = CFE_SUCCESS;
    wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = msg;
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    
    /* Act */ 
    ingest_app_message(sockfd, msgSize);
    
    /* Assert */
    int i;
    
    for(i = 0; i < msgSize; i++)
    {
        UtAssert_True(PipeTbl[pipe_assigned].Messages[msg_slot][i] == msg[i], 
          "PipeTbl[%d].Messages[%d][%d] should = %d and was %d", 
          pipe_assigned, msg_slot, i, msg[i], 
          PipeTbl[pipe_assigned].Messages[msg_slot][i]);
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg + 1, 
      "PipeTbl[%d].NumberOfMessages should increase by 1 to %d and was %d", 
      pipe_assigned, num_msg + 1, PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, 
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage);
    UtAssert_True(wrap_pthread_mutex_lock_was_called == TRUE,
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called == TRUE,
      "pthread_mutex_unlock was called");
    UtAssert_True(wrap_pthread_cond_signal_was_called == TRUE,
      "pthread_cond_signal was called");
}

void Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    /* from 1 to CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH */
    int num_msg = (rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1) + 1; 
    int msg_slot = read_msg + num_msg;
    int sockfd = Any_int();
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    wrap_pthread_cond_signal_should_be_called = TRUE;
    use_wrap_CFE_SBN_Client_GetMsgId = TRUE;
    wrap_CFE_SBN_Client_GetMsgId_return_value = msg[0] << 8 | msg[1];
    use_wrap_CFE_SBN_CLIENT_ReadBytes = TRUE;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = CFE_SUCCESS;
    wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = msg;
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    
    /* Act */ 
    ingest_app_message(sockfd, msgSize);
    
    /* Assert */
    int i;
    
    for(i = 0; i < msgSize; i++)
    {
        UtAssert_True(PipeTbl[pipe_assigned].Messages[msg_slot][i] == msg[i], 
          "PipeTbl[%d].Messages[%d][%d] should = %d and was %d", 
          pipe_assigned, msg_slot, i, msg[i], 
          PipeTbl[pipe_assigned].Messages[msg_slot][i]);
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg + 1, 
      "PipeTbl[%d].NumberOfMessages should increase by 1 to %d and was %d", 
      pipe_assigned, num_msg + 1, PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, 
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage);
    UtAssert_True(wrap_pthread_mutex_lock_was_called == TRUE,
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called == TRUE,
      "pthread_mutex_unlock was called");
    UtAssert_True(wrap_pthread_cond_signal_was_called == TRUE,
      "pthread_cond_signal was called");
}

void Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;  
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    int num_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1; /* 1 slot left */
    int msg_slot= read_msg + num_msg;
    int sockfd = Any_int();
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    wrap_pthread_cond_signal_should_be_called = TRUE;
    use_wrap_CFE_SBN_Client_GetMsgId = TRUE;
    wrap_CFE_SBN_Client_GetMsgId_return_value = msg[0] << 8 | msg[1];
    use_wrap_CFE_SBN_CLIENT_ReadBytes = TRUE;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = CFE_SUCCESS;
    wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = msg;
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    
    /* Act */ 
    ingest_app_message(sockfd, msgSize);
    
    /* Assert */
    int i;
    
    for(i = 0; i < msgSize; i++)
    {
        UtAssert_True(PipeTbl[pipe_assigned].Messages[msg_slot][i] == msg[i], 
          "PipeTbl[%d].Messages[%d][%d] should = %d and was %d ", 
          pipe_assigned, msg_slot, i, msg[i], 
          PipeTbl[pipe_assigned].Messages[msg_slot][i]);
    }  
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH, 
      "PipeTbl[%d].NumberOfMessages should = %d and was %d ", pipe_assigned, 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH, 
      PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, 
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage);
    UtAssert_True(wrap_pthread_mutex_lock_was_called == TRUE,
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called == TRUE,
      "pthread_mutex_unlock was called");
    UtAssert_True(wrap_pthread_cond_signal_was_called == TRUE,
      "pthread_cond_signal was called");
}

//void Test_ingest_app_message_SuccessCausesPipeNumberOfMessagesToIncreaseBy1
//void Test_ingest_app_message_FailsWhenNoPipesInUse
/* end ingest_app_message Tests */



void UtTest_Setup(void)
{
    /* ingest_app_message Tests */
    UtTest_Add(
      Test_ingest_app_message_ReadBytesFails, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_ReadBytesFails");
    UtTest_Add(
      Test_ingest_app_message_FailsWhenNoPipesInUse, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_FailsWhenNoPipesInUse");
    UtTest_Add(
      Test_ingest_app_message_FailsOverflowWhenNumberOfMessagesIsFull, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_FailsOverflowWhenNumberOfMessagesIsFull");
    UtTest_Add(
      Test_ingest_app_message_FailsWhenNoPipeLookingForMessageId, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_FailsWhenNoPipeLookingForMessageId");
    UtTest_Add(
      Test_ingest_app_message_SuccessAllSlotsAvailable, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_SuccessAllSlotsAvailable");
    UtTest_Add(
      Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable");
    UtTest_Add(
      Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft");
}