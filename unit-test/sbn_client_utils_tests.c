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

void add_connect_to_server_tests(void);

/*******************************************************************************
**
**  SBN_Client_Utils_Tests Setup and Teardown
**
*******************************************************************************/

void SBN_Client_Utils_Tests_Setup(void)
{
    SBN_Client_Setup();
    
    wrap_socket_return_value = (rand() % INT_MIN) * -1;
    wrap_htons_return_value = 0;
    wrap_inet_pton_return_value = 1;
    wrap_connect_return_value = -1;
    wrap_read_return_value = INT_MIN;
}

void SBN_Client_Utils_Tests_Teardown(void)
{
    SBN_Client_Teardown();
}

/*******************************************************************************
**
**  Helper Functions for check_pthread_create_status Tests
**
*******************************************************************************/

void check_pthread_create_status_Outlog_messageCorrectError(int error, 
       const char *error_name)
{    
    /* Arrange */ 
    char p_e_s[50];
    int status = error;
    int32 errorId = rand() % INT_MIN;
    
    sprintf(p_e_s, "Create thread error = %s", error_name);
    log_message_expected_string = p_e_s;
    perror_expected_string = "pthread_create error";
    
    /* Act */
    int32 result = check_pthread_create_status(status, errorId);
    
    /* Assert */
    UtAssert_True(result == errorId, 
        "check_pthread_create_status returned the errorId argument");
}


/*******************************************************************************
**
**  check_pthread_create_status Tests
**
*******************************************************************************/

void Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EAGAIN(void)
{
    check_pthread_create_status_Outlog_messageCorrectError(EAGAIN, "EAGAIN");
}

void Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EINVAL(void)
{    
    check_pthread_create_status_Outlog_messageCorrectError(EINVAL, "EINVAL");
}

void Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EPERM(void)
{    
    check_pthread_create_status_Outlog_messageCorrectError(EPERM, "EPERM");
}

void Test_check_pthread_create_status_Is_errorId_WhenStatusIsNonZero(void)
{
    /* Arrange */ 
    int status = rand() % INT_MIN;
    int32 errorId = rand() % INT_MIN;
    perror_expected_string = "pthread_create error";
    
    /* Act */
    int32 result = check_pthread_create_status(status, errorId);
    
    /* Assert */
    UtAssert_True(result == errorId, 
        "check_pthread_create_status returned the errorId argument");
}

void Test_check_pthread_create_status_Is_SBN_CLIENT_SUCCESS_When0(void)
{
    /* Arrange */ 
    int status = 0;
    int32 errorId = rand() % INT_MIN;
    
    /* Act */
    int32 result = check_pthread_create_status(status, errorId);
    
    /* Assert */
    UtAssert_True(result == SBN_CLIENT_SUCCESS, 
        "check_pthread_create_status returned SBN_CLIENT_SUCCESS");
}

/*******************************************************************************
**
**  Helper Functions for connect_to_server Tests
**
*******************************************************************************/

void connect_to_server_socket_fail_check(int32 expected_error, 
       const char *error_name)
{
    /* Arrange */
    char p_e_s[50];
    
    wrap_socket_return_value = -1;
    errno = expected_error;
    sprintf(p_e_s, "socket err = %s", error_name);
    log_message_expected_string = p_e_s;
    perror_expected_string = "connect_to_server socket error";
    const char * dummyIp = NULL;
    uint16_t dummyPort = 0;

    /* Act */ 
    int result = connect_to_server(dummyIp, dummyPort);
    
    /* Assert */
    UtAssert_True(result == SERVER_SOCKET_ERROR, 
      "Error returned should have been %d and was %d", 
      SERVER_SOCKET_ERROR, result);
}

