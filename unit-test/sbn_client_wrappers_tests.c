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

CFE_SB_PipeId_t pipePtr;
uint16 pipe_depth = 5;
const char *pipeName = "TestPipe";

/*******************************************************************************
**
**  SBN_Client_Wrappers_Tests Setup and Teardown
**
*******************************************************************************/

void SBN_Client_Wrappers_Tests_Setup(void)
{
    SBN_Client_Setup();
} /* end SBN_Client_Wrappers_Tests_Setup */

void SBN_Client_Wrappers_Tests_Teardown(void)
{
    SBN_Client_Teardown();  
    
    pipePtr = 0;
    pipe_depth = 5;
} /* end SBN_Client_Wrappers_Tests_Teardown */

/*******************************************************************************
**
**  __wrap_CFE_SB_CreatePipe Tests
**
*******************************************************************************/

void Test__wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS(void)
{
  /* Arrange - none required */
  /* Act */ 
  int32 result = CFE_SB_CreatePipe(&pipePtr, pipe_depth, pipeName);
  
  /* Assert */
  UtAssert_True(result == CFE_SUCCESS, 
    "Pipe creation should have succeeded with (= %d), the result was (= %d)", 
    CFE_SUCCESS, result);
} /* end Test__wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS */

void Test__wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly(void)
{
  /* Arrange - none required */
  /* Act */ 
  CFE_SB_CreatePipe(&pipePtr, pipe_depth, pipeName);
  
  /* Assert */
  UtAssert_True(pipePtr == 0, 
    "PipePtr should point to pipe 0 (initial pipe) and points to pipe %d.", 
    pipePtr);
  UtAssert_True(PipeTbl[0].InUse == CFE_SBN_CLIENT_IN_USE, 
    "PipeTbl[0].InUse should be %d and was %d", CFE_SBN_CLIENT_IN_USE, 
    PipeTbl[0].InUse);
  UtAssert_True(PipeTbl[0].PipeId == 0, 
    "PipeTbl[0].PipeID should be %d and was %d", 0, PipeTbl[0].PipeId);
  UtAssert_True(PipeTbl[0].SendErrors == 0, 
    "PipeTbl[0].SendErrors should be %d and was %d", 0, PipeTbl[0].SendErrors);
  UtAssert_True(strcmp(&PipeTbl[0].PipeName[0], pipeName) == 0, 
  "PipeTbl[0].PipeName should be %s and was %s", pipeName, 
    PipeTbl[0].PipeName);
  UtAssert_True(PipeTbl[0].NumberOfMessages == 0, 
    "PipeTbl[0].NumberOfMessages should be %d and was %d", 0, 
    PipeTbl[0].NumberOfMessages);
  UtAssert_True(PipeTbl[0].ReadMessage == 0, 
    "PipeTbl[0].ReadMessage should be %d and was %d", 0, 
    PipeTbl[0].ReadMessage);
} /* end Test__wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly */

void Test__wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull(void)
{
  /* Arrange */
  int i;
  //uint32 initial_event_q_depth = Ut_CFE_EVS_GetEventQueueDepth();
  
  for (i = 0; i < CFE_PLATFORM_SBN_CLIENT_MAX_PIPES; i++)
  {
    PipeTbl[i].InUse = CFE_SBN_CLIENT_IN_USE;
  }
  
  /* Act */ 
  int32 result = CFE_SB_CreatePipe(&pipePtr, pipe_depth, pipeName);
  //uint32 current_event_q_depth = Ut_CFE_EVS_GetEventQueueDepth();
  
  /* Assert */
  UtAssert_True(result == CFE_SBN_CLIENT_MAX_PIPES_MET, 
    "Call to CFE_SB_CreatePipe result should be %d and was %d", 
    CFE_SBN_CLIENT_MAX_PIPES_MET, result);
  // TODO:set stubs to intercept wraps on CFE calls
  // UtAssert_True(current_event_q_depth == initial_event_q_depth + 1, 
  // "Event queue count should be %d, but was %d", initial_event_q_depth + 1, 
  //  current_event_q_depth);
  // UtAssert_EventSent(CFE_SBN_CLIENT_MAX_PIPES_MET, CFE_EVS_ERROR, expected_
  //  error_msg, 
  // "Error event as expected was not sent. Expected: Error = %d, ErrorType=%d, 
  // Error Message = %s", CFE_SBN_CLIENT_MAX_PIPES_MET, CFE_EVS_ERROR, 
  // expected_error_msg);
} /* end Test__wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull */

/* end __wrap_CFE_SB_CreatePipe Tests */

/*******************************************************************************
**
**  __wrap_CFE_SB_DeletePipe Tests
**
*******************************************************************************/

