//
// Created by 13199 on 2024/7/30.
//
#include <iostream>
#include "Registers.h"

void Registers::initialize() {
  for (int i = 0; i < 32; i++) {
    reg[i] = 0;
    busy[i] = false;
    reorder[i] = 0;
    reg_nxt[i] = 0;
    busy_nxt[i] = false;
    reorder_nxt[i] = 0;
  }
}

void Registers::flush() {
  for (int i = 0; i < 32; i++) {
    reg[i] = reg_nxt[i];
    busy[i] = busy_nxt[i];
    reorder[i] = reorder_nxt[i];
  }
}

void Registers::listen(CDB &cdb) {
  if (cdb.clear) {
    for (bool & i : busy_nxt) {
      i = false;
    }
  }
}

bool Registers::read(unsigned int k, uint32_t &result, unsigned int &reo) {
  if (k == 0) {
    result = 0;
    return true;
  }
  if (busy[k]) {
    reo = reorder[k];
    return false;
  }
  result = reg[k];
  return true;
}

void Registers::write(unsigned int k, uint32_t value) {
  if (k == 0) return; // x0 always 0
//  if (k == 10) std::cout << "Write value " << value << " to reg: " << k << std::endl;
//  std::cerr << "Write value " << value << " to reg: " << k << std::endl;
  reg_nxt[k] = value;
}
