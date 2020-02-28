#include "sbn_client_tests_includes.h"

extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];

CFE_SB_PipeId_t pipePtr;
uint16 pipe_depth = 5;
const char *pipeName = "TestPipe";

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
  UtAssert_True(result == CFE_SUCCESS, TestResultMsg(
    "Pipe creation should have succeeded with (= %d), the result was (= %d)", 
    CFE_SUCCESS, result));
}

void Test__wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly(void)
{
  /* Arrange - none required */
  /* Act */ 
  CFE_SB_CreatePipe(&pipePtr, pipe_depth, pipeName);
  
  /* Assert */
  UtAssert_True(pipePtr == 0, TestResultMsg(
    "PipePtr should point to pipe 0 (initial pipe) and points to pipe %d.", 
    pipePtr));
  UtAssert_True(PipeTbl[0].InUse == CFE_SBN_CLIENT_IN_USE, TestResultMsg(
    "PipeTbl[0].InUse should be %d and was %d", CFE_SBN_CLIENT_IN_USE, 
    PipeTbl[0].InUse));
  UtAssert_True(PipeTbl[0].PipeId == 0, TestResultMsg(
    "PipeTbl[0].PipeID should be %d and was %d", 0, PipeTbl[0].PipeId));
  UtAssert_True(PipeTbl[0].SendErrors == 0, TestResultMsg(
    "PipeTbl[0].SendErrors should be %d and was %d", 0, PipeTbl[0].SendErrors));
  UtAssert_True(strcmp(&PipeTbl[0].PipeName[0], pipeName) == 0, TestResultMsg(
    "PipeTbl[0].PipeName should be %s and was %s", pipeName, 
    PipeTbl[0].PipeName));
  UtAssert_True(PipeTbl[0].NumberOfMessages == 0, TestResultMsg(
    "PipeTbl[0].NumberOfMessages should be %d and was %d", 0, 
    PipeTbl[0].NumberOfMessages));
  UtAssert_True(PipeTbl[0].ReadMessage == 0, TestResultMsg(
    "PipeTbl[0].ReadMessage should be %d and was %d", 0, 
    PipeTbl[0].ReadMessage));  
}

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
  UtAssert_True(result == CFE_SBN_CLIENT_MAX_PIPES_MET, TestResultMsg(
    "Call to CFE_SB_CreatePipe result should be %d and was %d", 
    CFE_SBN_CLIENT_MAX_PIPES_MET, result));
  //TODO:set stubs to intercept wraps on CFE calls
  //UtAssert_True(current_event_q_depth == initial_event_q_depth + 1, TestResultMsg("Event queue count should be %d, but was %d", initial_event_q_depth + 1, current_event_q_depth));
  //UtAssert_EventSent(CFE_SBN_CLIENT_MAX_PIPES_MET, CFE_EVS_ERROR, expected_error_msg, 
  //  TestResultMsg("Error event as expected was not sent. Expected: Error = %d, ErrorType=%d, Error Message = %s", CFE_SBN_CLIENT_MAX_PIPES_MET, CFE_EVS_ERROR, expected_error_msg));
}
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
  UtAssert_True(result == CFE_SUCCESS, TestResultMsg(
    "Call to CFE_SB_DeletePipe to delete pipe#%d should be %d and was %d", 
    pipeIdToDelete, CFE_SUCCESS, result));
}
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
    UtAssert_True(result == CFE_SUCCESS, TestResultMsg(
      "Call to CFE_SB_Subscribe should return %d and was %d", CFE_SUCCESS, 
      result));
    UtAssert_True(PipeTbl[pipe_id].SubscribedMsgIds[num_msgIds_subscribed] == 
      msg_id, TestResultMsg(
      "PipeTble[%d].SubscribedMsgIds[%d] should be %d and was %d", 
      pipe_id, num_msgIds_subscribed, msg_id, 
      PipeTbl[pipe_id].SubscribedMsgIds[num_msgIds_subscribed]));    
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
    UtAssert_True(result == CFE_SBN_CLIENT_BAD_ARGUMENT, TestResultMsg(
      "Call to CFE_SB_Subscribe with pipeId %d should be error %d and was %d", 
      pipe_id, CFE_SBN_CLIENT_BAD_ARGUMENT, result));
}

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
        printf("PipeTbl[%d].SubscribedMsgIds[%d] = %d\n", pipe_id, i, 
          PipeTbl[pipe_id].SubscribedMsgIds[i]);
    }
    
    /* Act */ 
    int32 result = CFE_SB_Subscribe(msg_id, pipe_id);
        
    /* Assert */
    UtAssert_True(result == CFE_SBN_CLIENT_BAD_ARGUMENT, TestResultMsg(
      "Call to CFE_SB_Subscribe with pipeId %d should be error %d and was %d", 
      pipe_id, CFE_SBN_CLIENT_BAD_ARGUMENT, result));
}
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

void Test__wrap_CFE_SB_RcvMsgFailsTimeoutLessThanNegativeOne(void)
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
} /* end Test__wrap_CFE_SB_RcvMsgFailsTimeoutLessThanNegativeOne */

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

