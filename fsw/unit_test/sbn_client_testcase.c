

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

CFE_SB_PipeId_t pipePtr;
uint16 pipe_depth;

/* Wrapped function override variables */
const char *puts_expected_string = "";
const char *perror_expected_string = "";

/* Wrapped function return value settings */
int wrap_socket_return_value;
uint16_t wrap_htons_return_value;
int wrap_inet_pton_return_value;
int wrap_connect_return_value;
size_t wrap_read_return_value;

void SBN_Client_Testcase_Setup(void)
{
    SBN_Client_Setup();
    
    wrap_socket_return_value = (rand() % INT_MIN) * -1;
    wrap_htons_return_value = 0;
    wrap_inet_pton_return_value = 1;
    wrap_connect_return_value = -1;
    wrap_read_return_value = INT_MIN;
    
    pipePtr = 0;
    pipe_depth = 5;
}

void SBN_Client_Testcase_Teardown(void)
{
    SBN_Client_Teardown();
    
    puts_expected_string = "";
    perror_expected_string = "";
}

/*
 * Wrapped function definitions
 */


/* Real Functions */
int    __real_puts(const char *str);
void   __real_perror(const char *s);
size_t __real_read(int fd, void* buf, size_t cnt);


/* Wrapped Functions */
int __wrap_puts(const char *str);
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


extern int32 __wrap_CFE_SB_SubscribeEx(CFE_SB_MsgId_t, 
                                       CFE_SB_PipeId_t, 
                                       CFE_SB_Qos_t, 
                                       uint16);
extern int32 __wrap_CFE_SB_SubscribeLocal(CFE_SB_MsgId_t, 
                                          CFE_SB_PipeId_t, 
                                          uint16);
extern int32 __wrap_CFE_SB_Unsubscribe(CFE_SB_MsgId_t, CFE_SB_PipeId_t);
extern int32 __wrap_CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t, CFE_SB_PipeId_t);
extern int   connect_to_server(const char *, uint16_t);
extern int32 check_pthread_create_status(int, int32);
extern int32 __wrap_CFE_SB_ZeroCopySend(CFE_SB_Msg_t *, 
                                        CFE_SB_ZeroCopyHandle_t);

extern int sbn_client_sockfd;
extern int sbn_client_cpuId;

const char *pipeName = "TestPipe";
/*******************************************************************************
**
**  Wrapped Functions
**
*******************************************************************************/

int __wrap_puts(const char *str)
{
    if (strlen(puts_expected_string) > 0)
    {
        UtAssert_StrCmp(str, puts_expected_string, 
          TestResultMsg("puts expected string '%s' == '%s' string recieved",
          puts_expected_string, str));
    }
    
    return __real_puts(str);
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
    return 0;
}

/*******************************************************************************
**
**  Helper Functions for check_pthread_create_status Tests
**
*******************************************************************************/

void check_pthread_create_status_OutputsCorrectError(int error, 
       const char *error_name)
{    
    /* Arrange */ 
    char p_e_s[50];
    int status = error;
    int32 errorId = rand() % INT_MIN;
    
    sprintf(p_e_s, "Create thread error = %s", error_name);
    puts_expected_string = p_e_s;
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

void Test_check_pthread_create_status_OutputsErrorWhenStatusIs_EAGAIN(void)
{
    check_pthread_create_status_OutputsCorrectError(EAGAIN, "EAGAIN");
}

void Test_check_pthread_create_status_OutputsErrorWhenStatusIs_EINVAL(void)
{    
    check_pthread_create_status_OutputsCorrectError(EINVAL, "EINVAL");
}

void Test_check_pthread_create_status_OutputsErrorWhenStatusIs_EPERM(void)
{    
    check_pthread_create_status_OutputsCorrectError(EPERM, "EPERM");
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
    puts_expected_string = p_e_s;
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
    puts_expected_string = p_e_s;
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

void Test_connect_to_server_Outputs_EACCES_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EACCES, "EACCES");
}

