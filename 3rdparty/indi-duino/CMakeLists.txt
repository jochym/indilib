cmake_minimum_required(VERSION 2.4.7)
PROJECT(indi_duino CXX C)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
set(FIRMATA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libfirmata/src/")
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
 
find_package(INDI REQUIRED)

configure_file (
  "${CMAKE_SOURCE_DIR}/duconfig.h.in"
  "${CMAKE_SOURCE_DIR}/duconfig.h"
  )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${INDI_INCLUDE_DIR})
include_directories(${FIRMATA_INCLUDE_DIR})


################# libfirmata ############################
set (firmata_VERSION_MAJOR 1)
set (firmata_VERSION_MINOR 0)
set (firmata_SRCS
        ${CMAKE_SOURCE_DIR}/libfirmata/src/firmata.cpp
        ${CMAKE_SOURCE_DIR}/libfirmata/src/arduino.cpp
)
add_library(firmata ${firmata_SRCS})
add_executable(blink_pin ${CMAKE_SOURCE_DIR}/libfirmata/examples/blink_pin.cpp)
target_link_libraries (blink_pin firmata)
add_executable(read_digital_pin ${CMAKE_SOURCE_DIR}/libfirmata/examples/read_digital_pin.cpp)
target_link_libraries (read_digital_pin firmata)
add_executable(read_analog_pin ${CMAKE_SOURCE_DIR}/libfirmata/examples/read_analog_pin.cpp)
target_link_libraries (read_analog_pin firmata)
add_executable(ask_digital_pin ${CMAKE_SOURCE_DIR}/libfirmata/examples/ask_digital_pin.cpp)
target_link_libraries (ask_digital_pin firmata)
add_executable(ask_analog_pin ${CMAKE_SOURCE_DIR}/libfirmata/examples/ask_analog_pin.cpp)
target_link_libraries (ask_analog_pin firmata)

##################### indi arduino #####################
set(indiduino_SRCS
   ${CMAKE_SOURCE_DIR}/indiduino.cpp
   )

add_executable(indi_duino ${indiduino_SRCS})
target_link_libraries(indi_duino ${INDI_DRIVER_LIBRARIES} firmata)

install(TARGETS indi_duino RUNTIME DESTINATION bin)

################### DEVICES XML  #####################
add_subdirectory(devices)