void Test__wrap_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse(void)
{
  /* Arrange */
  int pipeIdToDelete = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
  PipeTbl[pipeIdToDelete].PipeId = pipeIdToDelete;  
  PipeTbl[pipeIdToDelete].InUse = CFE_SBN_CLIENT_IN_USE;
  
  /* Act */ 
  int32 result = CFE_SB_DeletePipe(pipeIdToDelete);
  
  /* Assert */
  UtAssert_True(result == CFE_SUCCESS, 
    "Call to CFE_SB_DeletePipe to delete pipe#%d should be %d and was %d", 
    pipeIdToDelete, CFE_SUCCESS, result);
} /* end Test__wrap_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse */

/* end __wrap_CFE_SB_DeletePipe Tests */

/*******************************************************************************
**
**  __wrap_CFE_SB_Subscribe Tests
**
*******************************************************************************/

void Test__wrap_CFE_SB_SubscribePipeIsValidMsgIdUnsubscribedNotAtMaxMsgIds(void)
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
      
    for (i = num_msgIds_subscribed; i < (
      CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE - num_msgIds_subscribed); i++)
    {
        PipeTbl[pipe_id].SubscribedMsgIds[i] = CFE_SBN_CLIENT_INVALID_MSG_ID;
    }
    
    /* Act */ 
    int32 result = CFE_SB_Subscribe(msg_id, pipe_id);
    
    /* Assert */
    UtAssert_True(result == CFE_SUCCESS, 
      "Call to CFE_SB_Subscribe should return %d and was %d", CFE_SUCCESS, 
      result);
    UtAssert_True(PipeTbl[pipe_id].SubscribedMsgIds[num_msgIds_subscribed] == 
      msg_id, "PipeTble[%d].SubscribedMsgIds[%d] should be %d and was %d", 
      pipe_id, num_msgIds_subscribed, msg_id, 
      PipeTbl[pipe_id].SubscribedMsgIds[num_msgIds_subscribed]);    
}

void Test__wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid(void)
{
    /* Arrange */
    int pipe_id = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    int msg_id = 0x1800;
    PipeTbl[pipe_id].InUse = CFE_SBN_CLIENT_NOT_IN_USE;
    PipeTbl[pipe_id].PipeId = CFE_SBN_CLIENT_INVALID_PIPE;
    
    /* Act */ 
    int32 result = CFE_SB_Subscribe(msg_id, pipe_id);
        
    /* Assert */
    UtAssert_True(result == CFE_SBN_CLIENT_BAD_ARGUMENT, 
      "Call to CFE_SB_Subscribe with pipeId %d should be error %d and was %d", 
      pipe_id, CFE_SBN_CLIENT_BAD_ARGUMENT, result);
} /* end Test__wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid */

void Test__wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded(void)
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
        // printf("PipeTbl[%d].SubscribedMsgIds[%d] = %d\n", pipe_id, i, 
        //   PipeTbl[pipe_id].SubscribedMsgIds[i]);
    }
    
    /* Act */ 
    int32 result = CFE_SB_Subscribe(msg_id, pipe_id);
        
    /* Assert */
    UtAssert_True(result == CFE_SBN_CLIENT_BAD_ARGUMENT, 
      "Call to CFE_SB_Subscribe with pipeId %d should be error %d and was %d", 
      pipe_id, CFE_SBN_CLIENT_BAD_ARGUMENT, result);
} /* end Test__wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded */

/* end __wrap_CFE_SB_Subscribe Tests */

/*******************************************************************************
**
**  __wrap_CFE_SB_RcvMsg Tests
**
*******************************************************************************/

void Test__wrap_CFE_SB_RcvMsg_FailsBufferPointerIsNull(void)
{
    /* Arrange */
    CFE_SB_MsgPtr_t *buffer_ptr = NULL;
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();
    int32 timeout = Any_Positive_int32();
    int32 result;
    
    log_message_expected_string = "SBN_CLIENT: BUFFER POINTER IS NULL!";
    
    /* Act */
    result = CFE_SB_RcvMsg(buffer_ptr, pipe_assigned, timeout);
    
    /* Assert */
    UtAssert_True(result == CFE_SB_BAD_ARGUMENT,
      "__wrap_CFE_SB_RcvMsg returned CFE_SB_BAD_ARGUMENT");
    UtAssert_True(log_message_was_called, "log_message was called");
} /* end Test__wrap_CFE_SB_RcvMsg_FailsBufferPointerIsNull */

void Test__wrap_CFE_SB_RcvMsg_FailsTimeoutLessThanNegativeOne(void)
{
    /* Arrange */
    CFE_SB_MsgPtr_t buffer;
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();
    int32 timeout = Any_Negative_int32_Except(CFE_SB_PEND_FOREVER);
    int32 result;
    
    log_message_expected_string = "SBN_CLIENT: TIMEOUT IS LESS THAN -1!";
    
    /* Act */
    result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);
    
    /* Assert */
    UtAssert_True(result == CFE_SB_BAD_ARGUMENT,
      "__wrap_CFE_SB_RcvMsg returned CFE_SB_BAD_ARGUMENT");
    UtAssert_True(log_message_was_called, "log_message was called");
} /* end Test__wrap_CFE_SB_RcvMsg_FailsTimeoutLessThanNegativeOne */

