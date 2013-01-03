project(fcitx-simple)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Fcitx 4.2.7 REQUIRED)
find_package(Pthread REQUIRED)

_fcitx_add_uninstall_target()

set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fvisibility=hidden ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fvisibility=hidden ${CMAKE_CXX_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_MODULE_LINKER_FLAGS}")

set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share/locale")

add_definitions("-D_GNU_SOURCE")
if(NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR ${exec_prefix}/lib)
endif()

list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${LIB_INSTALL_DIR}" _isSystemPlatformLibDir)
list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${LIB_INSTALL_DIR}" _isSystemCxxLibDir)
if("${_isSystemPlatformLibDir}" STREQUAL "-1" AND "${_isSystemCxxLibDir}" STREQUAL "-1")
    set(CMAKE_SKIP_BUILD_RPATH  FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif("${_isSystemPlatformLibDir}" STREQUAL "-1" AND "${_isSystemCxxLibDir}" STREQUAL "-1")

include_directories(${FCITX4_FCITX_INCLUDE_DIRS})
include_directories(${FCITX4_FCITX_CONFIG_INCLUDE_DIRS})
include_directories(${FCITX4_FCITX_UTILS_INCLUDE_DIRS})
include_directories(${PTHREAD_INCLUDE_DIR})

link_directories(${FCITX4_FCITX_LIBRARY_DIRS})
link_directories(${FCITX4_FCITX_CONFIG_LIBRARY_DIRS})
link_directories(${FCITX4_FCITX_UTILS_LIBRARY_DIRS})

add_subdirectory(po)
add_subdirectory(src)
add_subdirectory(testbed)
