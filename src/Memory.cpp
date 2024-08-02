//
// Created by 13199 on 2024/7/26.
//

#include "Memory.h"

Memory::Memory() {
  ADDRESS address = 0;
  std::string s;
  std::string temp;
  while(std::cin >> temp) {
    if (temp[0] == '@') {
      std::stringstream adr_hex;
      adr_hex << std::hex << temp.substr(1, 8);
      adr_hex >> address;
    } else {
      s += temp;
      if (s.length() == 8) {
        memory[address] = HexToNum(s);
        s.clear();
        address += 4;
      }
    }
  } // while
}

Memory::Memory(const std::string& path) {
  std::fstream file;
  file.open(path);
  ADDRESS address = 0;
  std::string s;
  std::string temp;
  while(file >> temp) {
    if (temp[0] == '@') {
      std::stringstream adr_hex;
      adr_hex << std::hex << temp.substr(1, 8);
      adr_hex >> address;
    } else {
      s += temp;
      if (s.length() == 8) {
        memory[address] = HexToNum(s);
        s.clear();
        address += 4;
      }
    }
  } // while
}

void Memory::debug() {
  for (auto i : memory) {
    std::cerr << std::hex << std::setw(4) << std::setfill('0') << i.first << " 0x" << std::hex << std::setw(8) << i.second << std::endl;
  }
}

uint32_t Memory::read(ADDRESS address) {
  if (memory.find(address) == memory.end()) {
    return 0;
  }
  return memory[address];
}

void Memory::write(ADDRESS address, uint32_t value) {
  memory[address] = value;
}

uint32_t HexToNum(const std::string& s) {
  // 将十六进制字符串分割为四个字节并转换为整数
  uint32_t bytes[4];
  for (int i = 0; i < 4; ++i) {
    bytes[i] = std::stoul(s.substr(i * 2, 2), nullptr, 16);
  }

  // 按小端序组合字节
  return (bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24));
}
