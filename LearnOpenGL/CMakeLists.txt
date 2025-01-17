# CMakeList.txt : CMake project for LearnOpenGL, include source and define
# project specific logic here.

# Add source to this project's executable.
add_executable (LearnOpenGL "main.cpp" "shader_program.cpp" "shader_program.h" "fs_util.h" "fs_util.cpp" "stb_image.h")

target_link_libraries(LearnOpenGL PRIVATE glad)
target_link_libraries(LearnOpenGL PRIVATE glfw)

configure_file(
    "${PROJECT_SOURCE_DIR}/LearnOpenGL/config.h.in"
    "${PROJECT_BINARY_DIR}/LearnOpenGL/config.h"
)

target_include_directories(LearnOpenGL PRIVATE ${PROJECT_BINARY_DIR}/LearnOpenGL)

# TODO: Add tests and install targets if needed.