void Test__wrap_CFE_SB_RcvMsg_FailsInvalidPipeIdx(void)
{
    /* Arrange */
    CFE_SB_MsgPtr_t buffer;
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();
    int32 timeout = Any_Positive_int32();
    int32 result;
    /* No control on clock_gettime; its value does not affect this test */
    
    use_wrap_CFE_SBN_Client_GetPipeIdx = TRUE;
    wrap_CFE_SBN_Client_GetPipeIdx_return_value = CFE_SBN_CLIENT_INVALID_PIPE;
    
    log_message_expected_string = "SBN_CLIENT: ERROR INVALID PIPE ERROR!";
        
    /* Act */
    result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);
    
    /* Assert */
    UtAssert_True(result == CFE_SB_BAD_ARGUMENT, 
      "__wrap_CFE_SB_RcvMsg returned CFE_SB_BAD_ARGUMENT");
    UtAssert_True(log_message_was_called, "log_message was called");
} /* end Test__wrap_CFE_SB_RcvMsg_FailsInvalidPipeIdx */

void Test__wrap_CFE_SB_RcvMsg_FailPthreadMutexLockFailure(void)
{
    /* Arrange */
    CFE_SB_MsgPtr_t buffer;
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();
    int32 timeout = CFE_SB_POLL;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 result;
    
    pipe->NumberOfMessages = 1;
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_lock_return_value = Any_int_Except(0);
    use_wrap_CFE_SBN_Client_GetPipeIdx = TRUE;
    wrap_CFE_SBN_Client_GetPipeIdx_return_value = pipe_assigned;
    
    /* Act */
    result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);
    
    /* Assert */
    UtAssert_True(result == CFE_SB_PIPE_RD_ERR, 
      "__wrap_CFE_SB_RcvMsg returned CFE_SB_PIPE_RD_ERR, mutex lock fail");
    UtAssert_True(buffer == NULL, "__wrap_CFE_SB_RcvMsg set *BufPtr to NULL");
    UtAssert_True(wrap_pthread_mutex_lock_was_called, 
      "pthread_mutex_lock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_FailPthreadMutexLockFailure */

void Test__wrap_CFE_SB_RcvMsg_PollRequestReturnsWhenNoMessage(void)
{
    /* Arrange */
    CFE_SB_MsgPtr_t buffer;
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();
    int32 timeout = CFE_SB_POLL;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 result;
    
    pipe->NumberOfMessages = 1;
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    use_wrap_CFE_SBN_Client_GetPipeIdx = TRUE;
    wrap_CFE_SBN_Client_GetPipeIdx_return_value = pipe_assigned;
    
    /* Act */
    result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);
    
    /* Assert */
    UtAssert_True(result == CFE_SB_NO_MESSAGE, 
      "__wrap_CFE_SB_RcvMsg returned CFE_SB_NO_MESSAGE, pipe empty, poll rqst");
    UtAssert_True(buffer == NULL, "__wrap_CFE_SB_RcvMsg set *BufPtr to NULL");
    UtAssert_True(wrap_pthread_mutex_lock_was_called, 
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called, 
      "pthread_mutex_unlock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_PollRequestReturnsWhenNoMessage */

void Test__wrap_CFE_SB_RcvMsg_FailsPendWhenWaitReturnsError(void)
{
    /* Arrange */
    CFE_SB_MsgPtr_t buffer;
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();
    int32 timeout = CFE_SB_PEND_FOREVER;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 result;

    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    wrap_pthread_cond_wait_should_be_called = TRUE;
    use_wrap_pthread_cond_wait = TRUE;
    wrap_pthread_cond_timedwait_return_value = Any_int_Except(0);
    use_wrap_CFE_SBN_Client_GetPipeIdx = TRUE;
    wrap_CFE_SBN_Client_GetPipeIdx_return_value = pipe_assigned;

    pipe->NumberOfMessages = 1;

    /* Act */
    result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);

    /* Assert */
    UtAssert_True(result == CFE_SB_PIPE_RD_ERR, 
      "__wrap_CFE_SB_RcvMsg returned CFE_SB_PIPE_RD_ERR, pipe empty, pend fail");
    UtAssert_True(buffer == NULL, "__wrap_CFE_SB_RcvMsg set *BufPtr to NULL");
    UtAssert_True(wrap_pthread_mutex_lock_was_called, 
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_cond_wait_was_called, 
      "pthread_cond_wait was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called, 
      "pthread_mutex_unlock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_FailsPendWhenWaitReturnsError */

void Test__wrap_CFE_SB_RcvMsg_TimeoutReturnsNoMessageAfterTimeoutExpires(void)
{
    /* Arrange */
    CFE_SB_MsgPtr_t buffer;
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();
    int32 timeout = Any_Positive_int32();
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 result;

    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    wrap_pthread_cond_timedwait_should_be_called = TRUE;
    use_wrap_pthread_cond_timedwait = TRUE;
    wrap_pthread_cond_timedwait_return_value = ETIMEDOUT;
    use_wrap_CFE_SBN_Client_GetPipeIdx = TRUE;
    wrap_CFE_SBN_Client_GetPipeIdx_return_value = pipe_assigned;

    pipe->NumberOfMessages = 1;

    /* Act */
    result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);

    /* Assert */
    UtAssert_True(result == CFE_SB_TIME_OUT, 
      "__wrap_CFE_SB_RcvMsg returned CFE_SB_TIME_OUT, pipe empty, timed out");
    UtAssert_True(buffer == NULL, "__wrap_CFE_SB_RcvMsg set *BufPtr to NULL");
    UtAssert_True(wrap_pthread_mutex_lock_was_called, 
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_cond_timedwait_was_called, 
      "pthread_cond_timedwait was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called, 
      "pthread_mutex_unlock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_TimeoutReturnsNoMessageAfterTimeoutExpires */

void Test__wrap_CFE_SB_RcvMsg_SuccessPollRequestHasMessageInPipe(void)
{
    /* Arrange */
    size_t msgSize = Any_Message_Size();
    unsigned char *msg = Any_Pipe_Message(msgSize);
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();    
    CFE_SB_MsgId_t msg_id_slot = Any_Message_Id_Slot();
    uint32 previous_read_msg = Any_Pipe_Message_Location();
    uint32 current_read_msg = (previous_read_msg + 1) % 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wraps to 0 if necessary */
    uint32 number_of_messages = rand() % 
      (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1) + 2; /* 2 to MAX */
      
    CFE_SB_MsgPtr_t buffer;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 timeout = CFE_SB_POLL;
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    pipe->NumberOfMessages = number_of_messages;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);

    /* Assert */
    UtAssert_True(result == CFE_SUCCESS, 
      "__wrap_CFE_SB_RcvMsg result should be %d and was %d", CFE_SUCCESS, 
      result);
    UtAssert_MemCmp(buffer, msg, msgSize, "Message in buffer is as expected");    
    UtAssert_True(
      PipeTbl[pipe_assigned].NumberOfMessages == number_of_messages - 1, 
      "PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d and is %d", 
      pipe_assigned, number_of_messages - 1, 
      PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should have progressed to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);
    UtAssert_True(wrap_pthread_mutex_lock_was_called, 
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called, 
        "pthread_mutex_unlock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_SuccessPollRequestHasMessageInPipe */

void Test__wrap_CFE_SB_RcvMsg_SuccessTimeoutPendMessageAlreadyInPipe(void)
{
    /* Arrange */
    size_t msgSize = Any_Message_Size();
    unsigned char *msg = Any_Pipe_Message(msgSize);
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();    
    CFE_SB_MsgId_t msg_id_slot = Any_Message_Id_Slot();
    uint32 previous_read_msg = Any_Pipe_Message_Location();
    uint32 current_read_msg = (previous_read_msg + 1) % 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wraps to 0 if necessary */
    uint32 number_of_messages = rand() % 
      (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1) + 2; /* 2 to MAX */
      
    CFE_SB_MsgPtr_t buffer;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 timeout = CFE_SB_PEND_FOREVER;
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    pipe->NumberOfMessages = number_of_messages;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);

    /* Assert */
    UtAssert_True(result == CFE_SUCCESS, 
      "__wrap_CFE_SB_RcvMsg result should be %d and was %d", CFE_SUCCESS, 
      result);
    UtAssert_MemCmp(buffer, msg, msgSize, "Message in buffer is as expected"); 
    UtAssert_True(
      PipeTbl[pipe_assigned].NumberOfMessages == number_of_messages - 1, 
      "PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d and is %d", 
      pipe_assigned, number_of_messages - 1, 
      PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(
      PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should have progressed to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);
    UtAssert_True(wrap_pthread_mutex_lock_was_called, 
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called, 
      "pthread_mutex_unlock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_SuccessTimeoutPendMessageAlreadyInPipe */

void Test__wrap_CFE_SB_RcvMsg_SuccessTimeoutValueMessageAlreadyInPipe(void)
{
    /* Arrange */
    size_t msgSize = Any_Message_Size();
    unsigned char *msg = Any_Pipe_Message(msgSize);
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();    
    CFE_SB_MsgId_t msg_id_slot = Any_Message_Id_Slot();
    uint32 previous_read_msg = Any_Pipe_Message_Location();
    uint32 current_read_msg = (previous_read_msg + 1) % 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wraps to 0 if necessary */
    uint32 number_of_messages = rand() % 
      (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1) + 2; /* 2 to MAX */
      
    CFE_SB_MsgPtr_t buffer;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 timeout = Any_Positive_int32();
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    pipe->NumberOfMessages = number_of_messages;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);

    /* Assert */
    UtAssert_True(result == CFE_SUCCESS, 
      "__wrap_CFE_SB_RcvMsg result should be %d and was %d", CFE_SUCCESS, 
      result);
    UtAssert_MemCmp(buffer, msg, msgSize, "Message in buffer is as expected"); 
    UtAssert_True(
      PipeTbl[pipe_assigned].NumberOfMessages == number_of_messages - 1, 
      "PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d and is %d", 
      pipe_assigned, number_of_messages - 1, 
      PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should have progressed to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);      
    UtAssert_True(wrap_pthread_mutex_lock_was_called, 
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called, 
      "pthread_mutex_unlock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_SuccessTimeoutValueMessageAlreadyInPipe */

void Test__wrap_CFE_SB_RcvMsg_SuccessReceivesMessageDuringWait(void)
{
    /* Arrange */
    size_t msgSize = Any_Message_Size();
    unsigned char *msg = Any_Pipe_Message(msgSize);
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();    
    CFE_SB_MsgId_t msg_id_slot = Any_Message_Id_Slot();
    uint32 previous_read_msg = Any_Pipe_Message_Location();
    uint32 current_read_msg = (previous_read_msg + 1) % 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wraps to 0 if necessary */
    uint32 number_of_messages = 1;
      
    CFE_SB_MsgPtr_t buffer;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 timeout = CFE_SB_PEND_FOREVER;
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_cond_wait_should_be_called = TRUE;
    use_wrap_pthread_cond_wait = TRUE;
    wrap_pthread_cond_wait_return_value = 0;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    pipe->NumberOfMessages = number_of_messages;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);

    /* Assert */
    UtAssert_True(result == CFE_SUCCESS, 
      "__wrap_CFE_SB_RcvMsg result should be %d and was %d", CFE_SUCCESS, 
      result);
    UtAssert_MemCmp(buffer, msg, msgSize, "Message in buffer is as expected"); 
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == 0, 
      "PipeTbl[%d].NumberOfMessages should be 1", 
      pipe_assigned, number_of_messages, 
      PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should have progressed to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);
    UtAssert_True(wrap_pthread_mutex_lock_was_called, 
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_cond_wait_was_called, 
      "pthread_cond_wait was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called, 
      "pthread_mutex_unlock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_SuccessReceivesMessageDuringWait */

void Test__wrap_CFE_SB_RcvMsg_SuccessReceivesMessageWithinTimeout(void)
{
    /* Arrange */
    size_t msgSize = Any_Message_Size();
    unsigned char *msg = Any_Pipe_Message(msgSize);
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();    
    CFE_SB_MsgId_t msg_id_slot = Any_Message_Id_Slot();
    uint32 previous_read_msg = Any_Pipe_Message_Location();
    uint32 current_read_msg = (previous_read_msg + 1) % 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wraps to 0 if necessary */
    uint32 number_of_messages = 1;
      
    CFE_SB_MsgPtr_t buffer;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 timeout = Any_Positive_int32();
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_cond_timedwait_should_be_called = TRUE;
    use_wrap_pthread_cond_timedwait = TRUE;
    wrap_pthread_cond_timedwait_return_value = 0;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    pipe->NumberOfMessages = number_of_messages;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);

    /* Assert */
    UtAssert_True(result == CFE_SUCCESS, 
      "__wrap_CFE_SB_RcvMsg result should be %d and was %d", CFE_SUCCESS, 
      result);
    UtAssert_MemCmp(buffer, msg, msgSize, "Message in buffer is as expected"); 
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == 0, 
      "PipeTbl[%d].NumberOfMessages should be 1", 
      pipe_assigned, number_of_messages, 
      PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should have progressed to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);
    UtAssert_True(wrap_pthread_mutex_lock_was_called, 
      "pthread_mutex_lock was called");
    UtAssert_True(wrap_pthread_cond_timedwait_was_called, 
      "pthread_cond_timedwait was called");
    UtAssert_True(wrap_pthread_mutex_unlock_was_called, 
      "pthread_mutex_unlock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_SuccessReceivesMessageWithinTimeout */

void Test__wrap_CFE_SB_RcvMsg_FailsPthreadMutexUnlockFailure(void)
{
    /* Arrange */
    size_t msgSize = Any_Message_Size();
    unsigned char *msg = Any_Pipe_Message(msgSize);
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();    
    CFE_SB_MsgId_t msg_id_slot = Any_Message_Id_Slot();
    uint32 previous_read_msg = Any_Pipe_Message_Location();
    uint32 current_read_msg = (previous_read_msg + 1) % 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wraps to 0 if necessary */
    uint32 number_of_messages = rand() % 
      (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1) + 2; /* 2 to MAX */
      
    CFE_SB_MsgPtr_t buffer;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 timeout = CFE_SB_POLL;
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_return_value = Any_int_Except(0);

    pipe->InUse = CFE_SBN_CLIENT_IN_USE;
    pipe->PipeId = pipe_assigned;
    pipe->SubscribedMsgIds[msg_id_slot] = msg[0] << 8 | msg[1];
    pipe->NumberOfMessages = number_of_messages;
    pipe->ReadMessage = previous_read_msg;
    
    memcpy(pipe->Messages[current_read_msg], msg, msgSize);
    
    /* Act */ 
    int32 result = CFE_SB_RcvMsg(&buffer, pipe_assigned, timeout);

    /* Assert */
    UtAssert_True(result == CFE_SB_PIPE_RD_ERR, 
      "__wrap_CFE_SB_RcvMsg result should be %d and was %d", CFE_SB_PIPE_RD_ERR, 
      result);
    UtAssert_True(buffer == NULL, "Buffer returned points to NULL");    
    UtAssert_True(
      PipeTbl[pipe_assigned].NumberOfMessages == number_of_messages - 1, 
      "PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d and is %d", 
      pipe_assigned, number_of_messages - 1, 
      PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should have progressed to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);
      UtAssert_True(wrap_pthread_mutex_lock_was_called, 
        "pthread_mutex_lock was called");
      UtAssert_True(wrap_pthread_mutex_unlock_was_called, 
        "pthread_mutex_unlock was called");
} /* end Test__wrap_CFE_SB_RcvMsg_FailsPthreadMutexUnlockFailure */

void Test__wrap_CFE_SB_RcvMsg_SuccessPipeIsFull(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;

    int previous_read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;

    int current_read_msg = (previous_read_msg + 1) % 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wrap to 0 if necessary */

    int num_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; 

    CFE_SB_MsgPtr_t buffer;

    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;

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
    
    UtAssert_True(result == CFE_SUCCESS, 
      "__wrap_CFE_SB_RcvMsg result should be %d and was %d", CFE_SUCCESS, 
      result);
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].
        Messages[current_read_msg][i], "buffer[%d] should = %d and was %d", i, 
        PipeTbl[pipe_assigned].Messages[current_read_msg][i], 
        ((unsigned char *)buffer)[i]);
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, 
      "PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d and is %d", 
      pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should have progressed to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);
} /* end Test__wrap_CFE_SB_RcvMsg_SuccessPipeIsFull */

void Test__wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    //printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    //printf("msg_id_slot = %d\n", msg_id_slot);
    int previous_read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    //printf("previous_read_msg = %d\n", previous_read_msg);
    int current_read_msg = (previous_read_msg + 1) % 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wrap to 0 if necessary */
    //printf("current_read_msg = %d\n", current_read_msg);
    int num_msg = (rand() % (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1)) + 2; 
    //printf("num_msg = %d\n", num_msg);
    CFE_SB_MsgPtr_t buffer;

    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    
    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;

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
    
    UtAssert_True(result == CFE_SUCCESS, 
      "__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", 
      CFE_SUCCESS, result);
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].
        Messages[current_read_msg][i], "buffer[%d] should = %d and was %d", i, 
        PipeTbl[pipe_assigned].Messages[current_read_msg][i], 
        ((unsigned char *)buffer)[i]);
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, 
      "PipeTbl[%d].NumberOfMessages should decrease by 1 to %d and is %d", 
      pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should progress to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);
} /* end Test__wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe */

void Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    //printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    //printf("msg_id_slot = %d\n", msg_id_slot);
    int previous_read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    //printf("previous_read_msg = %d\n", previous_read_msg);
    int current_read_msg = (previous_read_msg + 1) % 
    CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wrap to 0 if necessary */
    //printf("current_read_msg = %d\n", current_read_msg);
    int num_msg = 2; 
    //printf("num_msg = %d\n", num_msg);
    CFE_SB_MsgPtr_t buffer;

    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];

    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;

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
    
    UtAssert_True(result == CFE_SUCCESS, 
      "__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", 
      CFE_SUCCESS, result);
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].
        Messages[current_read_msg][i], 
        "buffer[%d] should = %d and was %d", i, 
        PipeTbl[pipe_assigned].Messages[current_read_msg][i], 
        ((unsigned char *)buffer)[i]);
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, 
      "PipeTbl[%d].NumberOfMessages should decrease by 1 to %d and is %d", 
      pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should progress to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);
} /* end Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe */

