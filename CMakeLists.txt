cmake_minimum_required(VERSION 3.0)
project(Ultrasound)

set(CMAKE_C_STANDARD 11)

add_subdirectory(legacy_sysfs)
add_subdirectory(upm)
add_subdirectory(mraa)