void connect_to_server_connect_fail_check(int32 expected_error, 
       const char *error_name)
{
    /* Arrange */
    char p_e_s[50];
    
    wrap_socket_return_value = rand() % INT_MAX;
    wrap_htons_return_value = 0;
    wrap_inet_pton_return_value = 1;
    wrap_connect_return_value = CONNECT_ERROR_VALUE;
    errno = expected_error;
    sprintf(p_e_s, "connect err = %s", error_name);
    log_message_expected_string = p_e_s;
    perror_expected_string = "connect_to_server connect error";
    const char * dummyIp = NULL;
    uint16_t dummyPort = 0;

    /* Act */ 
    int result = connect_to_server(dummyIp, dummyPort);

    /* Assert */
    UtAssert_True(result == SERVER_CONNECT_ERROR, 
      "error returned should have been %d and was %d", 
      SERVER_CONNECT_ERROR, result);
}

/*******************************************************************************
**
**  connect_to_server Tests
**
*******************************************************************************/

void Test_connect_to_server_returns_sbn_client_sockfd_when_successful(void)
{
  /* Arrange */
  wrap_socket_return_value = rand() % INT_MAX;
  wrap_htons_return_value = 0;
  wrap_inet_pton_return_value = 1;
  wrap_connect_return_value = 0;
  const char * dummyIp = NULL;
  uint16_t dummyPort = 0;

  /* Act */ 
  int result = connect_to_server(dummyIp, dummyPort);
  
  /* Assert */
  UtAssert_True(result == wrap_socket_return_value, 
    "Sockfd returned should have been %d and was %d", 
    wrap_socket_return_value, result);
}

void Test_connect_to_server_Outlog_message_EACCES_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EACCES, "EACCES");
}

void Test_connect_to_server_Outlog_message_EAFNOSUPPORT_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EAFNOSUPPORT, "EAFNOSUPPORT");
}

void Test_connect_to_server_Outlog_message_EINVAL_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EINVAL, "EINVAL");
}

void Test_connect_to_server_Outlog_message_EMFILE_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EMFILE, "EMFILE");
}

void Test_connect_to_server_Outlog_message_ENOBUFS_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(ENOBUFS, "ENOBUFS");
}

void Test_connect_to_server_Outlog_message_ENOMEM_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(ENOMEM, "ENOMEM");
}

void Test_connect_to_server_Outlog_message_EPROTONOSUPPORT_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EPROTONOSUPPORT, "EPROTONOSUPPORT");
}

void Test_connect_to_server_Outlog_messageUnknownErrorWhenNoCaseMatches(void)
{
    /* Arrange */
    wrap_socket_return_value = -1;
    errno = 0xFFFF;
    /* TODO: printf is being used not log_message, need a better way to check */
    /* log_message_expected_string = "Unknown socket error = 65535"; */
    perror_expected_string = "connect_to_server socket error";
    const char * dummyIp = NULL;
    uint16_t dummyPort = 0;

    /* Act */ 
    int result = connect_to_server(dummyIp, dummyPort);
    
    /* Assert */
    UtAssert_True(result == SERVER_SOCKET_ERROR, 
      "Error returned should have been %d and was %d", 
      SERVER_SOCKET_ERROR, result);
}

void Test_connect_to_server_returns_error_when_inet_pton_src_is_invalid(void)
{
  /* Arrange */
  /* once inet_pton fails CUT returns error */
  wrap_socket_return_value = rand() % INT_MAX;
  wrap_htons_return_value = 0;
  wrap_inet_pton_return_value = 0;
  errno = SERVER_INET_PTON_SRC_ERROR;
  const char * dummyIp = NULL;
  uint16_t dummyPort = 0;

  /* Act */ 
  int result = connect_to_server(dummyIp, dummyPort);
  
  /* Assert */
  UtAssert_True(result == SERVER_INET_PTON_SRC_ERROR, 
    "Error returned should have been %d and was %d", 
    SERVER_INET_PTON_SRC_ERROR, result);
}

