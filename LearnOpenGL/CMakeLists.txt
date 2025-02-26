﻿# CMakeList.txt : CMake project for LearnOpenGL, include source and define
# project specific logic here.

# Add source to this project's executable.
add_executable(LearnOpenGL "main.cpp" "shader_program.cpp" "shader_program.h" "fs_util.h" "fs_util.cpp" "camera.cpp" "camera.h"  "texture.h" "texture.cpp" "model.h" "model.cpp")

target_link_libraries(LearnOpenGL
    PRIVATE glad
    PRIVATE glfw
    PRIVATE stb_image
    PRIVATE glm
    PRIVATE assimp
)

configure_file(
    "${PROJECT_SOURCE_DIR}/LearnOpenGL/config.h.in"
    "${PROJECT_BINARY_DIR}/LearnOpenGL/config.h"
    @ONLY
)
target_include_directories(LearnOpenGL PRIVATE "${PROJECT_BINARY_DIR}/LearnOpenGL")

if(WIN32)
    set_target_properties(
        LearnOpenGL
        PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
                   LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )

    add_custom_command(TARGET LearnOpenGL PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/dlls" $<TARGET_FILE_DIR:LearnOpenGL>)

endif()

# TODO: Add tests and install targets if needed.
