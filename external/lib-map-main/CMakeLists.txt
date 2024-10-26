cmake_minimum_required(VERSION 3.29.3)
project(libmap)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Create a static library
add_library(map STATIC ${PROJECT_SOURCE_DIR}/src/libmap.c)

# Specify the include directories for the library
target_include_directories(map PUBLIC ${PROJECT_SOURCE_DIR}/include)