void Test_connect_to_server_returns_error_when_inet_pton_af_is_invalid(void)
{
  /* Arrange */
  /* once inet_pton fails CUT returns error */
  wrap_socket_return_value = rand() % INT_MAX;
  wrap_htons_return_value = 0;
  wrap_inet_pton_return_value = -1;
  errno = SERVER_INET_PTON_INVALID_AF_ERROR;
  const char * dummyIp = NULL;
  uint16_t dummyPort = 0;

  /* Act */ 
  int result = connect_to_server(dummyIp, dummyPort);
  
  /* Assert */
  UtAssert_True(result == SERVER_INET_PTON_INVALID_AF_ERROR, 
    "Error returned should have been %d and was %d", 
    SERVER_INET_PTON_INVALID_AF_ERROR, result);
}

void Test_connect_to_server_returns_error_WhenConnectFails(void)
{
    /* Arrange */
    wrap_socket_return_value = rand() % INT_MAX;
    wrap_htons_return_value = 0;
    wrap_inet_pton_return_value = 1;
    wrap_connect_return_value = CONNECT_ERROR_VALUE;
    errno = -4;
    const char * dummyIp = NULL;
    uint16_t dummyPort = 0;

    /* Act */ 
    int result = connect_to_server(dummyIp, dummyPort);

    /* Assert */
    UtAssert_True(result == errno, 
      "error returned should have been %d and was %d", 
      errno, result);
}

void Test_connect_to_server_Outlog_message_EACCES_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EACCES, "EACCES");
}

void Test_connect_to_server_Outlog_message_EPERM_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EPERM, "EPERM");
}

void Test_connect_to_server_Outlog_message_EADDRINUSE_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EADDRINUSE, "EADDRINUSE");
}

void Test_connect_to_server_Outlog_message_EADDRNOTAVAIL_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EADDRNOTAVAIL, "EADDRNOTAVAIL");
}

void Test_connect_to_server_Outlog_message_EAFNOSUPPORT_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EAFNOSUPPORT, "EAFNOSUPPORT");
}

void Test_connect_to_server_Outlog_message_EAGAIN_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EAGAIN, "EAGAIN");
}

void Test_connect_to_server_Outlog_message_EALREADY_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EALREADY, "EALREADY");
}

void Test_connect_to_server_Outlog_message_EBADF_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EBADF, "EBADF");
}

void Test_connect_to_server_Outlog_message_ECONNREFUSED_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(ECONNREFUSED, "ECONNREFUSED");
}

void Test_connect_to_server_Outlog_message_EFAULT_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EFAULT, "EFAULT");
}

void Test_connect_to_server_Outlog_message_EINPROGRESS_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EINPROGRESS, "EINPROGRESS");
}

void Test_connect_to_server_Outlog_message_EINTR_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EINTR, "EINTR");
}

void Test_connect_to_server_Outlog_message_EISCONN_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EISCONN, "EISCONN");
}

void Test_connect_to_server_Outlog_message_ENETUNREACH_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(ENETUNREACH, "ENETUNREACH");
}

void Test_connect_to_server_Outlog_message_ENOTSOCK_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(ENOTSOCK, "ENOTSOCK");
}

void Test_connect_to_server_Outlog_message_EPROTOTYPE_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EPROTOTYPE, "EPROTOTYPE");
}

void Test_connect_to_server_Outlog_message_ETIMEDOUT_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(ETIMEDOUT, "ETIMEDOUT");
}
/* end connect_to_server Tests */

/*******************************************************************************
**
**  CFE_SBN_Client_GetPipeIdx Tests
**
*******************************************************************************/

void Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx(void)
{
    /* Arrange */
    CFE_SB_PipeId_t pipe = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    PipeTbl[pipe].InUse = CFE_SBN_CLIENT_IN_USE;    
    PipeTbl[pipe].PipeId = pipe;
  
    /* Act */ 
    uint8 result = CFE_SBN_Client_GetPipeIdx(pipe);
  
    /* Assert */
    UtAssert_True(result == pipe, 
      "CFE_SBN_Client_GetPipeIdx should have returned %d and was %d", 
      pipe, result);
}

/* NOTE:not sure if what happens in 
 * Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx can really 
 * ever occur during runtime */
void Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx(void)  
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
    UtAssert_True(result == tblIdx, 
      "CFE_SBN_Client_GetPipeIdx for pipeId %d should have been %d and was %d", 
      pipe, tblIdx, result);
}
/* end CFE_SBN_Client_GetPipeIdx Tests */



/* CFE_SBN_CLIENT_ReadBytes Tests*/
void Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeBroken(void)
{
    /* Arrange */
    int sock_fd = (rand() % 10) + 1; /* 1 to 10 */
    size_t MsgSz = 8; /* TODO: random size generation? */
    unsigned char msg_buffer[MsgSz];
    wrap_read_return_value = -1;
    int result;
    
    /* Act */ 
    result = CFE_SBN_CLIENT_ReadBytes(sock_fd, msg_buffer, MsgSz);
    
    /* Assert */
    UtAssert_True(result == CFE_SBN_CLIENT_PIPE_BROKEN_ERR, 
      "CFE_SBN_CLIENT_ReadBytes should return CFE_SBN_CLIENT_PIPE_BROKEN_ERR");
} /* end Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeBroken */

void Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeClosed(void)
{
    /* Arrange */
    int sock_fd = (rand() % 10) + 1; /* 1 to 10 */
    size_t MsgSz = 8; /* TODO: random size generation? */
    unsigned char msg_buffer[MsgSz];
    wrap_read_return_value = 0;
    int result;
    
    /* Act */ 
    result = CFE_SBN_CLIENT_ReadBytes(sock_fd, msg_buffer, MsgSz);
    
    /* Assert */
    UtAssert_True(result == CFE_SBN_CLIENT_PIPE_CLOSED_ERR, 
        "CFE_SBN_CLIENT_ReadBytes returned CFE_SBN_CLIENT_PIPE_CLOSED_ERR");
}

void Test_CFE_SBN_CLIENT_ReadBytes_ReturnsCfeSuccessWhenAllBytesReceived(void)
{
    /* Arrange */
    int sock_fd = (rand() % 10) + 1; /* 1 to 10 */
    size_t MsgSz = 8; /* TODO: random size generation? */
    unsigned char msg_buffer[MsgSz];
    wrap_read_return_value = 8;
    int result;
    
    /* Act */ 
    result = CFE_SBN_CLIENT_ReadBytes(sock_fd, msg_buffer, MsgSz);
    
    /* Assert */
    UtAssert_True(result == CFE_SUCCESS, 
        "CFE_SBN_CLIENT_ReadBytes returned CFE_SUCCESS");
}
/* end CFE_SBN_CLIENT_ReadBytes Tests*/

/*************************************************/

void Test_CFE_SBN_Client_GetMessageSubscribeIndex_FailsMaxMessagesHit(CFE_SB_PipeId_t PipeId)
{
    /* Arrange */
    CFE_SB_PipeId_t testPipeId = 0;
    int32 expectedResult = CFE_SBN_CLIENT_MAX_MSG_IDS_MET;
    int32 result;
    int i=0;
    
    for (i = 0; i < CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE; ++i)
    {
        PipeTbl[testPipeId].SubscribedMsgIds[i] = i + 1;
    }
    
    /* Act */
    result = CFE_SBN_Client_GetMessageSubscribeIndex(testPipeId);
    
    /* Assert */
    UtAssert_True(result == expectedResult, "Expected 0x%08X got 0x%08X", expectedResult, result);
    
}

