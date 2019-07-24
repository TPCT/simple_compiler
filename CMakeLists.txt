cmake_minimum_required(VERSION 3.14)
project(ASMPROJ C)

set(CMAKE_C_STANDARD 11)

add_executable(ASMPROJ main.c lexer.c lexer.h Requirements.c Requirements.h parser.h parser.c tokens.h storage.h storage.c codegenerator.c codegenerator.h)
