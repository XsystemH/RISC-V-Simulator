cmake_minimum_required(VERSION 3.22)
project(RISC_V)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable(code
        src/main.cpp
        src/CPU.h
        src/Memory.cpp
        src/Decoder.cpp
        src/ROB.cpp
        src/RS.cpp
        src/LSB.cpp
        src/Registers.cpp
        src/CDB.cpp
        src/LoopList.h
        src/Instruction.h
        src/Predictor.h
)
