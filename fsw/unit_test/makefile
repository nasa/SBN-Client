##############################################################################
## GNU Makefile for building UT unit tests

#
# Supported MAKEFILE targets:
#   clean - deletes object files, executables, output files, and gcov files
#   all   - makes utf_test_runner.exe
#   run   - runs utf_test_runner.exe
#   gcov  - prints a GCOV coverage report (make all, make run, make gcov)
#
# GCOV is disabled by default.  If you are using the source level debugger you will want to 
# disable GCOV.  To enable GCOV you can override the ENABLE_GCOV variable on the command line 
# by setting it to TRUE.  For example "make ENABLE_GCOV=TRUE".
#

CFE_PATH=../../../../cfe/fsw/cfe-core
OSAL_PATH=../../../../osal
PSP_PATH=../../../../psp

#
# VPATH specifies the search paths for source files outside of the current directory.  Note that
# all object files will be created in the current directory even if the source file is not in the 
# current directory.
#
VPATH := ../src
VPATH += ../src/PRI
VPATH += ../src/PUB
VPATH += ../../../../tools/ut_assert/src

#
# INCLUDES specifies the search paths for include files outside of the current directory.  
# Note that the -I is required. 
#
INCLUDES := -I.
INCLUDES += -I..
INCLUDES += -I../src
INCLUDES += -I../public_inc
#INCLUDES += -I../platform_inc
INCLUDES += -I../mission_inc
INCLUDES += -I../../../../tools/ut_assert/inc
INCLUDES += -I../../../../apps/sbn/fsw/platform_inc
INCLUDES += -I$(CFE_PATH)/os/inc
INCLUDES += -I$(CFE_PATH)/src/inc
INCLUDES += -I$(CFE_PATH)/src/time
INCLUDES += -I$(CFE_PATH)/src/sb
INCLUDES += -I$(CFE_PATH)/src/es
INCLUDES += -I$(CFE_PATH)/src/evs
INCLUDES += -I$(CFE_PATH)/src/fs
INCLUDES += -I$(CFE_PATH)/src/tbl
INCLUDES += -I$(CFE_PATH)/../mission_inc
INCLUDES += -I$(CFE_PATH)/../platform_inc/cpu1
INCLUDES += -I$(OSAL_PATH)/src/os/inc
INCLUDES += -I$(OSAL_PATH)/ut_assert/inc
INCLUDES += -I$(OSAL_PATH)/src/bsp/pc-linux/config/
INCLUDES += -I$(OSAL_PATH)/build/inc
INCLUDES += -I$(PSP_PATH)/fsw/inc
INCLUDES += -I$(PSP_PATH)/fsw/pc-linux/inc

#
# CF_OBJS specifies flight software object files.
#
CF_OBJS := sbn_client_init.o 
CF_OBJS += sbn_client_wrappers.o 
CF_OBJS += sbn_client.o  
CF_OBJS += sbn_client_minders.o 
CF_OBJS += sbn_client_ingest.o 
CF_OBJS += sbn_client_utils.o 

#
# UT_OBJS specifies unit test object files.
#
UT_OBJS := ut_osapi_stubs.o
UT_OBJS += ut_osfileapi_stubs.o
UT_OBJS += ut_cfe_psp_memutils_stubs.o
UT_OBJS += ut_cfe_sb_stubs.o
UT_OBJS += ut_cfe_sb_hooks.o
UT_OBJS += ut_cfe_es_stubs.o
UT_OBJS += ut_cfe_es_hooks.o
UT_OBJS += ut_cfe_evs_stubs.o
UT_OBJS += ut_cfe_evs_hooks.o
UT_OBJS += ut_cfe_tbl_stubs.o
UT_OBJS += ut_cfe_tbl_hooks.o
UT_OBJS += ut_cfe_fs_stubs.o
UT_OBJS += utassert.o
UT_OBJS += utlist.o
UT_OBJS += uttest.o
UT_OBJS += uttools.o
UT_OBJS += sbn_client_testcase.o
UT_OBJS += sbn_client_init_tests.o
UT_OBJS += sbn_client_minders_tests.o
UT_OBJS += sbn_client_ingest_tests.o
UT_OBJS += sbn_client_wrappers_tests.o
UT_OBJS += sbn_client_common_test_utils.o
UT_OBJS += sbn_client_wrapped_functions.o

