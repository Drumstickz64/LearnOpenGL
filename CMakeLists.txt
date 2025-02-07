# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.15)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(LearnOpenGL)

# Include sub-projects.
add_subdirectory(deps/glad)
add_subdirectory(deps/glfw)
add_subdirectory(deps/stb_image)

option(DEBUG "enable debug printing and opengl debug messages" OFF)
option(WIREFRAME_MODE OFF)

if(${DEBUG})
	set(DEBUG_CPP_VALUE "true")
else()
	set(DEBUG_CPP_VALUE "false")
endif()

if(${WIREFRAME})
	set(WIREFRAME_CPP_VALUE "true")
else()
	set(WIREFRAME_CPP_VALUE "false")
endif()
add_subdirectory(LearnOpenGL)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "deps/glm")

find_package(assimp CONFIG)

if (NOT assimp_FOUND)
  add_subdirectory("deps/assimp")
endif()

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LearnOpenGL PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(LearnOpenGL PRIVATE /W3)
else()
  target_compile_options(LearnOpenGL PRIVATE -Wall -Wextra)
endif()
