#include "sbn_client_tests_includes.h"

extern void SBN_Client_Setup(void);
extern void SBN_Client_Teardown(void);

/*
 * Globals
 */

extern int sbn_client_sockfd;
extern int sbn_client_cpuId;

void SBN_Client_Init_Setup(void)
{
    SBN_Client_Setup();
}

void SBN_Client_Init_Teardown(void)
{
    SBN_Client_Teardown();
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
    wrap_connect_to_server_return_value = Any_Negative_int();
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