void Test__wrap_CFE_SB_RcvMsg_PollRequestReturnsWhenNoMessage(void)
{
    /* Arrange */
    CFE_SB_MsgPtr_t buffer;
    CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();
    int32 timeout = CFE_SB_POLL;
    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
    int32 result;
    
    pipe->NumberOfMessages = 0;
    
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
}

// void Test__wrap_CFE_SB_RcvMsgTimeoutReturnsNoMessageAfterTimeoutExpires(void)
// {
//     /* Arrange */
//     CFE_SB_MsgPtr_t buffer;
//     CFE_SB_PipeId_t pipe_assigned = Any_CFE_SB_PipeId_t();
//     int32 timeout = Any_Positive_int32();
//     CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];
//     int32 result;
// 
//     wrap_pthread_mutex_lock_should_be_called = TRUE;
//     wrap_pthread_mutex_unlock_should_be_called = TRUE;
//     wrap_pthread_cond_timedwait_should_be_called = FALSE;
//     use_wrap_pthread_cond_timedwait = TRUE;
//     wrap_pthread_cond_return_value = ETIMEDOUT;
// 
//     pipe->NumberOfMessages = 0;
// 
//     /* Act */
// 
//     /* Assert */
// 
// }

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
    
    UtAssert_True(result == CFE_SUCCESS, TestResultMsg(
      "__wrap_CFE_SB_RcvMsg result should be %d and was %d", CFE_SUCCESS, 
      result));
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].
        Messages[current_read_msg][i], TestResultMsg(
        "buffer[%d] should = %d and was %d", i, 
        PipeTbl[pipe_assigned].Messages[current_read_msg][i], 
        ((unsigned char *)buffer)[i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, 
      TestResultMsg(
      "PipeTbl[%d].NumberOfMessages should have decresed by 1 to %d and is %d", 
      pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      TestResultMsg(
      "PipeTbl[%d].ReadMessage should have progressed to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage));
}

void Test__wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe(void)
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
    int current_read_msg = (previous_read_msg + 1) % 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wrap to 0 if necessary */
    printf("current_read_msg = %d\n", current_read_msg);
    int num_msg = (rand() % (CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1)) + 2; 
    printf("num_msg = %d\n", num_msg);
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
    
    UtAssert_True(result == CFE_SUCCESS, TestResultMsg(
      "__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", 
      CFE_SUCCESS, result));
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].
        Messages[current_read_msg][i], TestResultMsg(
        "buffer[%d] should = %d and was %d", i, 
        PipeTbl[pipe_assigned].Messages[current_read_msg][i], 
        ((unsigned char *)buffer)[i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, 
      TestResultMsg(
      "PipeTbl[%d].NumberOfMessages should decrease by 1 to %d and is %d", 
      pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      TestResultMsg(
      "PipeTbl[%d].ReadMessage should progress to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage));
}

void Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe(void)
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
    int current_read_msg = (previous_read_msg + 1) % 
    CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; /* auto wrap to 0 if necessary */
    printf("current_read_msg = %d\n", current_read_msg);
    int num_msg = 2; 
    printf("num_msg = %d\n", num_msg);
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
    
    UtAssert_True(result == CFE_SUCCESS, TestResultMsg(
      "__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", 
      CFE_SUCCESS, result));
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].
        Messages[current_read_msg][i], 
        TestResultMsg("buffer[%d] should = %d and was %d", i, 
        PipeTbl[pipe_assigned].Messages[current_read_msg][i], 
        ((unsigned char *)buffer)[i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, 
      TestResultMsg(
      "PipeTbl[%d].NumberOfMessages should decrease by 1 to %d and is %d", 
      pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      TestResultMsg(
      "PipeTbl[%d].ReadMessage should progress to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage));
}

void Test__wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe(void)
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
    
    UtAssert_True(result == CFE_SUCCESS, TestResultMsg(
      "__wrap_CFE_SB_RcvMsg did not succeed, result should be %d, but was %d", 
      CFE_SUCCESS, result));
    for(i = 0; i < msgSize; i++)
    {
      UtAssert_True(((unsigned char *)buffer)[i] == PipeTbl[pipe_assigned].
        Messages[current_read_msg][i], TestResultMsg(
        "buffer[%d] should = %d and was %d", i, 
        PipeTbl[pipe_assigned].Messages[current_read_msg][i], 
        ((unsigned char *)buffer)[i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg - 1, 
      TestResultMsg(
      "PipeTbl[%d].NumberOfMessages should decrease by 1 to %d and is %d", 
      pipe_assigned, num_msg - 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == current_read_msg, 
      TestResultMsg(
      "PipeTbl[%d].ReadMessage should progress to %d from %d and is %d", 
      pipe_assigned, current_read_msg, previous_read_msg, 
      PipeTbl[pipe_assigned].ReadMessage));
}

//TODO: Test__wrap_CFE_SB_RcvMsgSuccess when num messages = 1
//TODO: Test__wrap_CFE_SB_RcvMsgSuccess when num messages = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH
//TODO: Test__wrap_CFE_SB_RcvMsgFail when num messages = 0
/* end __wrap_CFE_SB_RcvMsg Tests */


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
    UtAssert_True(result = expectedResult, 
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
    UtAssert_True(result = expectedResult, 
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
    UtAssert_True(result = expectedResult, 
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
    UtAssert_True(result = expectedResult, 
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
    UtAssert_True(result = expectedResult, 
        "__wrap_CFE_SB_ZeroCopySend failed and returned -1");
} /* end Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails */


void SBN_Client_Wrappers_Tests_Setup(void)
{
    SBN_Client_Setup();
}

void SBN_Client_Wrappers_Tests_Teardown(void)
{
    SBN_Client_Teardown();  
    
    log_message_expected_string = "";
    
    pipePtr = 0;
    pipe_depth = 5;
}


void SBN_Client_Wrappers_Tests_AddTestCases(void)
{
    /* Wrap_CFE_SB_CreatePipe Tests */
    /* create pipe tests will not run with SBN_Client_Init enabled, 
     * needs more setup */
    UtTest_Add(Test__wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS");
    UtTest_Add(Test__wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly");
    UtTest_Add(Test__wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull");
    
    /* WRAP_CFE_SB_DeletePipe Tests */
    UtTest_Add(Test__wrap_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse");
    
    /* Wrap_CFE_SB_Subscribe Tests */
    UtTest_Add(
      Test__wrap_CFE_SB_SubscribePipeIsValidMsgIdUnsubscribedNotAtMaxMsgIds, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_SubscribePipeIsValidMsgIdUnsubscribedNotAtMaxMsgIds");
    UtTest_Add(Test__wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid");
    UtTest_Add(
      Test__wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded");
    
    /* Wrap_CFE_SB_RcvMsg Tests */
    UtTest_Add(Test__wrap_CFE_SB_RcvMsg_FailsBufferPointerIsNull, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_FailsBufferPointerIsNull");
    UtTest_Add(Test__wrap_CFE_SB_RcvMsgFailsTimeoutLessThanNegativeOne, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsgFailsTimeoutLessThanNegativeOne");
    UtTest_Add(Test__wrap_CFE_SB_RcvMsg_FailsInvalidPipeIdx, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_FailsInvalidPipeIdx");
    UtTest_Add(Test__wrap_CFE_SB_RcvMsg_PollRequestReturnsWhenNoMessage, 
      SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
      "Test__wrap_CFE_SB_RcvMsg_PollRequestReturnsWhenNoMessage");
    // UtTest_Add(Test__wrap_CFE_SB_RcvMsg_SuccessPipeIsFull, SBN_Client_Wrappers_Tests_Setup, 
    //   SBN_Client_Wrappers_Tests_Teardown, "Test__wrap_CFE_SB_RcvMsg_SuccessPipeIsFull");
    // UtTest_Add(Test__wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe, 
    //   SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
    //   "Test__wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe");
    // UtTest_Add(Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe, 
    //   SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
    //   "Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe");
    // UtTest_Add(Test__wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe, 
    //   SBN_Client_Wrappers_Tests_Setup, SBN_Client_Wrappers_Tests_Teardown, 
    //   "Test__wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe");
    
    /* __wrap_CFE_SB_SubscribeEx Tests */
    UtTest_Add(Test__wrap_CFE_SB_SubscribeEx_AlwaysFails, SBN_Client_Wrappers_Tests_Setup, 
      SBN_Client_Wrappers_Tests_Teardown, "Test__wrap_CFE_SB_SubscribeEx_AlwaysFails");
    
    /* __wrap_CFE_SB_SubscribeLocal Tests */
    UtTest_Add(Test__wrap_CFE_SB_SubscribeLocal_AlwaysFails, SBN_Client_Wrappers_Tests_Setup, 
      SBN_Client_Wrappers_Tests_Teardown, "Test__wrap_CFE_SB_SubscribeLocal_AlwaysFails");
    
    /* __wrap_CFE_SB_Unsubscribe Tests */
    UtTest_Add(Test__wrap_CFE_SB_Unsubscribe_AlwaysFails, SBN_Client_Wrappers_Tests_Setup, 
      SBN_Client_Wrappers_Tests_Teardown, "Test__wrap_CFE_SB_Unsubscribe_AlwaysFails");
    
    /* __wrap_CFE_SB_UnsubscribeLocal Tests */
    UtTest_Add(Test__wrap_CFE_SB_UnsubscribeLocal_AlwaysFails, SBN_Client_Wrappers_Tests_Setup, 
      SBN_Client_Wrappers_Tests_Teardown, "Test__wrap_CFE_SB_UnsubscribeLocal_AlwaysFails");
    
    /* __wrap_CFE_SB_ZeroCopySend Tests */
    UtTest_Add(Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails, SBN_Client_Wrappers_Tests_Setup, 
      SBN_Client_Wrappers_Tests_Teardown, "Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails");
}