void Test_connect_to_server_Outputs_EAFNOSUPPORT_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EAFNOSUPPORT, "EAFNOSUPPORT");
}

void Test_connect_to_server_Outputs_EINVAL_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EINVAL, "EINVAL");
}

void Test_connect_to_server_Outputs_EMFILE_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EMFILE, "EMFILE");
}

void Test_connect_to_server_Outputs_ENOBUFS_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(ENOBUFS, "ENOBUFS");
}

void Test_connect_to_server_Outputs_ENOMEM_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(ENOMEM, "ENOMEM");
}

void Test_connect_to_server_Outputs_EPROTONOSUPPORT_WhenSocketFails(void)
{
    connect_to_server_socket_fail_check(EPROTONOSUPPORT, "EPROTONOSUPPORT");
}

void Test_connect_to_server_OutputsUnknownErrorWhenNoCaseMatches(void)
{
    /* Arrange */
    wrap_socket_return_value = -1;
    errno = 0xFFFF;
    /* TODO: printf is being used not puts, need a better way to check */
    /* puts_expected_string = "Unknown socket error = 65535"; */
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

void Test_connect_to_server_Outputs_EACCES_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EACCES, "EACCES");
}

void Test_connect_to_server_Outputs_EPERM_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EPERM, "EPERM");
}

void Test_connect_to_server_Outputs_EADDRINUSE_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EADDRINUSE, "EADDRINUSE");
}

void Test_connect_to_server_Outputs_EADDRNOTAVAIL_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EADDRNOTAVAIL, "EADDRNOTAVAIL");
}

void Test_connect_to_server_Outputs_EAFNOSUPPORT_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EAFNOSUPPORT, "EAFNOSUPPORT");
}

void Test_connect_to_server_Outputs_EAGAIN_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EAGAIN, "EAGAIN");
}

void Test_connect_to_server_Outputs_EALREADY_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EALREADY, "EALREADY");
}

void Test_connect_to_server_Outputs_EBADF_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EBADF, "EBADF");
}

void Test_connect_to_server_Outputs_ECONNREFUSED_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(ECONNREFUSED, "ECONNREFUSED");
}

void Test_connect_to_server_Outputs_EFAULT_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EFAULT, "EFAULT");
}

void Test_connect_to_server_Outputs_EINPROGRESS_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EINPROGRESS, "EINPROGRESS");
}

void Test_connect_to_server_Outputs_EINTR_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EINTR, "EINTR");
}

void Test_connect_to_server_Outputs_EISCONN_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EISCONN, "EISCONN");
}

void Test_connect_to_server_Outputs_ENETUNREACH_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(ENETUNREACH, "ENETUNREACH");
}

void Test_connect_to_server_Outputs_ENOTSOCK_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(ENOTSOCK, "ENOTSOCK");
}

void Test_connect_to_server_Outputs_EPROTOTYPE_errorFromConnectCall(void)
{
    connect_to_server_connect_fail_check(EPROTOTYPE, "EPROTOTYPE");
}

void Test_connect_to_server_Outputs_ETIMEDOUT_errorFromConnectCall(void)
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
**  ingest_app_message Tests
**
*******************************************************************************/

/* ingest_app_message Tests */
void Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft(void)
{
    /* Arrange */
    int p[2]; pipe(p);
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("read_msg = %d\n", read_msg);
    int num_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1; /* 1 slot left */
    printf("num_msg = %d\n", num_msg);
    int msg_slot;
    
    write(p[1], msg, msgSize);
    close(p[1]);
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = 0x1881;
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    msg_slot = read_msg + num_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    printf("msg_slot = %d\n", msg_slot);
    
    /* Act */ 
    ingest_app_message(p[0], msgSize);
    
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
}

void Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable(void)
{
    /* Arrange */
    int p[2]; pipe(p);
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("read_msg = %d\n", read_msg);
    /* from 1 to CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH */
    int num_msg = (rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH - 1) + 1; 
    printf("num_msg = %d\n", num_msg);
    int msg_slot;
    
    write(p[1], msg, msgSize);
    close(p[1]);
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = 0x1881;
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    msg_slot = read_msg + num_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    printf("msg_slot = %d\n", msg_slot);
    
    /* Act */ 
    ingest_app_message(p[0], msgSize);
    
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
}

void Test_ingest_app_message_SuccessAllSlotsAvailable(void)
{
    /* Arrange */
    int p[2]; pipe(p);
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("read_msg = %d\n", read_msg);
    int num_msg = 0;
    printf("num_msg = %d\n", num_msg);
    int msg_slot;
    
    write(p[1], msg, msgSize);
    close(p[1]);
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = 0x1881;
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    msg_slot = read_msg + num_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    printf("msg_slot = %d\n", msg_slot);
    
    /* Act */ 
    ingest_app_message(p[0], msgSize);
    
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
}

void Test_ingest_app_message_FailsWhenNumberOfMessagesIsFull(void)
{
    /* Arrange */
    int p[2]; pipe(p);
    unsigned char msg[8] = {0x18, 0x81, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x00};
    int msgSize = sizeof(msg);
    int pipe_assigned = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    printf("pipe_assigned = %d\n", pipe_assigned);
    int msg_id_slot = rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE;
    printf("msg_id_slot = %d\n", msg_id_slot);
    int read_msg = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("read_msg = %d\n", read_msg);
    int num_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    printf("num_msg = %d\n", num_msg);
    int msg_slot;
    
    write(p[1], msg, msgSize);
    close(p[1]);
    
    PipeTbl[pipe_assigned].InUse = CFE_SBN_CLIENT_IN_USE;
    PipeTbl[pipe_assigned].PipeId = pipe_assigned;
    PipeTbl[pipe_assigned].SubscribedMsgIds[msg_id_slot] = 0x1881;
    PipeTbl[pipe_assigned].NumberOfMessages = num_msg;
    PipeTbl[pipe_assigned].ReadMessage = read_msg;
    
    msg_slot = read_msg + num_msg;
    
    if (msg_slot >= CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH)
    {
        msg_slot = msg_slot - CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH;
    } 
    printf("msg_slot = %d\n", msg_slot);
    
    /* Act */ 
    ingest_app_message(p[0], msgSize);
    
    /* Assert */
    /* TODO:add failure assert here */
    UtAssert_True(PipeTbl[pipe_assigned].NumberOfMessages == num_msg, 
      TestResultMsg(
      "PipeTbl[%d].NumberOfMessages %d should not increase and was %d", 
      pipe_assigned, num_msg, PipeTbl[pipe_assigned].NumberOfMessages));
    UtAssert_True(PipeTbl[pipe_assigned].ReadMessage == read_msg, TestResultMsg(
      "PipeTbl[%d].ReadMessage should not have changed from %d and was %d", 
      pipe_assigned, read_msg, PipeTbl[pipe_assigned].ReadMessage)); 
}

//void Test_ingest_app_message_SuccessCausesPipeNumberOfMessagesToIncreaseBy1
//void Test_ingest_app_message_FailsWhenNoPipesInUse
//void Test_ingest_app_message_FailsWhenNoPipeLookingForMessageId
/* end ingest_app_message Tests */



/*******************************************************************************
**
**  __wrap_CFE_SB_RcvMsg Tests
**
*******************************************************************************/

void Test__wrap_CFE_SB_RcvMsg_SuccessPipeIsFull(void)
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
    int num_msg = CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH; 
    printf("num_msg = %d\n", num_msg);
    CFE_SB_MsgPtr_t buffer;

    CFE_SBN_Client_PipeD_t *pipe = &PipeTbl[pipe_assigned];

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

