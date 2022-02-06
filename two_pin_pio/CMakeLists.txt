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

if (TARGET tinyusb_device)
    add_executable(two_pin_pio)

    #by default the header is generated into the build dir
    pico_generate_pio_header(two_pin_pio ${CMAKE_CURRENT_LIST_DIR}/stepper.pio)
    # however, alternatively you can choose to generate it somewhere else (in this case in the source tree for check in)
    #pico_generate_pio_header(two_pin_pio ${CMAKE_CURRENT_LIST_DIR}/stepper.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR})

    target_sources(two_pin_pio PRIVATE main.c)

    target_link_libraries(two_pin_pio PRIVATE 
        pico_stdlib 
        hardware_pio
        hardware_dma
        hardware_irq
        )

    # enable usb output, disable uart output
    pico_enable_stdio_usb(two_pin_pio 1)
    pico_enable_stdio_uart(two_pin_pio 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(two_pin_pio)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()
