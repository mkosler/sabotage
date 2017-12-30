cmake_minimum_required(VERSION 3.0)
project(sabotage)

set(CMAKE_MODULE_PATH "C:/SDL2-2.0.6")
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

find_package(SDL2 REQUIRED)
find_package(Lua REQUIRED)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "C:\\dirent\\include"
    ${SDL2_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${LUA_LIBRARIES})