cmake_minimum_required(VERSION 2.6.4)
project(CFS_SBN_CLIENT C)

include_directories(fsw/public_inc)
include_directories(${sbn_MISSION_DIR}/fsw/src)
include_directories(${sbn_MISSION_DIR}/fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

set(APP_NAME sbn_client)

add_library(${APP_NAME} "SHARED" ${APP_SRC_FILES})

# Override the default behavior of attaching a "lib" prefix
set_target_properties(${APP_NAME} PROPERTIES PREFIX "")
# SBN?

# Create the install targets for this shared/modular app
foreach(TGT ${APP_INSTALL_LIST})
  install(TARGETS ${APP_NAME} DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()

if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)