void Test__wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe(void)
{
    /* Arrange */
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    //printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    //printf("msg_id_slot = %d\n", msg_id_slot);
    int previous_read_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1;
    //printf("previous_read_msg = %d\n", previous_read_msg);
    int current_read_msg = 0; /* auto wrap to 0 if necessary */
    //printf("current_read_msg = %d\n", current_read_msg);
    int num_msg = (rand() % (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1)) + 2; 
    //printf("num_msg = %d\n", num_msg);
    CFE_SB_MsgPtr_t buffer;

    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];

    wrap_pthread_mutex_lock_should_be_called = TRUE;
    wrap_pthread_mutex_unlock_should_be_called = TRUE;

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
    
    UtAssert_True(result == CFE_SUCCESS, 
      "__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", 
      CFE_SUCCESS, result);
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].
        Messages[current_read_msg][i], "buffer[%d] should = %d and was %d", i, 
        PipeTbl[pipe_assigned].Messages[current_read_msg][i], 
        ((unsigned char *)buffer)[i]);
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, 
      "PipeTbl[%d].NumberOfMessages should decrease by 1 to %d and is %d", 
      pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages);
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      "PipeTbl[%d].ReadMessage should progress to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage);
} /* end Test__wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe */

/* TODO: Test__wrap_CFE_SB_RcvMsgSuccess when num messages = 1
 * TODO: Test__wrap_CFE_SB_RcvMsgSuccess when num messages = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH
 * TODO: Test__wrap_CFE_SB_RcvMsgFail when num messages = 0
 * end __wrap_CFE_SB_RcvMsg Tests */


