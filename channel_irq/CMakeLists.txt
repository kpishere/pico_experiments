cmake_minimum_required(VERSION 3.19)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(../pico-sdk/pico_sdk_init.cmake)

project(dma_channel_irq C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(DAVID_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()



add_executable(${PROJECT_NAME}
        channel_irq.c
        )

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pio_serialiser.pio)

target_link_libraries(${PROJECT_NAME}
		pico_stdlib
        hardware_dma
        hardware_irq
        hardware_pio
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# add url via pico_set_program_url
#example_auto_set_url(${PROJECT_NAME})

#if (NOT PICO_NO_HARDWARE)
#    add_subdirectory(channel_irq)
#    add_subdirectory(control_blocks)
#    add_subdirectory(hello_dma)
#endif ()
