cmake_minimum_required(VERSION 3.21)
project(raycast C)

set(CMAKE_C_STANDARD 99)

add_executable(raycast main.c constants.h)

# brew install sdl2
find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})