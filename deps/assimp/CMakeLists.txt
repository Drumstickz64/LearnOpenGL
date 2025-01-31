project(assimp)

add_library(assimp INTERFACE)

target_include_directories(assimp INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(assimp INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp.lib")