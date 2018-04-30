cmake_minimum_required(VERSION 3.0)
project(Ultrasound)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/include/upm)
include_directories(/usr/include/mraa)

add_library(mraa)

add_executable(UltrasoundUPM ultrasound_upm.c)