void Test__wrap_CFE_SB_SubscribeEx_AlwaysFails(void)
{
    /* Arrange */
    int32 expectedResult = -1;
    CFE_SB_MsgId_t dummyMsgId = NULL;
    CFE_SB_PipeId_t dummyPipeId = NULL;
    CFE_SB_Qos_t dummyQuality;
    uint16 dummyMsgLim = 0;
    
    /* Act */ 
    int32 result = CFE_SB_SubscribeEx(dummyMsgId, dummyPipeId, 
      dummyQuality, dummyMsgLim);
    
    /* Assert */
    UtAssert_True(result == expectedResult, 
        "__wrap_CFE_SB_SubscribeEx failed and returned -1");
} /* end Test__wrap_CFE_SB_SubscribeEx_AlwaysFails */

void Test__wrap_CFE_SB_SubscribeLocal_AlwaysFails(void)
{
    /* Arrange */
    int32 expectedResult = -1;
    CFE_SB_MsgId_t dummyMsgId = NULL;
    CFE_SB_PipeId_t dummyPipeId = NULL;
    uint16 dummyMsgLim = 0;
    
    /* Act */ 
    int32 result = CFE_SB_SubscribeLocal(dummyMsgId, dummyPipeId,
      dummyMsgLim);
    
    /* Assert */
    UtAssert_True(result == expectedResult, 
        "__wrap_CFE_SB_SubscribeLocal failed and returned -1");
} /* end Test__wrap_CFE_SB_SubscribeLocal_AlwaysFails */

