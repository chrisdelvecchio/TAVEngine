cmake_minimum_required(VERSION 3.29.3)
project(game)

# Specify the C standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/external/glad/include)
include_directories(${PROJECT_SOURCE_DIR}/external/GLFW)
include_directories(${PROJECT_SOURCE_DIR}/external/cglm-master/include)
include_directories(${PROJECT_SOURCE_DIR}/external/nanovg-master/src)
include_directories(${PROJECT_SOURCE_DIR}/external/lib-list-master/include)
include_directories(${PROJECT_SOURCE_DIR}/external/lib-io-main/include)
include_directories(${PROJECT_SOURCE_DIR}/external/lib-map-main/include)

# Source files: Get all .c files in the in project
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.c 
${PROJECT_SOURCE_DIR}/external/glad/src/glad.c)

# Library files: Get all library files in the lib directory
add_subdirectory(${PROJECT_SOURCE_DIR}/external/cglm-master)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/nanovg-master)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/lib-list-master)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/lib-io-main)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/lib-map-main)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# If you have libraries to link, specify them here
target_link_libraries(${PROJECT_NAME} 
cglm 
nanovg 
list 
io 
map 
${PROJECT_SOURCE_DIR}/external/GLFW/lib/libglfw3.a)