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
**  SBN_Client_Init_Tests Setup and Teardown
**
*******************************************************************************/

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
    int32 result;
    int32 expected_result = SBN_CLIENT_BAD_SOCK_FD_EID;
    /* connect_to_server call control */
    use_wrap_connect_to_server = TRUE;
    wrap_connect_to_server_return_value = Any_Negative_int();

    /* Act */ 
    result = SBN_Client_Init();

    /* Assert */
    UtAssert_True(result == expected_result, 
        "SBN_Client_Init result should be %d, but was %d", 
        SBN_CLIENT_BAD_SOCK_FD_EID, result);
}

void Test_SBN_Client_Init_FailsBecauseCreateHeartThreadFails(void)
{
    /* Arrange */
    int32 result;
    int32 expected_result = SBN_CLIENT_HEART_THREAD_CREATE_EID;
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

    /* Act */ 
    result = SBN_Client_Init();

    /* Assert */
    UtAssert_True(result == expected_result, 
        "SBN_Client_Init result should be %d, but was %d", 
        SBN_CLIENT_HEART_THREAD_CREATE_EID, result);
    UtAssert_True(sbn_client_sockfd == wrap_connect_to_server_return_value,
      "SBN_Client_Init successful call to connect_to_server set "
      "sbn_client_sockfd to the returned value");
    UtAssert_True(sbn_client_cpuId == 2, "SBN_Client_Init set the "
      "sbn_client_cpuId to 2");
}

void Test_SBN_Client_Init_FailsBecauseCreateReceiveThreadFails(void)
{
    /* Arrange */
    int32 result;
    int32 expected_result = SBN_CLIENT_RECEIVE_THREAD_CREATE_EID;
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

    /* Act */ 
    result = SBN_Client_Init();

    /* Assert */
    UtAssert_True(result == expected_result, 
        "SBN_Client_Init result should be %d, but was %d", 
        SBN_CLIENT_RECEIVE_THREAD_CREATE_EID, result);
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
      "SBN_Client_Init result should be %d, but was %d", 
      SBN_CLIENT_SUCCESS, result);
    UtAssert_True(sbn_client_sockfd == wrap_connect_to_server_return_value,
      "SBN_Client_Init successful call to connect_to_server set "
      "sbn_client_sockfd to the returned value");
    UtAssert_True(sbn_client_cpuId == 2, "SBN_Client_Init set the "
      "sbn_client_cpuId to 2");
}
/* end SBN_Client_Init Tests */


/*************************************************/

void UtTest_Setup(void)
{    
    /* SBN_Client_Init Tests */
    UtTest_Add(
      Test_SBN_Client_Init_FailsBecause_connect_to_server_Fails, 
      SBN_Client_Init_Setup, SBN_Client_Init_Teardown, 
      "Test_SBN_Client_Init_FailsBecause_connect_to_server_Fails");
    UtTest_Add(
      Test_SBN_Client_Init_FailsBecauseCreateHeartThreadFails, 
      SBN_Client_Init_Setup, SBN_Client_Init_Teardown, 
      "Test_SBN_Client_Init_FailsBecauseCreateHeartThreadFails");
    UtTest_Add(
      Test_SBN_Client_Init_FailsBecauseCreateReceiveThreadFails, 
      SBN_Client_Init_Setup, SBN_Client_Init_Teardown, 
      "Test_SBN_Client_Init_FailsBecauseCreateReceiveThreadFails");
    UtTest_Add(
      Test_SBN_Client_Init_Success, 
      SBN_Client_Init_Setup, SBN_Client_Init_Teardown, 
      "Test_SBN_Client_Init_Success");
}