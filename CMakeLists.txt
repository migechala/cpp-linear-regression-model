cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_COMPILER "/usr/bin/gcc")

project(linear_regression)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/MichaEngine/cmake/")

get_filename_component(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets" REALPATH)

add_executable(${PROJECT_NAME} source/main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES OSX_ARCHITECTURES arm64)

# Include the source directory
add_subdirectory(source)

# Link the 'source' library with the main executable
target_link_libraries(${PROJECT_NAME} source)
