
cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)
set(CMAKE_BUILD_TYPE Debug)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(../pico-sdk/pico_sdk_init.cmake)

project(pico_experiments C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(${CMAKE_SOURCE_DIR})

add_executable(hello_asm
        HelloWorld.S
)

pico_enable_stdio_usb(hello_asm 1)
pico_enable_stdio_uart(hello_asm 0)
pico_add_extra_outputs(hello_asm)
target_link_libraries(hello_asm pico_stdlib)

