# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "C:/MicrocontrollerTools/Pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")

project(P05_SW_PicoIRQTest C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(sources main.c)

# Add executable. Default name is the project name, version 0.1
add_executable(P05_SW_PicoIRQTest ${sources})

pico_set_program_name(P05_SW_PicoIRQTest "P05_SW_PicoIRQTest")
pico_set_program_version(P05_SW_PicoIRQTest "0.1")

pico_enable_stdio_uart(P05_SW_PicoIRQTest 1)
pico_enable_stdio_usb(P05_SW_PicoIRQTest 0)

# Add the standard library to the build
target_link_libraries(P05_SW_PicoIRQTest pico_stdlib)

# Add any user requested libraries
target_link_libraries( P05_SW_PicoIRQTest
        hardware_timer
        hardware_clocks
        )

pico_add_extra_outputs(P05_SW_PicoIRQTest)



