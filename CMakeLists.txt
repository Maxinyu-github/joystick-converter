cmake_minimum_required(VERSION 3.13)

# Set the board type
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(joystick_converter C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add subdirectories
add_subdirectory(firmware)
