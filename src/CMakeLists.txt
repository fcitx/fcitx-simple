include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(fcitx_simple_common_SOURCES simple-common.c)

add_library(fcitx-simple-common SHARED ${fcitx_simple_common_SOURCES})

set(fcitx_simple_frontend_SOURCES
    simple-frontend.c
)

set(fcitx_simple_module_SOURCES
    simple-module.c
)

set(fcitx_simple_ui_SOURCES
    simple-ui.c
)

fcitx_add_addon_full(simple-frontend SCAN
  SOURCES ${fcitx_simple_frontend_SOURCES}
  SCAN_NO_INSTALL
)
fcitx_add_addon_full(simple-module SCAN
  SOURCES ${fcitx_simple_module_SOURCES}
  LINK_LIBS fcitx-simple-common
  SCAN_NO_INSTALL
)
fcitx_add_addon_full(simple-ui SCAN
  SOURCES ${fcitx_simple_ui_SOURCES}
  SCAN_NO_INSTALL
)

set(fcitx_simple_SOURCES simple-api.cpp)

add_library(fcitx-simple SHARED ${fcitx_simple_SOURCES})

set_target_properties(fcitx-simple-common
  PROPERTIES VERSION 0.1
  SOVERSION 0
  COMPILE_FLAGS "-fvisibility=hidden"
  LINK_FLAGS "-Wl,--no-undefined")

set_target_properties(fcitx-simple
  PROPERTIES VERSION 0.1
  SOVERSION 0
  COMPILE_FLAGS "-fvisibility=hidden"
  LINK_FLAGS "-Wl,--no-undefined")

target_link_libraries(fcitx-simple-common ${FCITX4_FCITX_UTILS_LIBRARIES} ${PTHREAD_LIBRARIES})
target_link_libraries(fcitx-simple fcitx-simple-common ${FCITX4_FCITX_LIBRARIES} ${FCITX4_FCITX_UTILS_LIBRARIES} ${PTHREAD_LIBRARIES})

install(TARGETS fcitx-simple fcitx-simple-common DESTINATION ${LIB_INSTALL_DIR})
