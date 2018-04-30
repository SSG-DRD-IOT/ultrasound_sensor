cmake_minimum_required(VERSION 3.0)
project(Ultrasound)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/include/upm)
include_directories(/usr/include/mraa)
link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(UltrasoundUPM ultrasound_upm.c)

target_link_libraries(UltrasoundUPM upmc-hcsr04 mraa)