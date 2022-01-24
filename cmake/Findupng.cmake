include(FetchContent)

message(NOTICE "Fetch upng")
FetchContent_Declare(
        upng
        GIT_REPOSITORY https://github.com/elanthis/upng.git
)
FetchContent_MakeAvailable(upng)

file(GLOB UPNG_SOURCE
        ${upng_SOURCE_DIR}/upng.c
        ${upng_SOURCE_DIR}/upng.h
)

add_library(
        upng STATIC
        ${UPNG_SOURCE}
)

target_include_directories(
        upng PUBLIC
        ${upng_SOURCE_DIR}
)
