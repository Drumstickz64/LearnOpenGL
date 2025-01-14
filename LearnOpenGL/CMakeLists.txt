# CMakeList.txt : CMake project for LearnOpenGL, include source and define
# project specific logic here.

# Add source to this project's executable.
add_executable (LearnOpenGL "LearnOpenGL.cpp")

target_link_libraries(LearnOpenGL PRIVATE glad)
target_link_libraries(LearnOpenGL PRIVATE glfw)

# TODO: Add tests and install targets if needed.
