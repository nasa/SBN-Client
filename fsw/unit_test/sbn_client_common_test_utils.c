#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdarg.h>

#include "sbn_client_common_test_utils.h"
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

/*
 * Globals
 */

extern int sbn_client_sockfd;
extern int sbn_client_cpuId;

extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];

char em[MAX_ERROR_MESSAGE_SIZE];

char *TestResultMsg(const char *format, ...)
{
  va_list vl;
  va_start(vl, format);
   
  vsnprintf(em, MAX_ERROR_MESSAGE_SIZE, format, vl);
  
  va_end(vl);
  
  return em;
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

void SBN_Client_Setup(void)
{
  /* SBN_Client resets */
  sbn_client_sockfd = 0;
  sbn_client_cpuId = 0;
  
  memset(PipeTbl, 0, sizeof(PipeTbl));
    
  /* Global UT CFE resets -- 
   * NOTE: not sure if these are required for sbn_client */
  Ut_OSAPI_Reset();
  Ut_CFE_SB_Reset();
  Ut_CFE_ES_Reset();
  Ut_CFE_EVS_Reset();
  Ut_CFE_TBL_Reset();
} /* end SBN_Client_Setup */

void SBN_Client_Teardown(void)
{
    ; /* SBN_Client_Teardown is currently empty by design */
} /* end SBN_Client_Teardown */

