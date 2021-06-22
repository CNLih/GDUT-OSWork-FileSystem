cmake_minimum_required(VERSION 3.19)
project(filesystem C)

set(CMAKE_C_STANDARD 99)

add_executable(filesystem memory.c include/memory.h util/form.c include/form.h file.c include/file.h include/mysys.h mysys.c)