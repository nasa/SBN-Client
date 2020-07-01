##################################################################
#
# Coverage Unit Test build recipe
#
# This CMake file contains the recipe for building the sample unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

#
#
# NOTE on the subdirectory structures here:
#
# - "inc" provides local header files shared between the coveragetest,
#    wrappers, and overrides source code units
# - "coveragetest" contains source code for the actual unit test cases
#    The primary objective is to get line/path coverage on the FSW 
#    code units.
# - "wrappers" contains wrappers for the FSW code.  The wrapper adds
#    any UT-specific scaffolding to facilitate the coverage test, and
#    includes the unmodified FSW source file.
#
 
set(UT_NAME sbn_client) # UT_NAME is used in severl locations later in this file

# Use the UT assert public API, and allow direct
# inclusion of source files that are normally private
include_directories(${osal_MISSION_DIR}/ut_assert/inc)  #osal_MISSION_DIR = ${MISSION_SOURCE_DIR}/osal, UT source files for ut_assert
include_directories(${PROJECT_SOURCE_DIR}/fsw/src)      # PROJECT_SOURCE_DIR is set by the last call to project() command, sample_app?
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)    # CMAKE_CURRENT_SOURCE_DIR Path to the current source directory being processed, sample_app/unit_test?
include_directories(${PROJECT_SOURCE_DIR}/unit-test/helpers)
include_directories(${PROJECT_SOURCE_DIR}/unit-test/stubs)
include_directories(${PROJECT_SOURCE_DIR}/unit-test)

# Setting wrappers for how things are done currently.
set(WRAPS -Wl)
set(WRAPS "${WRAPS},-wrap,CFE_SB_CreatePipe")
set(WRAPS "${WRAPS},-wrap,CFE_SB_DeletePipe")
set(WRAPS "${WRAPS},-wrap,CFE_SB_Subscribe")
set(WRAPS "${WRAPS},-wrap,CFE_SB_SubscribeEx")
set(WRAPS "${WRAPS},-wrap,CFE_SB_SubscribeLocal")
set(WRAPS "${WRAPS},-wrap,CFE_SB_Unsubscribe")
set(WRAPS "${WRAPS},-wrap,CFE_SB_UnsubscribeLocal")
set(WRAPS "${WRAPS},-wrap,CFE_SB_SendMsg")
set(WRAPS "${WRAPS},-wrap,CFE_SB_RcvMsg")
set(WRAPS "${WRAPS},-wrap,CFE_SB_ZeroCopySend")
set(WRAPS "${WRAPS},-wrap,CFE_SB_CreatePipe")
set(WRAPS "${WRAPS},-wrap,CFE_SB_Subscribe")
set(WRAPS "${WRAPS},-wrap,CFE_SB_RcvMsg")
set(WRAPS "${WRAPS},-wrap,socket")
set(WRAPS "${WRAPS},-wrap,htons")
set(WRAPS "${WRAPS},-wrap,inet_pton")
set(WRAPS "${WRAPS},-wrap,connect")
set(WRAPS "${WRAPS},-wrap,read")
set(WRAPS "${WRAPS},-wrap,sleep")
set(WRAPS "${WRAPS},-wrap,perror")
set(WRAPS "${WRAPS},-wrap,pthread_create")
set(WRAPS "${WRAPS},-wrap,connect_to_server")
set(WRAPS "${WRAPS},-wrap,CFE_SBN_Client_InitPipeTbl")
set(WRAPS "${WRAPS},-wrap,check_pthread_create_status")
set(WRAPS "${WRAPS},-wrap,send_heartbeat")
set(WRAPS "${WRAPS},-wrap,recv_msg")
set(WRAPS "${WRAPS},-wrap,CFE_SBN_CLIENT_ReadBytes")
set(WRAPS "${WRAPS},-wrap,pthread_mutex_lock")
set(WRAPS "${WRAPS},-wrap,pthread_mutex_unlock")
set(WRAPS "${WRAPS},-wrap,pthread_cond_signal")
set(WRAPS "${WRAPS},-wrap,CFE_SBN_Client_GetMsgId")
set(WRAPS "${WRAPS},-wrap,CFE_SBN_Client_GetPipeIdx")
set(WRAPS "${WRAPS},-wrap,pthread_cond_timedwait")
set(WRAPS "${WRAPS},-wrap,pthread_cond_wait")


