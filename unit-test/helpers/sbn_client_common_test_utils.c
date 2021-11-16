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

/*
 * Globals
 */

char error_message[MAX_ERROR_MESSAGE_SIZE];
unsigned char message[CFE_SBN_CLIENT_MAX_MESSAGE_SIZE];

const char *log_message_expected_string = "";
boolean log_message_was_called = FALSE;
void (*wrap_log_message_call_func)(void) = NULL;
/*
 * Functions
 */

char *TestResultMsg(const char *format, ...)
{
  va_list vl;
  va_start(vl, format);
   
  vsnprintf(error_message, MAX_ERROR_MESSAGE_SIZE, format, vl);
  
  va_end(vl);
  
  return error_message;
}

int Any_Negative_int(void)
{
    int random_val = (rand() % INT_MAX) + 1;
    
    return random_val * -1;
}

int32 Any_Negative_int32_Except(int exception)
{
    int32 random_val = exception;
    
    while (random_val == exception)
    {
        random_val = Any_Negative_int();
    }
    
    return random_val;
}

int Any_Positive_int_Or_Zero(void)
{
    int random_val = rand();
    
    return random_val;
}

int32 Any_Positive_int32(void)
{
    int32 random_val = (rand() % INT_MAX) + 1;
    
    return random_val;
}

int Any_Non_Zero_int(void)
{
    return Any_int_Except(0);
}

int Any_int(void)
{
    int random_val = Any_Positive_int_Or_Zero();
    
    int coin_toss = rand() % 2;
    
    if (coin_toss == 1)
    {
        /* 0 to INT_MAX becomes -1 to INT_MIN */
        random_val *= -1; /* flip sign */
        random_val += -1; /* subtract 1 */ 
    }
    
    return random_val;
}

int32 Any_int32(void)
{
    return Any_int();
}

CFE_SB_PipeId_t Any_CFE_SB_PipeId_t(void)
{
    CFE_SB_PipeId_t random_val = rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPES;
    
    return random_val;
}


int Any_int_Except(int exception)
{
    int random_val = exception;
    
    while (random_val == exception)
    {
        random_val = Any_int();
    }
    
    return random_val;
}

int32 Any_int32_Except(int32 exception)
{
    int32 random_val = exception;
    
    while (random_val == exception)
    {
        random_val = Any_Positive_int_Or_Zero();
        
        int coin_toss = rand() % 2;
        
        if (coin_toss == 1)
        {
            random_val *= -1;
        }
        
    }
    
    return random_val;
}


int32 Any_int32_ExceptThese(int32 * exceptions, size_t length)
{
    int32   random_value = 0;
    boolean value_is_in_exceptions = TRUE;
    
    while (value_is_in_exceptions)
    {
        int index;
        
        random_value = Any_int();
        
        value_is_in_exceptions = FALSE;
        
        for(index = 0; index < length; index++)
        {
            
            if (exceptions[index] == random_value)
            {
                value_is_in_exceptions = TRUE;
            }
            
        }   
         
    }
    
    return random_value;
}

CFE_SB_MsgId_t Any_Message_Id_Slot(void)
{
    return (CFE_SB_MsgId_t)(rand() % CFE_SBN_CLIENT_MAX_MSG_IDS_PER_PIPE);
}

uint32 Any_Pipe_Message_Location(void)
{
    return (uint32)(rand() % CFE_PLATFORM_SBN_CLIENT_MAX_PIPE_DEPTH);
}

unsigned char Any_unsigned_char(void)
{
    unsigned char random_val = rand();
    
    return random_val;
}

unsigned char * Any_Pipe_Message(size_t length)
{
    int i;
    
    for(i = 0; i < length; i++)
    {
        message[i] = Any_unsigned_char();
    }

    return message;
}

size_t Any_Message_Size(void)
{
    size_t random_val = (rand() % CFE_SBN_CLIENT_MAX_MESSAGE_SIZE - 7) + 8;

    return random_val;
}



void SBN_Client_Setup(void)
{
    /* SBN_Client resets */
    sbn_client_sockfd = 0;
    sbn_client_cpuId = 0;

    memset(PipeTbl, 0, sizeof(PipeTbl));

    /* Global UT CFE resets -- 
    * NOTE: not sure if these are required for sbn_client */
    UT_ResetState(0);
    
    SBN_CLient_Wrapped_Functions_Setup();
} /* end SBN_Client_Setup */

void SBN_Client_Teardown(void)
{
    SBN_CLient_Wrapped_Functions_Teardown();
} /* end SBN_Client_Teardown */

