cmake_minimum_required(VERSION 3.19)
project(filesystem C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")
add_executable(filesystem memory.c include/memory.h util/form.c include/form.h file.c include/file.h include/mysys.h mysys.c util/tree.c include/tree.h shell.c include/shell.h user.c include/user.h)