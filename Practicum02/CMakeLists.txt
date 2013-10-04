cmake_minimum_required(VERSION 2.6)
project(MMTPracticum2)

if(WIN32)
  include_directories(encoder/win32)
  include_directories(decoder/win32)
endif(WIN32)

include_directories(encoder)
include_directories(decoder)

add_executable(encoder 
    encoder/main.cpp
    encoder/ConfigReader.cpp
    encoder/BitStream.cpp)

add_executable(decoder 
    decoder/main.cpp
    decoder/ConfigReader.cpp
    decoder/BitStream.cpp)
