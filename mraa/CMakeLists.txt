cmake_minimum_required(VERSION 3.0)
project(Ultrasound)

include_directories(/usr/include/upm)
include_directories(/usr/include/mraa)
link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(UltrasoundUPM ultrasound_mraa.c)

target_link_libraries(UltrasoundUPM mraa)