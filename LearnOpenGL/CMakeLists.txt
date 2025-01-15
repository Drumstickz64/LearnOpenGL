# CMakeList.txt : CMake project for LearnOpenGL, include source and define
# project specific logic here.

# Add source to this project's executable.
add_executable (LearnOpenGL "main.cpp" "shader_program.cpp" "shader_program.h" "fs.h" "fs.cpp")

target_link_libraries(LearnOpenGL PRIVATE glad)
target_link_libraries(LearnOpenGL PRIVATE glfw)

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

target_include_directories(LearnOpenGL PRIVATE ${PROJECT_BINARY_DIR})

# TODO: Add tests and install targets if needed.
