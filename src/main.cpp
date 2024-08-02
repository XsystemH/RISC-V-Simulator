//
// Created by 13199 on 2024/7/25.
//
#include <iostream>
#include "Memory.h"
#include "CPU.h"

int main() {
//  std::freopen("log.out", "w", stderr);
//  std::cerr << "Hello! RISC-V!" << std::endl;
//  std::string path = "../sample/sample.data";
//  std::string path = "../testcases/bulgarian.data";
//  Memory mem(path);
  Memory mem;
//  mem.debug();
  CPU cpu(&mem);
  std::cout << cpu.run();
  return 0;
}