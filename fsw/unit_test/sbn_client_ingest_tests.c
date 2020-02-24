#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#include <uttest.h>

#include "sbn_client_utils.h"
#include "sbn_client_ingest.h"
#include "sbn_client_common_test_utils.h"

extern const char *puts_expected_string;
extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];

int __real_CFE_SBN_CLIENT_ReadBytes(int, unsigned char *, size_t);
CFE_SB_MsgId_t __real_CFE_SBN_Client_GetMsgId(CFE_SB_MsgPtr_t);

int __wrap_CFE_SBN_CLIENT_ReadBytes(int, unsigned char *, size_t);
int __wrap_pthread_mutex_lock(pthread_mutex_t *);
int __wrap_pthread_mutex_unlock(pthread_mutex_t *);
int __wrap_pthread_cond_signal(pthread_cond_t *);
CFE_SB_MsgId_t __wrap_CFE_SBN_Client_GetMsgId(CFE_SB_MsgPtr_t);

boolean use_wrap_CFE_SBN_CLIENT_ReadBytes = FALSE;
unsigned char *wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = NULL;
int wrap_CFE_SBN_CLIENT_ReadBytes_return_value = INT_MIN;
boolean wrap_pthread_mutex_lock_should_be_called = FALSE;
boolean wrap_pthread_mutex_lock_was_called = FALSE;
boolean wrap_pthread_mutex_unlock_should_be_called = FALSE;
boolean wrap_pthread_mutex_unlock_was_called = FALSE;
boolean wrap_pthread_cond_signal_should_be_called = FALSE;
boolean wrap_pthread_cond_signal_was_called = FALSE;
boolean use_wrap_CFE_SBN_Client_GetMsgId = FALSE;
CFE_SB_MsgId_t wrap_CFE_SBN_Client_GetMsgId_return_value = 0xFFFF;

int __wrap_CFE_SBN_CLIENT_ReadBytes(int sockfd, unsigned char *msg_buffer, 
                                    size_t MsgSz)
{
    int result;
    
    if (use_wrap_CFE_SBN_CLIENT_ReadBytes)
    {
        if (wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer != NULL)
        {
            memcpy(msg_buffer, wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer, MsgSz);
        }
        result = wrap_CFE_SBN_CLIENT_ReadBytes_return_value;
    }
    else
    {
        result = __real_CFE_SBN_CLIENT_ReadBytes(sockfd, msg_buffer, MsgSz);
    }
    
    return result;
}

int __wrap_pthread_mutex_lock(pthread_mutex_t *mutex)
{
    wrap_pthread_mutex_lock_was_called = TRUE;
    
    if (!wrap_pthread_mutex_lock_should_be_called)
    {
        UtAssert_Failed("pthread_mutex_lock called, but should not have been");
    }
    
    return 0;
}

int __wrap_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    wrap_pthread_mutex_unlock_was_called = TRUE;
    
    if (!wrap_pthread_mutex_unlock_should_be_called)
    {
        UtAssert_Failed(
          "pthread_mutex_unlock called, but should not have been");
    }
    
    return 0;
}

int __wrap_pthread_cond_signal(pthread_cond_t *cond)
{
    wrap_pthread_cond_signal_was_called = TRUE;
    
    if (!wrap_pthread_cond_signal_should_be_called)
    {
        UtAssert_Failed(
          "pthread_cond_signal called, but should not have been");
    }
    
    return 0;
}

CFE_SB_MsgId_t __wrap_CFE_SBN_Client_GetMsgId(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_MsgId_t result;
    
    if (use_wrap_CFE_SBN_Client_GetMsgId)
    {
        result = wrap_CFE_SBN_Client_GetMsgId_return_value;
    }
    else
    {
        result = __real_CFE_SBN_Client_GetMsgId(MsgPtr);
    }
    
    return result;
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
    
    puts_expected_string = err_msg;
    
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

    puts_expected_string = err_msg;

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
      TestResultMsg(
      "PipeTbl[%d].NumberOfMessages %d should not increase and was %d", 
      pipe_assigned, num_msg, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, TestResultMsg(
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage));
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
    
    puts_expected_string = err_msg;
    
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
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == 0, TestResultMsg(
      "PipeTbl[%d].NumberOfMessages should = %d and was %d ", pipe_assigned, 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH, 
      PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, TestResultMsg(
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage));
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
          TestResultMsg("PipeTbl[%d].Messages[%d][%d] should = %d and was %d", 
          pipe_assigned, msg_slot, i, msg[i], 
          PipeTbl[pipe_assigned].Messages[msg_slot][i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg + 1, 
      TestResultMsg(
      "PipeTbl[%d].NumberOfMessages should increase by 1 to %d and was %d", 
      pipe_assigned, num_msg + 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, TestResultMsg(
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage));
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
          TestResultMsg("PipeTbl[%d].Messages[%d][%d] should = %d and was %d", 
          pipe_assigned, msg_slot, i, msg[i], 
          PipeTbl[pipe_assigned].Messages[msg_slot][i]));
    }
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg + 1, 
      TestResultMsg(
      "PipeTbl[%d].NumberOfMessages should increase by 1 to %d and was %d", 
      pipe_assigned, num_msg + 1, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, TestResultMsg(
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage));
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
          TestResultMsg("PipeTbl[%d].Messages[%d][%d] should = %d and was %d ", 
          pipe_assigned, msg_slot, i, msg[i], 
          PipeTbl[pipe_assigned].Messages[msg_slot][i]));
    }  
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH, TestResultMsg(
      "PipeTbl[%d].NumberOfMessages should = %d and was %d ", pipe_assigned, 
      CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH, 
      PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, TestResultMsg(
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage));
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

void SBN_Client_Ingest_Setup(void)
{
    SBN_Client_Setup();
}

void SBN_Client_Ingest_Teardown(void)
{
    SBN_Client_Teardown();

    use_wrap_CFE_SBN_CLIENT_ReadBytes = FALSE;
    wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = NULL;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = INT_MIN;
    wrap_pthread_mutex_lock_should_be_called = FALSE;
    wrap_pthread_mutex_lock_was_called = FALSE;
    wrap_pthread_mutex_unlock_should_be_called = FALSE;
    wrap_pthread_mutex_unlock_was_called = FALSE;
    wrap_pthread_cond_signal_should_be_called = FALSE;
    wrap_pthread_cond_signal_was_called = FALSE;
    use_wrap_CFE_SBN_Client_GetMsgId = FALSE;
    wrap_CFE_SBN_Client_GetMsgId_return_value = 0xFFFF;
    
    puts_expected_string = "";
}



void SBN_Client_Ingest_Tests_AddTestCases(void)
{
    /* ingest_app_message Tests */
    UtTest_Add(Test_ingest_app_message_ReadBytesFails, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_ReadBytesFails");
    UtTest_Add(Test_ingest_app_message_FailsWhenNoPipesInUse, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_FailsWhenNoPipesInUse");
    UtTest_Add(Test_ingest_app_message_FailsOverflowWhenNumberOfMessagesIsFull, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_FailsOverflowWhenNumberOfMessagesIsFull");
    UtTest_Add(Test_ingest_app_message_FailsWhenNoPipeLookingForMessageId, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_FailsWhenNoPipeLookingForMessageId");
    UtTest_Add(Test_ingest_app_message_SuccessAllSlotsAvailable, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_SuccessAllSlotsAvailable");
    UtTest_Add(Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable");
    UtTest_Add(Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft, 
      SBN_Client_Ingest_Setup, SBN_Client_Ingest_Teardown, 
      "Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft");
}