void Test__wrap_CFE_SB_SubscribeEx_AlwaysFails(void)
{
    /* Arrange */
    int32 expectedResult = -1;
    CFE_SB_MsgId_t dummyMsgId = NULL;
    CFE_SB_PipeId_t dummyPipeId = NULL;
    CFE_SB_Qos_t dummyQuality;
    uint16 dummyMsgLim = 0;
    
    /* Act */ 
    int32 result = __wrap_CFE_SB_SubscribeEx(dummyMsgId, dummyPipeId, 
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
    int32 result = __wrap_CFE_SB_SubscribeLocal(dummyMsgId, dummyPipeId,
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
    int32 result = __wrap_CFE_SB_Unsubscribe(dummyMsgId, dummyPipeId);
    
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
    int32 result = __wrap_CFE_SB_UnsubscribeLocal(dummyMsgId, dummyPipeId);
    
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
    int32 result = __wrap_CFE_SB_ZeroCopySend(dummyMsg, dummyHandle);
    
    /* Assert */
    UtAssert_True(result = expectedResult, 
        "__wrap_CFE_SB_ZeroCopySend failed and returned -1");
} /* end Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails */

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
    UtTest_Add(Test_check_pthread_create_status_OutputsErrorWhenStatusIs_EAGAIN,
       SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
       "Test_check_pthread_create_status_OutputsErrorWhenStatusIs_EAGAIN");
    UtTest_Add(Test_check_pthread_create_status_OutputsErrorWhenStatusIs_EINVAL,
       SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
       "Test_check_pthread_create_status_OutputsErrorWhenStatusIs_EINVAL");
    UtTest_Add(Test_check_pthread_create_status_OutputsErrorWhenStatusIs_EPERM, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_check_pthread_create_status_OutputsErrorWhenStatusIs_EPERM");
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
    
    /* Wrap_CFE_SB_CreatePipe Tests */
    /* create pipe tests will not run with SBN_Client_Init enabled, 
     * needs more setup */
    UtTest_Add(Test__wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_CreatePipe_Results_In_CFE_SUCCESS");
    UtTest_Add(Test__wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_CreatePipe_InitializesPipeCorrectly");
    UtTest_Add(Test__wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_CreatePipe_SendsMaxPipesErrorWhenPipesAreFull");
    
    /* WRAP_CFE_SB_DeletePipe Tests */
    UtTest_Add(Test__wrap_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_DeletePipeSuccessWhenPipeIdIsCorrectAndInUse");
    
    /* Wrap_CFE_SB_Subscribe Tests */
    UtTest_Add(
      Test__wrap_CFE_SB_SubscribePipeIsValidMsgIdUnsubscribedNotAtMaxMsgIds, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_SubscribePipeIsValidMsgIdUnsubscribedNotAtMaxMsgIds");
    UtTest_Add(Test__wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_SubscribeFailsWhenPipeIsInvalid");
    UtTest_Add(
      Test__wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_SubscribeFailsWhenNumberOfMessagesForPipeIsExceeded");
    
    /* ingest_app_message Tests */
    UtTest_Add(Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_ingest_app_message_SuccessWhenOnlyOneSlotLeft");
    UtTest_Add(Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_ingest_app_message_SuccessAnyNumberOfSlotsAvailable");
    UtTest_Add(Test_ingest_app_message_SuccessAllSlotsAvailable, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_ingest_app_message_SuccessAllSlotsAvailable");
    UtTest_Add(Test_ingest_app_message_FailsWhenNumberOfMessagesIsFull, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_ingest_app_message_FailsWhenNumberOfMessagesIsFull");
    
    /* Wrap_CFE_SB_RcvMsg Tests */
    UtTest_Add(Test__wrap_CFE_SB_RcvMsg_SuccessPipeIsFull, SBN_Client_Testcase_Setup, 
      SBN_Client_Testcase_Teardown, "Test__wrap_CFE_SB_RcvMsg_SuccessPipeIsFull");
    UtTest_Add(Test__wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_RcvMsgSuccessAtLeastTwoMessagesInPipe");
    UtTest_Add(Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe");
    UtTest_Add(Test__wrap_CFE_SB_RcvMsgSuccessPreviousMessageIsAtEndOfPipe, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test__wrap_CFE_SB_RcvMsgSuccessTwoMessagesInPipe");
    
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

    /* __wrap_CFE_SB_SubscribeEx Tests */
    UtTest_Add(Test__wrap_CFE_SB_SubscribeEx_AlwaysFails, SBN_Client_Testcase_Setup, 
      SBN_Client_Testcase_Teardown, "Test__wrap_CFE_SB_SubscribeEx_AlwaysFails");

    /* __wrap_CFE_SB_SubscribeLocal Tests */
    UtTest_Add(Test__wrap_CFE_SB_SubscribeLocal_AlwaysFails, SBN_Client_Testcase_Setup, 
      SBN_Client_Testcase_Teardown, "Test__wrap_CFE_SB_SubscribeLocal_AlwaysFails");

    /* __wrap_CFE_SB_Unsubscribe Tests */
    UtTest_Add(Test__wrap_CFE_SB_Unsubscribe_AlwaysFails, SBN_Client_Testcase_Setup, 
      SBN_Client_Testcase_Teardown, "Test__wrap_CFE_SB_Unsubscribe_AlwaysFails");

    /* __wrap_CFE_SB_UnsubscribeLocal Tests */
    UtTest_Add(Test__wrap_CFE_SB_UnsubscribeLocal_AlwaysFails, SBN_Client_Testcase_Setup, 
      SBN_Client_Testcase_Teardown, "Test__wrap_CFE_SB_UnsubscribeLocal_AlwaysFails");
    
    /* __wrap_CFE_SB_ZeroCopySend Tests */
    UtTest_Add(Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails, SBN_Client_Testcase_Setup, 
      SBN_Client_Testcase_Teardown, "Test__wrap_CFE_SB_ZeroCopySend_AlwaysFails");
}

/* Helper Functions */

void add_connect_to_server_tests(void)
{
    UtTest_Add(Test_connect_to_server_returns_sbn_client_sockfd_when_successful, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_returns_sbn_client_sockfd_when_successful");
    UtTest_Add(Test_connect_to_server_Outputs_EACCES_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EACCES_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outputs_EAFNOSUPPORT_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EAFNOSUPPORT_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outputs_EINVAL_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EINVAL_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outputs_EMFILE_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EMFILE_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outputs_ENOBUFS_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_ENOBUFS_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outputs_ENOMEM_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_ENOMEM_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_Outputs_EPROTONOSUPPORT_WhenSocketFails, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EPROTONOSUPPORT_WhenSocketFails");
    UtTest_Add(Test_connect_to_server_OutputsUnknownErrorWhenNoCaseMatches, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_OutputsUnknownErrorWhenNoCaseMatches");
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
    UtTest_Add(Test_connect_to_server_Outputs_EACCES_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EACCES_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EPERM_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EPERM_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EADDRINUSE_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EADDRINUSE_errorFromConnectCall");  
    UtTest_Add(
      Test_connect_to_server_Outputs_EADDRNOTAVAIL_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EADDRNOTAVAIL_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EAFNOSUPPORT_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EAFNOSUPPORT_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EAGAIN_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EAGAIN_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EALREADY_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EALREADY_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EBADF_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EBADF_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_ECONNREFUSED_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_ECONNREFUSED_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EFAULT_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EFAULT_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EINPROGRESS_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EINPROGRESS_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EINTR_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EINTR_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EISCONN_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EISCONN_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_ENETUNREACH_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_ENETUNREACH_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_ENOTSOCK_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_ENOTSOCK_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_EPROTOTYPE_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_EPROTOTYPE_errorFromConnectCall");  
    UtTest_Add(Test_connect_to_server_Outputs_ETIMEDOUT_errorFromConnectCall, 
      SBN_Client_Testcase_Setup, SBN_Client_Testcase_Teardown, 
      "Test_connect_to_server_Outputs_ETIMEDOUT_errorFromConnectCall");  

}