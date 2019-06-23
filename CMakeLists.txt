cmake_minimum_required(VERSION 3.7)
project(gatSS C)

# Turn verbose on
# set( CMAKE_VERBOSE_MAKEFILE on )

# Standard and compiler flags
set(CMAKE_C_STANDARD 99)
add_compile_options(-pedantic -Wall -O0)

# ASAN
#add_compile_options(-fsanitize=address)
#link_libraries("-fsanitize=address")
add_subdirectory(src)