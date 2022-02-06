cmake_minimum_required(VERSION 3.19)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)
set(CMAKE_BUILD_TYPE Debug)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(../pico-sdk/pico_sdk_init.cmake)

project(pico_experiments)


project(SIMPLE_DMA_fade C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_executable(${PROJECT_NAME}
	simple_pwm_dma_led_fade.c
)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
	pico_stdlib
	hardware_dma
	hardware_pwm
)
