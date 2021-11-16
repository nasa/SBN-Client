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

#include "sbn_client_wrapped_functions.h"

#define PTHREAD_MUTEX_UNLOCK_SUCCESS  0
#define PTHREAD_MUTEX_UNLOCK_FAILURE  EPERM


boolean use_wrap_CFE_SBN_CLIENT_ReadBytes = FALSE;
unsigned char *wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = NULL;
int wrap_CFE_SBN_CLIENT_ReadBytes_return_value = INT_MIN;
boolean wrap_pthread_mutex_lock_should_be_called = FALSE;
boolean wrap_pthread_mutex_lock_was_called = FALSE;
int wrap_pthread_mutex_lock_return_value = 0;
boolean wrap_pthread_mutex_unlock_should_be_called = FALSE;
boolean wrap_pthread_mutex_unlock_was_called = FALSE;
int wrap_pthread_mutex_unlock_return_value = 0;
boolean wrap_pthread_cond_wait_should_be_called = FALSE;
boolean wrap_pthread_cond_wait_was_called = FALSE;
boolean use_wrap_pthread_cond_wait = FALSE;
int wrap_pthread_cond_wait_return_value = INT_MIN;
boolean wrap_pthread_cond_signal_should_be_called = FALSE;
boolean wrap_pthread_cond_signal_was_called = FALSE;
boolean use_wrap_CFE_SBN_Client_GetMsgId = FALSE;
CFE_SB_MsgId_t wrap_CFE_SBN_Client_GetMsgId_return_value = 0xFFFF;
boolean wrap_pthread_cond_timedwait_should_be_called = FALSE;
boolean use_wrap_pthread_cond_timedwait = FALSE;
boolean wrap_pthread_cond_timedwait_was_called = FALSE;
int wrap_pthread_cond_timedwait_return_value = 0;
boolean use_wrap_CFE_SBN_Client_GetPipeIdx = FALSE;
uint8 wrap_CFE_SBN_Client_GetPipeIdx_return_value = UCHAR_MAX;
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
boolean use_wrap_send_heartbeat = FALSE;
int     wrap_send_heartbeat_return_value = SBN_CLIENT_SUCCESS;
uint8   send_hearbeat_call_number = 0;
uint8   send_heartbeat_discontinue_on_call_number = 0;
boolean use_wrap_recv_msg = FALSE;
int     wrap_recv_msg_return_value = SBN_CLIENT_SUCCESS;
uint8   recv_msg_call_number = 0;
uint8   recv_msg_discontiue_on_call_number = 0;
const char *perror_expected_string = "";
int wrap_socket_return_value;
uint16_t wrap_htons_return_value;
int wrap_inet_pton_return_value;
int wrap_connect_return_value;
size_t wrap_read_return_value;


/* function pointers */
void (*wrap_sleep_call_func)(void) = NULL;

/* Functions called by function pointer */
void wrap_sleep_set_continue_heartbeat_false(void)
{
    continue_heartbeat = FALSE;
}


int __wrap_CFE_SBN_CLIENT_ReadBytes(int sockfd, unsigned char *msg_buffer, 
                                    size_t MsgSz)
{
    int result;
    
    if (use_wrap_CFE_SBN_CLIENT_ReadBytes)
    {
        if (wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer != NULL)
        {
            memcpy(msg_buffer, wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer, MsgSz);
        }
        result = wrap_CFE_SBN_CLIENT_ReadBytes_return_value;
    }
    else
    {
        result = __real_CFE_SBN_CLIENT_ReadBytes(sockfd, msg_buffer, MsgSz);
    }
    
    return result;
}

int __wrap_pthread_mutex_lock(pthread_mutex_t *mutex)
{
    wrap_pthread_mutex_lock_was_called = TRUE;
    
    if (!wrap_pthread_mutex_lock_should_be_called)
    {
        UtAssert_Failed("pthread_mutex_lock called, but should not have been");
    }
    
    return wrap_pthread_mutex_lock_return_value;
}

int __wrap_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    wrap_pthread_mutex_unlock_was_called = TRUE;
    
    if (!wrap_pthread_mutex_unlock_should_be_called)
    {
        UtAssert_Failed(
          "pthread_mutex_unlock called, but should not have been");
          
        if (wrap_pthread_mutex_lock_was_called == TRUE)
        {
          UtAssert_Failed(
            "pthread_mutex_unlock called before calling pthread_mutex_lock");
        }
        
    }
    
    return wrap_pthread_mutex_unlock_return_value;
}

int __wrap_pthread_cond_signal(pthread_cond_t *cond)
{
    wrap_pthread_cond_signal_was_called = TRUE;
    
    if (!wrap_pthread_cond_signal_should_be_called)
    {
        UtAssert_Failed(
          "pthread_cond_signal called, but should not have been");
    }
    
    return 0;
}

int __wrap_pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex)
{
    int result;
    
    wrap_pthread_cond_wait_was_called = TRUE;
    
    if (!wrap_pthread_cond_wait_should_be_called)
    {
        UtAssert_Failed(
          "pthread_cond_wait was called, but should not have been");
    }
    
    if (use_wrap_pthread_cond_wait)
    {
        result = wrap_pthread_cond_wait_return_value;
    }
    else
    {
        result = __real_pthread_cond_wait(cond, mutex);
    }
    
    return result;
}

