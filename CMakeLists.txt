# CMakeLists.txt
cmake_minimum_required(VERSION 3.12)


project(myMalloc)

include_directories(${PROJECT_SOURCE_DIR}/include)
set(CMAKE_BUILD_TYPE Debug)
add_compile_definitions(DEBUG)
add_executable(exec ./src/main.c ./src/utils.c ./src/malloc.c ./src/free.c)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
