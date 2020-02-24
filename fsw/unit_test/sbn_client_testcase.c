

/*
 * Includes
 */

/* Test case only includes */
#include <time.h>
#include <limits.h>

#include "uttest.h"

/* sbn_client includes required to manipulate tests */
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include "sbn_client_common_test_utils.h"
#include "sbn_client_version.h"
#include "sbn_client.h"
#include "sbn_client_init.h"
#include "sbn_client_minders.h"
#include "sbn_client_ingest.h"
#include "sbn_client_utils.h"

/* UT includes */
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


extern void SBN_Client_Setup(void);
extern void SBN_Client_Teardown(void);
void add_connect_to_server_tests(void);

/* Wrapped function override variables */
const char *log_message_expected_string = "";
const char *perror_expected_string = "";

/* Wrapped function return value settings */
int wrap_socket_return_value;
uint16_t wrap_htons_return_value;
int wrap_inet_pton_return_value;
int wrap_connect_return_value;
size_t wrap_read_return_value;

void (*wrap_log_message_call_func)(void) = NULL;
void (*wrap_sleep_call_func)(void) = NULL;

void SBN_Client_Testcase_Setup(void)
{
    SBN_Client_Setup();
    
    wrap_socket_return_value = (rand() % INT_MIN) * -1;
    wrap_htons_return_value = 0;
    wrap_inet_pton_return_value = 1;
    wrap_connect_return_value = -1;
    wrap_read_return_value = INT_MIN;
}

void SBN_Client_Testcase_Teardown(void)
{
    SBN_Client_Teardown();
    
    log_message_expected_string = "";
    perror_expected_string = "";
}

/*
 * Wrapped function definitions
 */


/* Real Functions */
int    __real_log_message(const char *str);
void   __real_perror(const char *s);
size_t __real_read(int fd, void* buf, size_t cnt);


/* Wrapped Functions */
int __wrap_log_message(const char *str);
void __wrap_perror(const char *s);
int __wrap_socket(int, int, int);
uint16_t __wrap_htons(uint16_t);
int __wrap_inet_pton(int, const char *, void*);
int __wrap_connect(int, const struct sockaddr *, socklen_t);
int __wrap_connect_to_server(const char *, uint16_t);
size_t __wrap_read(int fd, void* buf, size_t cnt); 
unsigned int __wrap_sleep(unsigned int seconds);



/* SBN variable accessors */
extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
extern MsgId_to_pipes_t 
  MsgId_Subscriptions[CFE_SBN_CLIENT_MSG_ID_TO_PIPE_ID_MAP_SIZE];


/* UT_Assert external functions */
extern Ut_CFE_FS_ReturnCodeTable_t      
   Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_MAX_INDEX];
extern Ut_OSFILEAPI_HookTable_t         Ut_OSFILEAPI_HookTable;
extern Ut_CFE_ES_HookTable_t            Ut_CFE_ES_HookTable;
extern Ut_OSAPI_HookTable_t             Ut_OSAPI_HookTable;

extern int   connect_to_server(const char *, uint16_t);
extern int32 check_pthread_create_status(int, int32);

extern int sbn_client_sockfd;
extern int sbn_client_cpuId;

/*******************************************************************************
**
**  Wrapped Functions
**
*******************************************************************************/

int __wrap_log_message(const char *str)
{
    if (strlen(log_message_expected_string) > 0)
    {
        UtAssert_StrCmp(str, log_message_expected_string, 
          TestResultMsg("log_message expected string '%s' == '%s' string recieved",
          log_message_expected_string, str));
    }
    
    if (wrap_log_message_call_func != NULL)
    {
        (*wrap_log_message_call_func)();
    }
    
    return __real_log_message(str);
}

void __wrap_perror(const char *str)
{
    if (strlen(perror_expected_string) > 0)
    {
        UtAssert_StrCmp(str, perror_expected_string, 
          TestResultMsg("perror expected string '%s' == '%s' string recieved",
          perror_expected_string, str));
    }
    
    __real_perror(str);
}

int __wrap_socket(int domain, int type, int protocol)
{
  return wrap_socket_return_value;
}

uint16_t __wrap_htons(uint16_t hostshort)
{
  return wrap_htons_return_value;
}

int __wrap_inet_pton(int af, const char *src, void *dst)
{
  return wrap_inet_pton_return_value;
}

int __wrap_connect(int sbn_client_sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  return wrap_connect_return_value;
}

size_t __wrap_read(int fd, void* buf, size_t cnt)
{
    if (wrap_read_return_value != INT_MIN)
    {
        return wrap_read_return_value;
    }
    else
    {
        return __real_read(fd, buf, cnt);
    }
}

unsigned int __wrap_sleep(unsigned int seconds)
{
    if (wrap_sleep_call_func != NULL)
    {
        (*wrap_sleep_call_func)();
    }
    
    return 0;
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
      TestResultMsg("Error returned should have been %d and was %d", 
      SERVER_SOCKET_ERROR, result));
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
      TestResultMsg("error returned should have been %d and was %d", 
      SERVER_CONNECT_ERROR, result));
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
    TestResultMsg("Sockfd returned should have been %d and was %d", 
    wrap_socket_return_value, result));
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
      TestResultMsg("Error returned should have been %d and was %d", 
      SERVER_SOCKET_ERROR, result));
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
    TestResultMsg("Error returned should have been %d and was %d", 
    SERVER_INET_PTON_SRC_ERROR, result));
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
    TestResultMsg("Error returned should have been %d and was %d", 
    SERVER_INET_PTON_INVALID_AF_ERROR, result));
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
      TestResultMsg("error returned should have been %d and was %d", 
      errno, result));
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
    UtAssert_True(result == pipe, TestResultMsg(
      "CFE_SBN_Client_GetPipeIdx should have returned %d and was %d", 
      pipe, result));
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
    UtAssert_True(result == tblIdx, TestResultMsg(
      "CFE_SBN_Client_GetPipeIdx for pipeId %d should have been %d and was %d", 
      pipe, tblIdx, result));
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


