#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdarg.h>

#include "sbn_client_tests_common.h"

char em[MAX_ERROR_MESSAGE_SIZE];

char *TestResultMsg(const char *format, ...)
{
  va_list vl;
  va_start(vl, format);
   
  vsnprintf(em, MAX_ERROR_MESSAGE_SIZE, format, vl);
  
  va_end(vl);
  
  return em;
}

int Any_Negative_Integer(void)
{
    int random_val = (rand() % INT_MAX) + 1;
    
    return random_val * -1;
}

int Any_Positive_int_Or_Zero(void)
{
    int random_val = rand() % INT_MAX;
    
    return random_val;
}

int Any_int_Except(int exception)
{
    int random_val = exception;
    
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