void Test__wrap_CFE_SB_Unsubscribe_AlwaysFails(void)
{
    /* Arrange */
    int32 expectedResult = -1;
    CFE_SB_MsgId_t dummyMsgId = NULL;
    CFE_SB_PipeId_t dummyPipeId = NULL;
    
    /* Act */ 
    int32 result = CFE_SB_Unsubscribe(dummyMsgId, dummyPipeId);
    
    /* Assert */
    UtAssert_True(result == expectedResult, 
        "__wrap_CFE_SB_Unsubscribe failed and returned -1");
} /* end Test__wrap_CFE_SB_Unsubscribe_AlwaysFails */

void Test__wrap_CFE_SB_UnsubscribeLocal_AlwaysFails(void)
{
    /* Arrange */
    int32 expectedResult = -1;
    CFE_SB_MsgId_t dummyMsgId = NULL;
    CFE_SB_PipeId_t dummyPipeId = NULL;
    
    /* Act */ 
    int32 result = CFE_SB_UnsubscribeLocal(dummyMsgId, dummyPipeId);
    
    /* Assert */
    UtAssert_True(result == expectedResult, 
        "__wrap_CFE_SB_UnsubscribeLocal failed and returned -1");
} /* end Test__wrap_CFE_SB_UnsubscribeLocal_AlwaysFails */

void Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails(void)
{
    /* Arrange */
    int32 expectedResult = -1;
    CFE_SB_Msg_t *dummyMsg = NULL;
    CFE_SB_ZeroCopyHandle_t dummyHandle = NULL;
    
    /* Act */ 
    int32 result = CFE_SB_ZeroCopySend(dummyMsg, dummyHandle);
    
    /* Assert */
    UtAssert_True(result == expectedResult, 
        "__wrap_CFE_SB_ZeroCopySend failed and returned -1");
} /* end Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails */

/* end __wrap_CFE_SB_RcvMsg Tests */

/*******************************************************************************
**
**  add test group functions
**
*******************************************************************************/

void add__wrap_CFE_SB_CreatePipe_tests(void)
{
  /* create pipe tests will not run with SBN_Client_Init enabled, 
     * needs more setup */
    UtTest_Add(
      Test__wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS");
    UtTest_Add(
      Test__wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly");
    UtTest_Add(
      Test__wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull");
} /* end add__wrap_CFE_SB_CreatePipe_tests */

void add__wrap_CFE_SB_DeletePipe_tests(void)
{
    UtTest_Add(
      Test__wrap_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse");
} /* end add__wrap_CFE_SB_DeletePipe_tests */

void add__wrap_CFE_SB_Subscribe(void)
{
    UtTest_Add(
      Test__wrap_CFE_SB_SubscribePipeIsValidMsgIdUnsubscribedNotAtMaxMsgIds, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_SubscribePipeIsValidMsgIdUnsubscribedNotAtMaxMsgIds");
    UtTest_Add(
      Test__wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid");
    UtTest_Add(
      Test__wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded");
} /* end add__wrap_CFE_SB_Subscribe */

void add__wrap_CFE_SB_RcvMsg_tests(void)
{
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_FailsBufferPointerIsNull, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_FailsBufferPointerIsNull");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_FailsTimeoutLessThanNegativeOne, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_FailsTimeoutLessThanNegativeOne");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_FailsInvalidPipeIdx, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_FailsInvalidPipeIdx");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_FailPthreadMutexLockFailure, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_FailPthreadMutexLockFailure");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_PollRequestReturnsWhenNoMessage, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_PollRequestReturnsWhenNoMessage");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_FailsPendWhenWaitReturnsError, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_FailsPendWhenWaitReturnsError");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_TimeoutReturnsNoMessageAfterTimeoutExpires, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_TimeoutReturnsNoMessageAfterTimeoutExpires");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_SuccessPollRequestHasMessageInPipe, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_SuccessPollRequestHasMessageInPipe");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_SuccessTimeoutPendMessageAlreadyInPipe, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_SuccessTimeoutPendMessageAlreadyInPipe");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_SuccessTimeoutValueMessageAlreadyInPipe, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_SuccessTimeoutValueMessageAlreadyInPipe");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_SuccessReceivesMessageDuringWait, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_SuccessReceivesMessageDuringWait");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_SuccessReceivesMessageWithinTimeout, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_SuccessReceivesMessageWithinTimeout");
    UtTest_Add(
      Test__wrap_CFE_SB_RcvMsg_FailsPthreadMutexUnlockFailure, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_FailsPthreadMutexUnlockFailure");
    // UtTest_Add(
    //   Test__wrap_CFE_SB_RcvMsg_SuccessPipeIsFull, 
    //   SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
    //   "Test__wrap_CFE_SB_RcvMsg_SuccessPipeIsFull");
    // UtTest_Add(
    //   Test__wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe, 
    //   SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
    //   "Test__wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe");
    // UtTest_Add(
    //   Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe, 
    //   SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
    //   "Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe");
    // UtTest_Add(
    //   Test__wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe, 
    //   SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
    //   "Test__wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe");
} /* end add__wrap_CFE_SB_RcvMsg_tests */

void add__wrap_CFE_SB_SubscribeEx_tests(void)
{
    UtTest_Add(
      Test__wrap_CFE_SB_SubscribeEx_AlwaysFails, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_SubscribeEx_AlwaysFails");
} /* end add__wrap_CFE_SB_SubscribeEx_tests */

void add__wrap_CFE_SB_SubscribeLocal_tests(void)
{
    UtTest_Add(
      Test__wrap_CFE_SB_SubscribeLocal_AlwaysFails, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_SubscribeLocal_AlwaysFails");
} /* end add__wrap_CFE_SB_SubscribeLocal_tests */

void add__wrap_CFE_SB_Unsubscribe_tests(void)
{
    UtTest_Add(
      Test__wrap_CFE_SB_Unsubscribe_AlwaysFails, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_Unsubscribe_AlwaysFails");
} /* end add__wrap_CFE_SB_Unsubscribe_tests */

void add__wrap_CFE_SB_UnsubscribeLocal_tests(void)
{
    UtTest_Add(
      Test__wrap_CFE_SB_UnsubscribeLocal_AlwaysFails,
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_UnsubscribeLocal_AlwaysFails");
} /* end add__wrap_CFE_SB_UnsubscribeLocal_tests */

void add__wrap_CFE_SB_ZeroCopySend_tests(void)
{
    UtTest_Add(
      Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails");
} /* end add__wrap_CFE_SB_ZeroCopySend_tests */

/* end add test group functions */

/*******************************************************************************
**
**  Required UtTest_Setup function for ut-assert framework
**
*******************************************************************************/

void UtTest_Setup(void)
{
    add__wrap_CFE_SB_CreatePipe_tests();
    
    add__wrap_CFE_SB_DeletePipe_tests();
    
    add__wrap_CFE_SB_Subscribe();
    
    add__wrap_CFE_SB_RcvMsg_tests();
    
    add__wrap_CFE_SB_SubscribeEx_tests();
    
    add__wrap_CFE_SB_SubscribeLocal_tests();
    
    add__wrap_CFE_SB_Unsubscribe_tests();
    
    add__wrap_CFE_SB_UnsubscribeLocal_tests();
    
    add__wrap_CFE_SB_ZeroCopySend_tests();
} /* end UtTest_Setup */

/* end Required UtTest_Setup function for ut-assert framework */
