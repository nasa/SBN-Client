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

#ifndef _sbn_client_wrapped_functions_h_
#define _sbn_client_wrapped_functions_h_

#include "sbn_client_tests_includes.h"

int __real_CFE_SBN_CLIENT_ReadBytes(int, unsigned char *, size_t);
int __real_pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int __real_pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, 
  const struct timespec *);
CFE_SB_MsgId_t __real_CFE_SBN_Client_GetMsgId(CFE_SB_MsgPtr_t);
uint8 __real_CFE_SBN_Client_GetPipeIdx(CFE_SB_PipeId_t);
int __real_connect_to_server(const char *, uint16_t);
void __real_CFE_SBN_Client_InitPipeTbl(void);
int __real_pthread_create(pthread_t *, const pthread_attr_t *,
  void *(*) (void *), void *);
int32 __real_check_pthread_create_status(int, int32);
int   __real_send_heartbeat(int);
int32 __real_recv_msg(int32);
void   __real_perror(const char *s);
size_t __real_read(int fd, void* buf, size_t cnt);

int __wrap_CFE_SBN_CLIENT_ReadBytes(int, unsigned char *, size_t);
int __wrap_pthread_mutex_lock(pthread_mutex_t *);
int __wrap_pthread_mutex_unlock(pthread_mutex_t *);
int __wrap_pthread_cond_signal(pthread_cond_t *);
int __wrap_pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int __wrap_pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
  const struct timespec *);
CFE_SB_MsgId_t __wrap_CFE_SBN_Client_GetMsgId(CFE_SB_MsgPtr_t);
uint8 __wrap_CFE_SBN_Client_GetPipeIdx(CFE_SB_PipeId_t);
int __wrap_connect_to_server(const char *, uint16_t);
void __wrap_exit(int);
int __wrap_pthread_create(pthread_t *, const pthread_attr_t *,
  void *(*) (void *), void *);
int   __wrap_send_heartbeat(int);
int32 __wrap_recv_msg(int32);
void __wrap_perror(const char *s);
int __wrap_socket(int, int, int);
uint16_t __wrap_htons(uint16_t);
int __wrap_inet_pton(int, const char *, void*);
int __wrap_connect(int, const struct sockaddr *, socklen_t);
int __wrap_connect_to_server(const char *, uint16_t);
size_t __wrap_read(int fd, void* buf, size_t cnt); 
unsigned int __wrap_sleep(unsigned int seconds);

/* functions called by function pointer */
void wrap_sleep_set_continue_heartbeat_false(void);
void wrap_log_message_set_continue_recv_check_false(void);

extern boolean use_wrap_CFE_SBN_CLIENT_ReadBytes;
extern unsigned char *wrap_CFE_SBN_CLIENT_ReadBytes_msg_buffer;
extern int wrap_CFE_SBN_CLIENT_ReadBytes_return_value;
extern boolean wrap_pthread_mutex_lock_should_be_called;
extern boolean wrap_pthread_mutex_lock_was_called;
extern int wrap_pthread_mutex_lock_return_value;
extern boolean wrap_pthread_mutex_unlock_should_be_called;
extern boolean wrap_pthread_mutex_unlock_was_called;
extern int wrap_pthread_mutex_unlock_return_value;
extern boolean wrap_pthread_cond_signal_should_be_called;
extern boolean wrap_pthread_cond_signal_was_called;
extern boolean wrap_pthread_cond_wait_should_be_called;
extern boolean wrap_pthread_cond_wait_was_called;
extern boolean use_wrap_pthread_cond_wait;
extern int wrap_pthread_cond_wait_return_value;
extern boolean use_wrap_CFE_SBN_Client_GetMsgId;
extern CFE_SB_MsgId_t wrap_CFE_SBN_Client_GetMsgId_return_value;
extern boolean wrap_pthread_cond_timedwait_should_be_called;
extern boolean use_wrap_pthread_cond_timedwait;
extern boolean wrap_pthread_cond_timedwait_was_called;
extern int wrap_pthread_cond_timedwait_return_value;
extern boolean use_wrap_CFE_SBN_Client_GetPipeIdx;
extern uint8 wrap_CFE_SBN_Client_GetPipeIdx_return_value;
extern boolean use_wrap_connect_to_server;
extern int wrap_connect_to_server_return_value;
extern int wrap_exit_expected_status;  
extern boolean use_wrap_CFE_SBN_Client_InitPipeTbl;
extern int pthread_create_errors_on_call_number;
extern uint8 pthread_create_call_number;
extern int pthread_create_error_value;
extern boolean use_wrap_check_pthread_create_status;
extern boolean wrap_check_pthread_create_status_fail_call;
extern uint8 check_pthread_create_status_call_number;
extern int check_pthread_create_status_errors_on_call_number;
extern boolean use_wrap_send_heartbeat;
extern int     wrap_send_heartbeat_return_value;
extern uint8   send_hearbeat_call_number;
extern uint8   send_heartbeat_discontinue_on_call_number;
extern boolean use_wrap_recv_msg;
extern int     wrap_recv_msg_return_value;
extern uint8   recv_msg_call_number;
extern uint8   recv_msg_discontiue_on_call_number;
extern const char *log_message_expected_string;
extern boolean log_message_was_called;
extern const char *perror_expected_string;
extern int wrap_socket_return_value;
extern uint16_t wrap_htons_return_value;
extern int wrap_inet_pton_return_value;
extern int wrap_connect_return_value;
extern size_t wrap_read_return_value;

/* function pointers */

extern void (*wrap_log_message_call_func)(void);
extern void (*wrap_sleep_call_func)(void);

void SBN_CLient_Wrapped_Functions_Setup(void);
void SBN_CLient_Wrapped_Functions_Teardown(void);
#endif /* _sbn_client_wrapped_functions_h_ */