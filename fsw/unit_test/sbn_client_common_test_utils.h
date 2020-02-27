#ifndef _sbn_client_common_test_utils_h_
#define _sbn_client_common_test_utils_h_

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
void SBN_Client_Setup(void);
void SBN_Client_Teardown(void);

#endif /* _sbn_client_common_test_utils_h_ */