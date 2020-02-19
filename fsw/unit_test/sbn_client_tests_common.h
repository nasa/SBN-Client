#ifndef _sbn_client_tests_common_h_
#define _sbn_client_tests_common_h_

/*
 * Defines
 */


#define MAX_ERROR_MESSAGE_SIZE  120
#define CONNECT_ERROR_VALUE     -1

#define FIRST_CALL     1
#define SECOND_CALL    2

char *TestResultMsg(const char *, ...);
int Any_Negative_Integer(void);
int Any_Positive_int_Or_Zero(void);
int Any_int_Except(int exception);

#endif /* _sbn_client_tests_common_h_ */