void UtTest_Setup(void)
{
    // UtGroupSetup_Add(Test_Group_Setup);
    // UtGroupTeardown_Add(Test_Group_Teardown);
    // 
    /* check_pthread_create_status Tests */
    UtTest_Add(
      Test_CFE_SBN_Client_GetMessageSubscribeIndex_FailsMaxMessagesHit,
       SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
       "Test_CFE_SBN_Client_GetMessageSubscribeIndex_FailsMaxMessagesHit");
    
    UtTest_Add(
      Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EAGAIN,
       SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
       "Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EAGAIN");
    UtTest_Add(
      Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EINVAL,
       SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
       "Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EINVAL");
    UtTest_Add(
      Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EPERM, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EPERM");
    UtTest_Add(
      Test_check_pthread_create_status_Is_errorId_WhenStatusIsNonZero, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_check_pthread_create_status_Is_errorId_WhenStatusIsNonZero");
    UtTest_Add(
      Test_check_pthread_create_status_Is_SBN_CLIENT_SUCCESS_When0, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_check_pthread_create_status_Is_SBN_CLIENT_SUCCESS_When0");
    
    /* connect_to_server Tests */
    add_connect_to_server_tests();
    
    /* CFE_SBN_Client_GetPipeIdx Tests */
    UtTest_Add(
      Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx");
    UtTest_Add(
      Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx");
    
    /* CFE_SBN_CLIENT_ReadBytes Tests*/
    UtTest_Add(
      Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeBroken, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeBroken");
    UtTest_Add(
      Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeClosed, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeClosed");
    UtTest_Add(
      Test_CFE_SBN_CLIENT_ReadBytes_ReturnsCfeSuccessWhenAllBytesReceived, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_CFE_SBN_CLIENT_ReadBytes_ReturnsCfeSuccessWhenAllBytesReceived");

}

/* Helper Functions */

void add_connect_to_server_tests(void)
{
    UtTest_Add(
      Test_connect_to_server_returns_sbn_client_sockfd_when_successful, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_returns_sbn_client_sockfd_when_successful");
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EACCES_WhenSocketFails, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EACCES_WhenSocketFails");
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EAFNOSUPPORT_WhenSocketFails, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EAFNOSUPPORT_WhenSocketFails");
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EINVAL_WhenSocketFails, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EINVAL_WhenSocketFails");
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EMFILE_WhenSocketFails, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EMFILE_WhenSocketFails");
    UtTest_Add(
      Test_connect_to_server_Outlog_message_ENOBUFS_WhenSocketFails, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_ENOBUFS_WhenSocketFails");
    UtTest_Add(
      Test_connect_to_server_Outlog_message_ENOMEM_WhenSocketFails, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_ENOMEM_WhenSocketFails");
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EPROTONOSUPPORT_WhenSocketFails, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EPROTONOSUPPORT_WhenSocketFails");
    UtTest_Add(
      Test_connect_to_server_Outlog_messageUnknownErrorWhenNoCaseMatches, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_messageUnknownErrorWhenNoCaseMatches");
    UtTest_Add(
      Test_connect_to_server_returns_error_when_inet_pton_src_is_invalid, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_returns_error_when_inet_pton_src_is_invalid");
    UtTest_Add(
      Test_connect_to_server_returns_error_when_inet_pton_af_is_invalid, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_returns_error_when_inet_pton_af_is_invalid");
    UtTest_Add(
      Test_connect_to_server_returns_error_WhenConnectFails, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_returns_error_WhenConnectFails");
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EACCES_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EACCES_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EPERM_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EPERM_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EADDRINUSE_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EADDRINUSE_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EADDRNOTAVAIL_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EADDRNOTAVAIL_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EAFNOSUPPORT_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EAFNOSUPPORT_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EAGAIN_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EAGAIN_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EALREADY_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EALREADY_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EBADF_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EBADF_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_ECONNREFUSED_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_ECONNREFUSED_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EFAULT_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EFAULT_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EINPROGRESS_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EINPROGRESS_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EINTR_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EINTR_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EISCONN_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EISCONN_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_ENETUNREACH_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_ENETUNREACH_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_ENOTSOCK_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_ENOTSOCK_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EPROTOTYPE_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_EPROTOTYPE_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_ETIMEDOUT_errorFromConnectCall, 
      SBN_Client_Utils_Tests_Setup, SBN_Client_Utils_Tests_Teardown, 
      "Test_connect_to_server_Outlog_message_ETIMEDOUT_errorFromConnectCall");  
}