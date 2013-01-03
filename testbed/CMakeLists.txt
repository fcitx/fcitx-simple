 include_directories("${PROJECT_SOURCE_DIR}/src"
"${PROJECT_BINARY_DIR}/src")

set(FCITX_TESTBED_SOURCES
    main.c
)

add_executable(fcitx-testbed ${FCITX_TESTBED_SOURCES})
install(TARGETS fcitx-testbed RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
target_link_libraries(fcitx-testbed fcitx-simple ${FCITX4_FCITX_LIBRARIES} ${FCITX4_FCITX_UTILS_LIBRARIES})