# Get the files in the source subdirectory
set(source_files)
aux_source_directory("${PROJECT_SOURCE_DIR}/fsw/src" source_files)

set(source_file_names)
foreach(SOURCE_FILE ${source_files})
    get_filename_component(SOURCE_NAME "${SOURCE_FILE}" NAME_WE)
    add_library("${SOURCE_NAME}_object" OBJECT "${SOURCE_FILE}")
    list(APPEND source_file_names "${SOURCE_NAME}_object")
endforeach()
    
    
add_library("logger_stub" OBJECT "${PROJECT_SOURCE_DIR}/unit-test/stubs/sbn_client_logger_stubs.c")


# Get the files in the applications unit-test directory
set(app_ut_helper_files)
aux_source_directory("${PROJECT_SOURCE_DIR}/unit-test/helpers" app_ut_helper_files)
# Have a variable hold the helper library names so they can be referenced later
set(app_ut_helper_names)
# Build all the test helpers and save the object names

foreach(HELPER_FILE ${app_ut_helper_files})
    get_filename_component(HELPER_NAME "${HELPER_FILE}" NAME_WE)
    add_library("${HELPER_NAME}_object" OBJECT "${HELPER_FILE}")
    list(APPEND app_ut_helper_names "${HELPER_NAME}_object")
endforeach()
    
# Generate a dedicated "testrunner" executable that executes the tests for each FSW code unit
# Cycle through all the app's source files, there will be a test file for each
foreach(SRCFILE ${source_files})
    # Get the base sourcfile name as a module name without path or the extension, 
    # this will be used as the base name of the unit test file.
    get_filename_component(MODULENAME "${SRCFILE}" NAME_WE)
    
    set(source_file_names_copy ${source_file_names})
    
    set(stub_names)
    if(NOT MODULENAME MATCHES "sbn_client_logger")
        # aux_source_directory("${PROJECT_SOURCE_DIR}/unit-test/stubs" stub_dir)
        set(stub_names "logger_stub")
        remove(source_file_names_copy "sbn_client_logger_object")
    endif()
    
    remove(source_file_names_copy "${MODULENAME}_object")
    
    # Use the module name to find the testname, adding _tests to the end
    set(TESTNAME "${MODULENAME}_tests")
    
    # Make the testcase sourcefile name with path and extension
    set(TESTCASE_SOURCE_FILE    "${PROJECT_SOURCE_DIR}/unit-test/${TESTNAME}.c")
    
    set(TEST_SOURCE_OBJECT "ut_${TESTNAME}_object")
    
    # Compile the source unit under test as an OBJECT
    add_library(${TEST_SOURCE_OBJECT} OBJECT
        ${SRCFILE}
    )
    
    # Apply the UT_C_FLAGS to the units under test
    # This should enable coverage analysis on platforms that support this
    set_target_properties(${TEST_SOURCE_OBJECT} PROPERTIES
        COMPILE_FLAGS "${UT_C_FLAGS}")
        
    # Compile a test runner application, which contains the
    # actual coverage test code (test cases) and the unit under test
    # creates sample_app-sample_app-testrunner from coveragetest/coveragetest_sample_app.c
    add_executable(${TESTNAME}-testrunner
        ${TESTCASE_SOURCE_FILE}
        $<TARGET_OBJECTS:${TEST_SOURCE_OBJECT}>
    )
    
    # This also needs to be linked with WRAPS (for stubbing) and UT_C_FLAGS (for coverage)
    set_target_properties(${TESTNAME}-testrunner PROPERTIES 
    LINK_FLAGS "${WRAPS} ${UT_C_FLAGS}")
    
    # This is also linked with any other stub libraries needed,
    # as well as the UT assert framework
    # sbn_client needs referenced here, not sure how to remove it, maybe when there's stubs for sbn_client files?
    target_link_libraries(${TESTNAME}-testrunner
        ${source_file_names_copy}
        ${app_ut_helper_names}
        ${stub_names}
        ut_cfe-core_stubs
        ut_assert
    )
    
    # Add it to the set of tests to run as part of "make test"
    add_test(${TESTNAME} ${TESTNAME}-testrunner)
    
endforeach()