###############################################################################

COMPILER=gcc
LINKER=gcc

#
# Compiler and Linker Options
#
ENABLE_GCOV = TRUE
ifeq ($(ENABLE_GCOV), TRUE)
GCOV_COPT = -fprofile-arcs -ftest-coverage -pg -p
GCOV_LOPT = -pg -p -fprofile-arcs -ftest-coverage -lgcov
endif

ENABLE_WRAP = TRUE
ifeq ($(ENABLE_WRAP), TRUE)
WRAP_LOPT := -Wl
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_CreatePipe
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_DeletePipe
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_Subscribe
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_SubscribeEx
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_SubscribeLocal
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_Unsubscribe
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_UnsubscribeLocal
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_SendMsg
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_RcvMsg
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_ZeroCopySend
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_CreatePipe
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_Subscribe
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SB_RcvMsg
WRAP_LOPT :=$(WRAP_LOPT),-wrap,socket
WRAP_LOPT :=$(WRAP_LOPT),-wrap,htons
WRAP_LOPT :=$(WRAP_LOPT),-wrap,inet_pton
WRAP_LOPT :=$(WRAP_LOPT),-wrap,connect
WRAP_LOPT :=$(WRAP_LOPT),-wrap,read
WRAP_LOPT :=$(WRAP_LOPT),-wrap,sleep
WRAP_LOPT :=$(WRAP_LOPT),-wrap,log_message
WRAP_LOPT :=$(WRAP_LOPT),-wrap,perror
WRAP_LOPT :=$(WRAP_LOPT),-wrap,pthread_create
WRAP_LOPT :=$(WRAP_LOPT),-wrap,connect_to_server
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SBN_Client_InitPipeTbl
WRAP_LOPT :=$(WRAP_LOPT),-wrap,check_pthread_create_status
WRAP_LOPT :=$(WRAP_LOPT),-wrap,send_heartbeat
WRAP_LOPT :=$(WRAP_LOPT),-wrap,recv_msg
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SBN_CLIENT_ReadBytes
WRAP_LOPT :=$(WRAP_LOPT),-wrap,pthread_mutex_lock
WRAP_LOPT :=$(WRAP_LOPT),-wrap,pthread_mutex_unlock
WRAP_LOPT :=$(WRAP_LOPT),-wrap,pthread_cond_signal
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SBN_Client_GetMsgId
WRAP_LOPT :=$(WRAP_LOPT),-wrap,CFE_SBN_Client_GetPipeIdx
WRAP_LOPT :=$(WRAP_LOPT),-wrap,pthread_cond_timedwait
WRAP_LOPT :=$(WRAP_LOPT),-wrap,pthread_cond_wait
endif

#WARNINGS = -Wall -W -ansi -Werror -Wstrict-prototypes -Wundef
WARNINGS = -Wall -Wstrict-prototypes
DEBUGGER = -g

override COPT += $(WARNINGS) $(DEBUGGER) $(GCOV_COPT) 
#COPT = $(WARNINGS) $(DEBUGGER) $(GCOV_COPT) -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -D_ix86_ 

LOPT = $(GCOV_LOPT) $(WRAP_LOPT)

###############################################################################
## Rule to make the specified TARGET
##
%.exe: %.o
	$(LINKER) -pthread $(LOPT) $^ -o $*.exe

###############################################################################
##  "C" COMPILER RULE
##
%.o: %.c
	$(COMPILER) -c $(COPT) $(INCLUDES) $<

##############################################################################
##

all:sbn_client_testrunner.exe

sbn_client_testrunner.exe: sbn_client_testrunner.o $(UT_OBJS) $(CF_OBJS) $(ENG_OBJS)

clean ::
	rm -f *.o *.exe *.gcda *.gcno *.gcov gmon.out

run ::
	./sbn_client_testrunner.exe

#gcov ::
#	@echo
#	@gcov $(CF_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
#		sed -n '/File/p' | sed '/ads/d'  | \
#		sed 's/ Lines executed:/ /; s/File/gcov:/; s/of//'
#	@rm -f *.gcda *.gcno
#	@echo

gcov ::
	@echo
	@gcov $(CF_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
         sed -n '/File/p' | sed '/ads/d' | sed -e '/\.h/d'  | \
         sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo

# end of file
