//
// Created by 13199 on 2024/7/26.
//

#ifndef RISC_V_MEMORY_H
#define RISC_V_MEMORY_H

#include <cstdint>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Instruction.h"

uint32_t HexToNum(const std::string& s);

class Memory {
private:
  std::map<ADDRESS, uint32_t> memory;
  std::map<ADDRESS, uint32_t> instructions;

public:
  Memory();
  explicit Memory(const std::string& path);
  uint32_t getInstr(ADDRESS address);
  uint32_t read(ADDRESS address);
  void write(ADDRESS address, uint32_t value);

  void debug();
};

#endif //RISC_V_MEMORY_H
