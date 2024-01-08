cmake_minimum_required(VERSION 2.6.4)
project(CFS_SBN_CLIENT C)

include_directories(fsw/public_inc)
include_directories(${sbn_MISSION_DIR}/fsw/src)
include_directories(${sbn_MISSION_DIR}/fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

set(APP_NAME sbn_client)

add_library(${APP_NAME} "SHARED" ${APP_SRC_FILES})
target_link_libraries(${APP_NAME} core_api)
target_sources(${APP_NAME} PRIVATE ${sbn_MISSION_DIR}/fsw/src/sbn_pack.c)
target_sources(${APP_NAME} PRIVATE ../../cfe/modules/msg/fsw/src/cfe_msg_ccsdspri.c)
target_sources(${APP_NAME} PRIVATE ../../cfe/modules/msg/fsw/src/cfe_msg_msgid_v1.c)

# Override the default behavior of attaching a "lib" prefix
set_target_properties(${APP_NAME} PROPERTIES PREFIX "")
# SBN?

# Create the install targets for this shared/modular app
if (APP_DYNAMIC_TARGET_LIST)
  cfs_app_do_install(${APP_NAME} ${APP_DYNAMIC_TARGET_LIST})
endif (APP_DYNAMIC_TARGET_LIST)

#if (ENABLE_UNIT_TESTS)
#  add_subdirectory(unit-test)
#endif (ENABLE_UNIT_TESTS)
