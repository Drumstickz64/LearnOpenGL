# CMakeList.txt : CMake project for LearnOpenGL, include source and define
# project specific logic here.
#

add_subdirectory (deps/glfw)

# Add source to this project's executable.
add_executable (LearnOpenGL "LearnOpenGL.cpp"  "deps/glad/src/glad.c")

target_include_directories(LearnOpenGL PRIVATE "deps/glad/include")
target_link_libraries(LearnOpenGL PRIVATE glfw)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LearnOpenGL PROPERTY CXX_STANDARD 20)
endif()

if (MSVC)
  target_compile_options(LearnOpenGL PRIVATE /W3)
else()
  target_compile_options(LearnOpenGL PRIVATE -Wall -Wextra)
endif()

# TODO: Add tests and install targets if needed.
