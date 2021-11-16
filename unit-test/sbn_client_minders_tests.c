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
**  SBN_Client_Minders_Tests Setup and Teardown
**
*******************************************************************************/

void SBN_Client_Minders_Tests_Setup(void)
{
    SBN_Client_Setup();
}

void SBN_Client_Minders_Tests_Teardown(void)
{
    SBN_Client_Teardown();
}

void wrap_log_message_set_continue_recv_check_false(void)
{
    continue_receive_check = FALSE;
}

void Test_SBN_Client_HeartbeatMinder_NoLoopContinueHeartbeatFalse(void)
{
    /* Arrange */
    void * result;
    
    continue_heartbeat = FALSE;
    
    /* Act */
    result = SBN_Client_HeartbeatMinder(NULL);
    
    /* Assert */
    UtAssert_True(result == NULL, "SBN_Client_HeartbeatMinder returned NULL");
}

void Test_SBN_Client_HeartbeatMinder_HeartbeatWithSockfdZero(void)
{
    /* Arrange */
    void * result;
    sbn_client_sockfd = 0;
    
    wrap_sleep_call_func = &wrap_sleep_set_continue_heartbeat_false;

    /* Act */
    result = SBN_Client_HeartbeatMinder(NULL);

    /* Assert */
    UtAssert_True(result == NULL, "SBN_Client_HeartbeatMinder returned NULL");
}

void Test_SBN_Client_HeartbeatMinder_RunsUntilContinueHeartbeatIsFalse(void)
{
    /* Arrange */
    void * result;
    sbn_client_sockfd = Any_Non_Zero_int();
    
    /* call number becomes from 1 to 255 */
    send_heartbeat_discontinue_on_call_number = (rand() % UCHAR_MAX) + 1; 
    
    use_wrap_send_heartbeat = TRUE;
    wrap_send_heartbeat_return_value = Any_int();

    /* Act */
    result = SBN_Client_HeartbeatMinder(NULL);

    /* Assert */
    UtAssert_True(result == NULL, "SBN_Client_HeartbeatMinder returned NULL");
    UtAssert_True(send_hearbeat_call_number == 
      send_heartbeat_discontinue_on_call_number,
      "send_heartbeat was called the expected number of times");
}

void Test_SBN_Client_ReceiveMinder_NoLoopContinueReceiveCheckFalse(void)
{
    /* Arrange */
    void * result;
    
    continue_receive_check = FALSE;
    
    /* Act */
    result = SBN_Client_ReceiveMinder(NULL);
    
    /* Assert */
    UtAssert_True(result == NULL, "SBN_Client_HeartbeatMinder returned NULL");

}

void Test_SBN_Client_ReceiveMinder_Outlog_messageError(void)
{
    /* Arrange */
    char err_msg[50];
    void * result;
    
    use_wrap_recv_msg = TRUE;
    
    wrap_recv_msg_return_value = Any_int32_Except(CFE_SUCCESS);
    snprintf(err_msg, 50, "Recieve message returned error 0x%08X\n", 
      wrap_recv_msg_return_value);
    
    log_message_expected_string = err_msg;
    wrap_log_message_call_func = 
      &wrap_log_message_set_continue_recv_check_false;
    
    /* Act */
    result = SBN_Client_ReceiveMinder(NULL);
    
    /* Assert */
    UtAssert_True(result == NULL, "SBN_Client_HeartbeatMinder returned NULL"); 
}

void Test_SBN_Client_ReceiveMinder_RunsUntilContinueReceiveCheckIsFalse(void)
{
    /* Arrange */
    void * result;
    
     /* call number becomes from 1 to 255 */
    recv_msg_discontiue_on_call_number = (rand() % UCHAR_MAX) + 1;
    
    use_wrap_recv_msg = TRUE;
    wrap_recv_msg_return_value = CFE_SUCCESS;
    
    /* Act */
    result = SBN_Client_ReceiveMinder(NULL);
    
    /* Assert */
    UtAssert_True(result == NULL, "SBN_Client_HeartbeatMinder returned NULL"); 
    UtAssert_True(recv_msg_call_number == recv_msg_discontiue_on_call_number,
      "recv_msg was called the expected number of times");
}

void UtTest_Setup(void)
{
    UtTest_Add(
      Test_SBN_Client_HeartbeatMinder_NoLoopContinueHeartbeatFalse,
      SBN_Client_Minders_Tests_Setup, SBN_Client_Minders_Tests_Teardown,
      "Test_SBN_Client_HeartbeatMinder_NoLoopContinueHeartbeatFalse");
    UtTest_Add(
      Test_SBN_Client_HeartbeatMinder_HeartbeatWithSockfdZero,
      SBN_Client_Minders_Tests_Setup, SBN_Client_Minders_Tests_Teardown,
      "Test_SBN_Client_HeartbeatMinder_HeartbeatWithSockfdZero");
    UtTest_Add(
      Test_SBN_Client_HeartbeatMinder_RunsUntilContinueHeartbeatIsFalse,
      SBN_Client_Minders_Tests_Setup, SBN_Client_Minders_Tests_Teardown,
      "Test_SBN_Client_HeartbeatMinder_RunsUntilContinueHeartbeatIsFalse");

    
    
    
    UtTest_Add(
      Test_SBN_Client_ReceiveMinder_NoLoopContinueReceiveCheckFalse,
      SBN_Client_Minders_Tests_Setup, SBN_Client_Minders_Tests_Teardown,
      "Test_SBN_Client_ReceiveMinder_NoLoopContinueReceiveCheckFalse");
    UtTest_Add(
      Test_SBN_Client_ReceiveMinder_Outlog_messageError,
      SBN_Client_Minders_Tests_Setup, SBN_Client_Minders_Tests_Teardown,
      "Test_SBN_Client_ReceiveMinder_Outlog_messageError");
    UtTest_Add(
      Test_SBN_Client_ReceiveMinder_RunsUntilContinueReceiveCheckIsFalse,
      SBN_Client_Minders_Tests_Setup, SBN_Client_Minders_Tests_Teardown,
      "Test_SBN_Client_ReceiveMinder_RunsUntilContinueReceiveCheckIsFalse");
}
