
cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)
set(CMAKE_BUILD_TYPE Debug)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(../pico-sdk/pico_sdk_init.cmake)

project(pico_experiments)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(hello_spi)

target_sources(hello_spi PRIVATE hello.c board_picksix.c)

target_link_libraries(hello_spi PRIVATE
        pico_stdlib
        hardware_spi
        )

pico_add_extra_outputs(hello_spi)
