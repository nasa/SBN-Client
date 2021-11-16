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
#include "utassert.h"
#include "utstubs.h"

/* SBN_Client includes */
#include "sbn_client_ingest.h"
#include "sbn_client_init.h"
#include "sbn_client_logger.h"
#include "sbn_client_minders.h"
#include "sbn_client_utils.h"
#include "sbn_client_version.h"
#include "sbn_client.h"

/* SBN_Client test includes */
#include "sbn_client_common_test_utils.h"
#include "sbn_client_wrapped_functions.h"
#include "sbn_client_logger_stubs.h"

/* SBN_Client variable access */
extern int sbn_client_sockfd;
extern int sbn_client_cpuId;
extern boolean continue_heartbeat;
extern boolean continue_receive_check;
extern CFE_SBN_Client_PipeD_t PipeTbl[CFE_PLATFORM_SBN_CLIENT_MAX_PIPES];
extern const char *log_message_expected_string;
extern boolean log_message_was_called;

#endif /* _sbn_client_tests_includes_h_ */