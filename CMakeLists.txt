cmake_minimum_required(VERSION 3.21)
project(raycast C)

set(CMAKE_C_STANDARD 99)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE APP_HEADERS CONFIGURE_DEPENDS "${SRC_DIR}/*.h")
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${SRC_DIR}/*.c")

add_executable(raycast ${APP_HEADERS} ${APP_SOURCES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# brew install sdl2
find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

find_package(upng REQUIRED)
target_link_libraries(${PROJECT_NAME} upng)

add_custom_target(copy_images
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/images/
        ${CMAKE_BINARY_DIR}/images/
        COMMENT "Copying images..."
        VERBATIM)
add_dependencies(${PROJECT_NAME} copy_images)