cmake_minimum_required(VERSION 2.6)
project(MMTPracticum2)

if(WIN32)
  include_directories(src/win32)
endif(WIN32)

include_directories(src)

add_executable(encoder 
    src/encoder.cpp
    src/ConfigReader.cpp
    src/Raw.cpp
    src/Enc.cpp
    src/RLE.cpp
    src/DCT.cpp
    src/Quantizer.cpp
    src/Linearizer.cpp
    src/BitStream.cpp)

add_executable(decoder 
    src/decoder.cpp
    src/ConfigReader.cpp
    src/Raw.cpp
    src/Enc.cpp
    src/RLE.cpp
    src/DCT.cpp
    src/Quantizer.cpp
    src/Linearizer.cpp
    src/BitStream.cpp)
