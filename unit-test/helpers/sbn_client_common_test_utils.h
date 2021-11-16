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

#ifndef _sbn_client_common_test_utils_h_
#define _sbn_client_common_test_utils_h_

#include "sbn_client_tests_includes.h"

/*
 * Defines
 */

#define MAX_ERROR_MESSAGE_SIZE  120
#define CONNECT_ERROR_VALUE     -1

#define FIRST_CALL     1
#define SECOND_CALL    2

char *TestResultMsg(const char *, ...);
int Any_Negative_int(void);
int32 Any_Negative_int32_Except(int);
int Any_Positive_int_Or_Zero(void);
int32 Any_Positive_int32(void);
int Any_int_Except(int);
int Any_int(void);
int32 Any_int32(void);
int Any_Non_Zero_int(void);
int32 Any_int32_Except(int32);
int32 Any_int32_ExceptThese(int32 *, size_t);
CFE_SB_PipeId_t Any_CFE_SB_PipeId_t(void);
CFE_SB_MsgId_t Any_Message_Id_Slot(void);
uint32 Any_Pipe_Message_Location(void);
unsigned char Any_unsigned_char(void);
unsigned char * Any_Pipe_Message(size_t length);
size_t Any_Message_Size(void);



void SBN_Client_Setup(void);
void SBN_Client_Teardown(void);

#endif /* _sbn_client_common_test_utils_h_ */