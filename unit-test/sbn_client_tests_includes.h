#ifndef _sbn_client_tests_includes_h_
#define _sbn_client_tests_includes_h_

/* Library includes */
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

/* UT includes */
#include "uttest.h"
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

/* SBN_Client includes */
#include "sbn_client_ingest.h"
#include "sbn_client_init.h"
#include "sbn_client_minders.h"
#include "sbn_client_utils.h"
#include "sbn_client_version.h"
#include "sbn_client.h"

/* SBN_Client test includes */
#include "sbn_client_common_test_utils.h"
#include "sbn_client_wrapped_functions.h"

/* SBN_Client variable access */
extern int sbn_client_sockfd;
extern int sbn_client_cpuId;
extern boolean continue_heartbeat;
extern boolean continue_receive_check;
extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];


#endif /* _sbn_client_tests_includes_h_ */