int __wrap_pthread_cond_timedwait(pthread_cond_t * cond,
  pthread_mutex_t * mutex, const struct timespec * abstime)
{
    int result;
    
    wrap_pthread_cond_timedwait_was_called = TRUE;
    
    if (!wrap_pthread_cond_timedwait_should_be_called)
    {
        UtAssert_Failed(
          "pthread_cond_timedwait was called, but should not have been");
    }
    
    if (use_wrap_pthread_cond_timedwait)
    {
        result = wrap_pthread_cond_timedwait_return_value;
    }
    else
    {
        result = __real_pthread_cond_timedwait(cond, mutex, abstime);
    }
    
    return result;
}

CFE_SB_MsgId_t __wrap_CFE_SBN_Client_GetMsgId(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_MsgId_t result;
    
    if (use_wrap_CFE_SBN_Client_GetMsgId)
    {
        result = wrap_CFE_SBN_Client_GetMsgId_return_value;
    }
    else
    {
        result = __real_CFE_SBN_Client_GetMsgId(MsgPtr);
    }
    
    return result;
}

uint8 __wrap_CFE_SBN_Client_GetPipeIdx(CFE_SB_PipeId_t PipeId)
{
    if (use_wrap_CFE_SBN_Client_GetPipeIdx)
    {
        return wrap_CFE_SBN_Client_GetPipeIdx_return_value;
    }
    else
    {
        return __real_CFE_SBN_Client_GetPipeIdx(PipeId);
    }
}

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
}/* NOTE: wrapper will auto discontinue after send_hearbeat_call_number
 * rolls over from 255, this is by design so any test using the wrapper cannot
 * run in an infinite loop */ 
int __wrap_send_heartbeat(int sockfd)
{
    int result;
    
    send_hearbeat_call_number += 1;
    
    if (use_wrap_send_heartbeat)
    {
        if (send_hearbeat_call_number == 
            send_heartbeat_discontinue_on_call_number)
        {
            continue_heartbeat = FALSE;
        }
        
        return wrap_send_heartbeat_return_value;
    }
    else
    {
        result = __real_send_heartbeat(sockfd);
    }
    
    return result;
} 

/* NOTE: wrapper will auto discontinue after recv_msg_call_number
 * rolls over from 255, this is by design so any test using the wrapper cannot
 * run in an infinite loop */
int32 __wrap_recv_msg(int sockfd)
{
    int32 result;
    
    recv_msg_call_number += 1;
    
    if (use_wrap_recv_msg)
    {
        if (recv_msg_call_number == recv_msg_discontiue_on_call_number)
        {
            continue_receive_check = FALSE;
        }
        
        return wrap_recv_msg_return_value;
    }
    else
    {
        result = __real_recv_msg(sockfd);
    }
    
    return result;
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





void SBN_CLient_Wrapped_Functions_Setup(void)
{
    ; /* SBN_CLient_Wrapped_Functions_Setup currently does nothing */
}

void SBN_CLient_Wrapped_Functions_Teardown(void)
{
    /* SBN_CLient_Wrapped_Functions_Teardown resets all variables */
    use_wrap_CFE_SBN_CLIENT_ReadBytes = FALSE;
    wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer = NULL;
    wrap_CFE_SBN_CLIENT_ReadBytes_return_value = INT_MIN;
    wrap_pthread_mutex_lock_should_be_called = FALSE;
    wrap_pthread_mutex_lock_was_called = FALSE;
    wrap_pthread_mutex_lock_return_value = 0;
    wrap_pthread_mutex_unlock_should_be_called = FALSE;
    wrap_pthread_mutex_unlock_was_called = FALSE;
    wrap_pthread_mutex_unlock_return_value = 0;
    wrap_pthread_cond_wait_should_be_called = FALSE;
    wrap_pthread_cond_wait_was_called = FALSE;
    use_wrap_pthread_cond_wait = FALSE;
    wrap_pthread_cond_wait_return_value = INT_MIN;
    wrap_pthread_cond_signal_should_be_called = FALSE;
    wrap_pthread_cond_signal_was_called = FALSE;
    use_wrap_CFE_SBN_Client_GetMsgId = FALSE;
    wrap_CFE_SBN_Client_GetMsgId_return_value = 0xFFFF;
    wrap_pthread_cond_timedwait_should_be_called = FALSE;
    use_wrap_pthread_cond_timedwait = FALSE;
    wrap_pthread_cond_timedwait_was_called = FALSE;
    wrap_pthread_cond_timedwait_return_value = 0;
    use_wrap_CFE_SBN_Client_GetPipeIdx = FALSE;
    wrap_CFE_SBN_Client_GetPipeIdx_return_value = UCHAR_MAX;
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
    use_wrap_send_heartbeat = FALSE;
    wrap_send_heartbeat_return_value = SBN_CLIENT_SUCCESS;
    send_hearbeat_call_number = 0;
    send_heartbeat_discontinue_on_call_number = 0;
    use_wrap_recv_msg = FALSE;
    wrap_recv_msg_return_value = SBN_CLIENT_SUCCESS;
    recv_msg_call_number = 0;
    recv_msg_discontiue_on_call_number = 0;
    perror_expected_string = "";
        
    
    /* function pointers */
    wrap_sleep_call_func = NULL;
    
    /* external resets */    
    continue_heartbeat = TRUE;
    continue_receive_check = TRUE;
}

