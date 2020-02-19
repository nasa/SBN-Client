

/*
 * Includes
 */

/* Test case only includes */
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>

#include "uttest.h"

/* sbn_client includes required to manipulate tests */

#include "sbn_client_common_test_utils.h"
#include "sbn_client.h"
#include "sbn_client_version.h"
#include "sbn_client_init.h"

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

/* Wrap Functions */
int __wrap_connect_to_server(const char *, uint16_t);
void __wrap_exit(int);
int __wrap_pthread_create(pthread_t *, const pthread_attr_t *,
                          void *(*) (void *), void *);


/* Real Functions */
int __real_connect_to_server(const char *, uint16_t);
void __real_CFE_SBN_Client_InitPipeTbl(void);
int __real_pthread_create(pthread_t *, const pthread_attr_t *,
                          void *(*) (void *), void *);
int32 __real_check_pthread_create_status(int, int32);


/*
 * Globals
 */

/* Wrapped function override variables */
boolean use_wrap_connect_to_server = FALSE;
int wrap_connect_to_server_return_value = INT_MIN;

int wrap_exit_expected_status = INT_MIN;  

boolean use_wrap_CFE_SBN_Client_InitPipeTbl = FALSE;

int pthread_create_errors_on_call_number = INT_MIN;
uint8 pthread_create_call_number = 0;
int pthread_create_error_value = INT_MIN;

boolean use_wrap_check_pthread_create_status = FALSE;
boolean wrap_check_pthread_create_status_fail_call = FALSE;
uint8 check_pthread_create_status_call_number = 0;
int check_pthread_create_status_errors_on_call_number = INT_MIN;


extern int sbn_client_sockfd;
extern int sbn_client_cpuId;

/*
 * Wrapped function definitions
 */
int __wrap_connect_to_server(const char *server_ip, uint16_t server_port)
{
    int result = INT_MIN;

    if (use_wrap_connect_to_server)
    {
        result = wrap_connect_to_server_return_value;
    }
    else
    {
        result = __real_connect_to_server(server_ip, server_port);
    } /* end if */

    return result;
}
 
void __wrap_exit(int status)
{
    UtAssert_True(status == wrap_exit_expected_status,
      TestResultMsg("exit() status should be %d, and was %d", 
      wrap_exit_expected_status, status));
}
 
void __wrap_CFE_SBN_Client_InitPipeTbl(void)
{
    if (use_wrap_CFE_SBN_Client_InitPipeTbl)
    {
        ; /* CFE_SBN_Client_InitPipeTbl is a void so do nothing */
    }
    else
    {
        __real_CFE_SBN_Client_InitPipeTbl();
    }
}

int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
{
    pthread_create_call_number += 1;
    int result;
    
    if (pthread_create_errors_on_call_number == pthread_create_call_number)
    {
        result = pthread_create_error_value;
    }
    else
    {
        result = 0;
    }
    
    return result;
}

int32 __wrap_check_pthread_create_status(int status, int32 errorId)
{
    int32 result = INT_MIN;
    
    check_pthread_create_status_call_number += 1;
    
    if (use_wrap_check_pthread_create_status)
    {
        if (wrap_check_pthread_create_status_fail_call &&
            check_pthread_create_status_call_number == 
            check_pthread_create_status_errors_on_call_number)
        {
            UtAssert_True(status == pthread_create_error_value, 
              "check_pthread_create_status received expected error value "
              "returned by pthread_create");
            result = errorId;
        }
        else
        {
            result = SBN_CLIENT_SUCCESS;
        }
    }
    else
    {
        result = __real_check_pthread_create_status(status, errorId);
    }
    
    return result;
}




void SBN_Client_Init_Setup(void)
{
    SBN_Client_Setup();
}

void SBN_Client_Init_Teardown(void)
{
    SBN_Client_Teardown();
    
    /* varibles to reset after use in tests so other tests are not affected */
    use_wrap_connect_to_server = FALSE;
    wrap_connect_to_server_return_value = INT_MIN;
    wrap_exit_expected_status = INT_MIN;
    use_wrap_CFE_SBN_Client_InitPipeTbl = FALSE;
    pthread_create_errors_on_call_number = INT_MIN;
    pthread_create_call_number = 0;
    pthread_create_error_value = INT_MIN;
    use_wrap_check_pthread_create_status = FALSE;
    wrap_check_pthread_create_status_fail_call = FALSE;
    check_pthread_create_status_call_number = 0;
    check_pthread_create_status_errors_on_call_number = INT_MIN;
}

/*******************************************************************************
**
**  SBN_Client_Init Tests
**
*******************************************************************************/

void Test_SBN_Client_Init_FailsBecause_connect_to_server_Fails(void)
{
    /* Arrange */
    /* connect_to_server call control */
    use_wrap_connect_to_server = TRUE;
    wrap_connect_to_server_return_value = Any_Negative_Integer();
    wrap_exit_expected_status = SBN_CLIENT_BAD_SOCK_FD_EID;

    /* Act */ 
    int32 result = SBN_Client_Init();

    /* Assert */
    /* Note during a live run of this function it will exit(sbn_client_sockfd); 
     * however during a test, exit() is wrapped and the value passed to it is 
     * checked in the wrapped function.  The check for a result is ONLY put here
     * to show that the Status gets set correctly in the function.  If the 
     * exit() assert in the wrapper passes, it shows that the function will 
     * correctly exit the operation */
    UtAssert_True(result == wrap_exit_expected_status, 
        TestResultMsg("SBN_Client_Init result should be %d, but was %d", 
        SBN_CLIENT_BAD_SOCK_FD_EID, result));
}

