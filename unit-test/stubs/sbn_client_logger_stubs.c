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

#include "sbn_client_logger_stubs.h"

extern boolean continue_receive_check;
extern const char *log_message_expected_string;
extern boolean log_message_was_called;
extern const char *perror_expected_string;

extern void (*wrap_log_message_call_func)(void);

int32 log_message(const char *format, ...)
{

    log_message_was_called = TRUE;
    // 
    // if (strlen(log_message_expected_string) > 0)
    // {
    //     UtAssert_StrCmp(str, log_message_expected_string, 
    //       TestResultMsg("log_message expected string '%s' == '%s' string recieved",
    //       log_message_expected_string, str));
    // }
    // 
    // if (wrap_log_message_call_func != NULL)
    // {
    //     (*wrap_log_message_call_func)();
    // }
    int result;
    int i = 0;
    int num_vars = 0;
    va_list vl;
    
    UT_Stub_CopyToLocal(UT_KEY(log_message), format, sizeof(format));
    
    va_start(vl, format);
    while (format[i] && format[i+1])
    {
        
        if (format[i] == '%')
        {
            num_vars++;
            UT_Stub_CopyToLocal(UT_KEY(log_message), va_arg(vl, int), sizeof(int));
        }
        
        i++;
    }
    
    va_end(vl);
    
    // 
    // 
    // printf("PreCheck NUM_VARS = %d\n", num_vars);
    // 
    // for(i = 0;format[i] != '\0';i++)
    // {
    // 
    //     if (format[i] == '%')
    //     { 
    //         num_vars++;
    //     }
    // 
    // }
    // printf("PostCheck NUM_VARS = %d\n", num_vars);
    
    //result =  __real_log_message(format, vl);
    
    return result;
}


