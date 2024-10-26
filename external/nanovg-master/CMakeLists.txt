cmake_minimum_required(VERSION 3.10)
project(nanovg)

set(CMAKE_C_STANDARD 11)

# Include directories
include_directories(src)

# NanoVG Static Library
add_library(nanovg STATIC src/nanovg.c)  # Adjust file if necessary

# Set compile definitions
target_compile_definitions(nanovg PRIVATE _CRT_SECURE_NO_WARNINGS)