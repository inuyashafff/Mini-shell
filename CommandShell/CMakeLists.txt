cmake_minimum_required(VERSION 3.8)
project(CommandShell)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES main.cpp shell.h shell.cpp parser.cpp parser.h command.cpp command.h )
add_executable(CommandShell ${SOURCE_FILES})