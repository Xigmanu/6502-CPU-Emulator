cmake_minimum_required(VERSION 3.28.1)
project(6502_emu)

set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} ./source/cpu.c ./source/main.c ./source/test.c)
