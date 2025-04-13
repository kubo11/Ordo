set(ORDO_SOURCES
    child_process_manager.cpp
    command_manager.cpp
    main.cpp
    shortcut_manager.cpp
    window_manager_policy.cpp
)

add_executable(${PROJECT_NAME} ${ORDO_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC SYSTEM ${MIRAL_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)
target_link_libraries(${PROJECT_NAME} ${MIRAL_LDFLAGS} ${XKBCOMMON_LIBRARIES})