/* CFE_SBN_Client_GetAvailPipeIdx Tests*/
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

void Test_starter(void)
{
    /* Arrange */
    
    
    /* Act */ 
    
    
    /* Assert */
    
}


/*************************************************/

void SBN_Client_Test_AddTestCases(void)
{
    // UtGroupSetup_Add(Test_Group_Setup);
    // UtGroupTeardown_Add(Test_Group_Teardown);
    // 
    /* check_pthread_create_status Tests */
    UtTest_Add(Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EAGAIN,
       SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
       "Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EAGAIN");
    UtTest_Add(Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EINVAL,
       SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
       "Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EINVAL");
    UtTest_Add(Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EPERM, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_check_pthread_create_status_Outlog_messageErrorWhenStatusIs_EPERM");
    UtTest_Add(Test_check_pthread_create_status_Is_errorId_WhenStatusIsNonZero, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_check_pthread_create_status_Is_errorId_WhenStatusIsNonZero");
    UtTest_Add(Test_check_pthread_create_status_Is_SBN_CLIENT_SUCCESS_When0, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_check_pthread_create_status_Is_SBN_CLIENT_SUCCESS_When0");
    
    /* connect_to_server Tests */
    add_connect_to_server_tests();
    
    /* CFE_SBN_Client_InitPipeTbl Tests */
    UtTest_Add(Test_CFE_SBN_Client_InitPipeTblFullyInitializesPipes, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_CFE_SBN_Client_InitPipeTblFullyInitializesPipes");
    
    /* CFE_SBN_Client_GetPipeIdx Tests */
    UtTest_Add(Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdEqualsPipeIdx");
    UtTest_Add(Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_CFE_SBN_Client_GetPipeIdxSuccessPipeIdDoesNotEqualPipeIdx");
    
    /* CFE_SBN_CLIENT_ReadBytes Tests*/
    UtTest_Add(Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeBroken, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeBroken");
    UtTest_Add(Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeClosed, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_CFE_SBN_CLIENT_ReadBytes_ReturnsErrorWhenPipeClosed");
    UtTest_Add(
      Test_CFE_SBN_CLIENT_ReadBytes_ReturnsCfeSuccessWhenAllBytesReceived, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_CFE_SBN_CLIENT_ReadBytes_ReturnsCfeSuccessWhenAllBytesReceived");
    
    /* CFE_SBN_Client_GetAvailPipeIdx Tests*/
    UtTest_Add(Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsErrorWhenAllPipesUsed, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsErrorWhenAllPipesUsed");
    UtTest_Add(Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsIndexForFirstOpenPipe, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_CFE_SBN_Client_GetAvailPipeIdx_ReturnsIndexForFirstOpenPipe");

}

/* Helper Functions */

void add_connect_to_server_tests(void)
{
    UtTest_Add(Test_connect_to_server_returns_sbn_client_sockfd_when_successful, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_returns_sbn_client_sockfd_when_successful");
    UtTest_Add(Test_connect_to_server_Outlog_message_EACCES_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EACCES_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outlog_message_EAFNOSUPPORT_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EAFNOSUPPORT_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outlog_message_EINVAL_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EINVAL_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outlog_message_EMFILE_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EMFILE_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outlog_message_ENOBUFS_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_ENOBUFS_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outlog_message_ENOMEM_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_ENOMEM_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outlog_message_EPROTONOSUPPORT_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EPROTONOSUPPORT_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outlog_messageUnknownErrorWhenNoCaseMatches, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_messageUnknownErrorWhenNoCaseMatches");
    UtTest_Add(
      Test_connect_to_server_returns_error_when_inet_pton_src_is_invalid, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_returns_error_when_inet_pton_src_is_invalid");
    UtTest_Add(
      Test_connect_to_server_returns_error_when_inet_pton_af_is_invalid, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_returns_error_when_inet_pton_af_is_invalid");
    UtTest_Add(Test_connect_to_server_returns_error_WhenConnectFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_returns_error_WhenConnectFails");
    UtTest_Add(Test_connect_to_server_Outlog_message_EACCES_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EACCES_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EPERM_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EPERM_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EADDRINUSE_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EADDRINUSE_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outlog_message_EADDRNOTAVAIL_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EADDRNOTAVAIL_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EAFNOSUPPORT_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EAFNOSUPPORT_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EAGAIN_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EAGAIN_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EALREADY_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EALREADY_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EBADF_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EBADF_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_ECONNREFUSED_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_ECONNREFUSED_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EFAULT_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EFAULT_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EINPROGRESS_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EINPROGRESS_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EINTR_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EINTR_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EISCONN_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EISCONN_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_ENETUNREACH_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_ENETUNREACH_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_ENOTSOCK_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_ENOTSOCK_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_EPROTOTYPE_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_EPROTOTYPE_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outlog_message_ETIMEDOUT_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outlog_message_ETIMEDOUT_errorFromConnectCall");  
}