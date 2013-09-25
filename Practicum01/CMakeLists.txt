cmake_minimum_required(VERSION 2.6)
project(MMTPracticum1)

if(WIN32)
  include_directories(src/win32)
endif(WIN32)

include_directories(src)

add_executable(tga_encode 
    src/main.cpp
    src/Tga.cpp
    src/Color.cpp
    src/BitStream.cpp)
