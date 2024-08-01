cmake_minimum_required(VERSION 3.22)
project(RISC_V)

set(CMAKE_CXX_STANDARD 14)

add_executable(RISC_V
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
)
