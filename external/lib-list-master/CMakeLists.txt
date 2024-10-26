cmake_minimum_required(VERSION 3.29.3)
project(liblist)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Create a static library
add_library(list STATIC ${PROJECT_SOURCE_DIR}/src/liblist.c)

# Specify the include directories for the library
target_include_directories(list PUBLIC ${PROJECT_SOURCE_DIR}/include)

# # Add the executable
# add_executable(liblist ${PROJECT_SOURCE_DIR}/src/main.c ${PROJECT_SOURCE_DIR}/src/liblist.c)