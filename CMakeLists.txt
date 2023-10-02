# CMakeLists.txt
cmake_minimum_required(VERSION 3.8)


project(myMalloc)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(exec ./src/main.c)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
