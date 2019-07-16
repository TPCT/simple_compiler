cmake_minimum_required(VERSION 3.14)
project(ASMPROJ C)

set(CMAKE_C_STANDARD 11)

add_executable(ASMPROJ main.c Assembler_LEXER.c Assembler.h)