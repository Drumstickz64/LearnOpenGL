# Project Name
project(stb_image LANGUAGES C)

# Add the header-only library
add_library(stb_image INTERFACE)

target_include_directories(stb_image INTERFACE ".")