void Test_SBN_Client_Init_FailsBecauseCreateHeartThreadFails(void)
{
    /* Arrange */
    /* connect_to_server call control */
    use_wrap_connect_to_server = TRUE;
    wrap_connect_to_server_return_value = Any_Positive_int_Or_Zero();
    
    use_wrap_CFE_SBN_Client_InitPipeTbl = TRUE;

    /* set pthread_create error */
    pthread_create_errors_on_call_number = FIRST_CALL;
    pthread_create_error_value = Any_int_Except(0);
    
    /* set check_pthread_create_status return given error id */
    use_wrap_check_pthread_create_status = TRUE;
    wrap_check_pthread_create_status_fail_call = TRUE;
    check_pthread_create_status_errors_on_call_number = FIRST_CALL;
    
    /* set expected exit value */
    wrap_exit_expected_status = SBN_CLIENT_HEART_THREAD_CREATE_EID;

    /* Act */ 
    int32 result = SBN_Client_Init();

    /* Assert */
    UtAssert_True(result == wrap_exit_expected_status, 
        TestResultMsg("SBN_Client_Init result should be %d, but was %d", 
        SBN_CLIENT_HEART_THREAD_CREATE_EID, result));
    UtAssert_True(sbn_client_sockfd == wrap_connect_to_server_return_value,
      "SBN_Client_Init successful call to connect_to_server set "
      "sbn_client_sockfd to the returned value");
    UtAssert_True(sbn_client_cpuId == 2, "SBN_Client_Init set the "
      "sbn_client_cpuId to 2");
}

void Test_SBN_Client_Init_FailsBecauseCreateReceiveThreadFails(void)
{
    /* Arrange */
    /* connect_to_server call control */
    use_wrap_connect_to_server = TRUE;
    wrap_connect_to_server_return_value = Any_Positive_int_Or_Zero();
    
    use_wrap_CFE_SBN_Client_InitPipeTbl = TRUE;

    /* set pthread_create error */
    pthread_create_errors_on_call_number = SECOND_CALL;
    pthread_create_error_value = Any_int_Except(0);
    
    /* set check_pthread_create_status return given error id */
    use_wrap_check_pthread_create_status = TRUE;
    wrap_check_pthread_create_status_fail_call = TRUE;
    check_pthread_create_status_errors_on_call_number = SECOND_CALL;
    
    /* set expected exit value */
    wrap_exit_expected_status = SBN_CLIENT_RECEIVE_THREAD_CREATE_EID;

    /* Act */ 
    int32 result = SBN_Client_Init();

    /* Assert */
    UtAssert_True(result == SBN_CLIENT_RECEIVE_THREAD_CREATE_EID, 
        TestResultMsg("SBN_Client_Init result should be %d, but was %d", 
        SBN_CLIENT_RECEIVE_THREAD_CREATE_EID, result));
    UtAssert_True(sbn_client_sockfd == wrap_connect_to_server_return_value,
      "SBN_Client_Init successful call to connect_to_server set "
      "sbn_client_sockfd to the returned value");
    UtAssert_True(sbn_client_cpuId == 2, "SBN_Client_Init set the "
      "sbn_client_cpuId to 2");
}

void Test_SBN_Client_Init_Success(void)
{
    /* Arrange */
    /* connect_to_server call control */
    use_wrap_connect_to_server = TRUE;
    wrap_connect_to_server_return_value = Any_Positive_int_Or_Zero();
    
    use_wrap_CFE_SBN_Client_InitPipeTbl = TRUE;

    /* set pthread_create to NOT error */
    pthread_create_errors_on_call_number = 0;
    
    /* set check_pthread_create_status return given error id */
    use_wrap_check_pthread_create_status = TRUE;
    wrap_check_pthread_create_status_fail_call = FALSE;
    
    /* Act */ 
    int32 result = SBN_Client_Init();

    /* Assert */
    UtAssert_True(result == SBN_CLIENT_SUCCESS, 
      TestResultMsg("SBN_Client_Init result should be %d, but was %d", 
      SBN_CLIENT_SUCCESS, result));
    UtAssert_True(sbn_client_sockfd == wrap_connect_to_server_return_value,
      "SBN_Client_Init successful call to connect_to_server set "
      "sbn_client_sockfd to the returned value");
    UtAssert_True(sbn_client_cpuId == 2, "SBN_Client_Init set the "
      "sbn_client_cpuId to 2");
}
/* end SBN_Client_Init Tests */


// void Test_starter(void)
// {
//     /* Arrange */
// 
// 
//     /* Act */ 
// 
// 
//     /* Assert */
// 
// }


/*************************************************/

void SBN_Client_Init_Tests_AddTestCases(void)
{    
    /* SBN_Client_Init Tests */
    UtTest_Add(Test_SBN_Client_Init_FailsBecause_connect_to_server_Fails, 
               SBN_Client_Init_Setup, SBN_Client_Init_Teardown, 
              "Test_SBN_Client_Init_FailsBecause_connect_to_server_Fails");
    UtTest_Add(Test_SBN_Client_Init_FailsBecauseCreateHeartThreadFails, 
               SBN_Client_Init_Setup, SBN_Client_Init_Teardown, 
              "Test_SBN_Client_Init_FailsBecauseCreateHeartThreadFails");
    UtTest_Add(Test_SBN_Client_Init_FailsBecauseCreateReceiveThreadFails, 
               SBN_Client_Init_Setup, SBN_Client_Init_Teardown, 
              "Test_SBN_Client_Init_FailsBecauseCreateReceiveThreadFails");
    UtTest_Add(Test_SBN_Client_Init_Success, 
               SBN_Client_Init_Setup, SBN_Client_Init_Teardown, 
              "Test_SBN_Client_Init_Success");
}