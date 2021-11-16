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

extern const char *log_message_expected_string;
extern boolean log_message_was_called;
extern const char *perror_expected_string;

extern void (*wrap_log_message_call_func)(void);
/*******************************************************************************
**
**  SBN_Client_Logger_Tests Setup and Teardown
**
*******************************************************************************/

void SBN_Client_Logger_Tests_Setup(void)
{
    SBN_Client_Setup();
}

void SBN_Client_Logger_Tests_Teardown(void)
{
    SBN_Client_Teardown();
    

    log_message_expected_string = "";
    log_message_was_called = FALSE;
    perror_expected_string = "";
        

    /* function pointers */
    wrap_log_message_call_func = NULL;
}

/*******************************************************************************
**
**  log_message Tests
**
*******************************************************************************/

void Test_log_message_WritesExpectedNumberOfCharacters(void)
{
    /* Arrange */
    const char *test_message = "This is a test, %s %d %03f";
    const char *test_string = "two numbers:";
    int test_int = 5;
    float test_float = 5.515000;
    int32 result;
    int32 expectedResult = 39;
    
    /* Act */
    result = log_message(test_message, test_string, test_int, test_float);
    
    /* Assert */
    UtAssert_True(result == expectedResult, 
      "log_message wrote %d characters and should be %d", 
      result, expectedResult);
    
}

/* end log_message Tests */

void UtTest_Setup(void)
{
    UtTest_Add(Test_log_message_WritesExpectedNumberOfCharacters,
               SBN_Client_Logger_Tests_Setup, SBN_Client_Logger_Tests_Teardown, 
              "Test_log_message_WritesExpectedNumberOfCharacters");
}
