//
// Created by 13199 on 2024/7/29.
//

#ifndef RISC_V_REGISTERS_H
#define RISC_V_REGISTERS_H

#include <cstdint>

class Registers {
public:
  uint32_t reg[32]{};
  bool busy[32]{};
  unsigned int reorder[32]{};

  uint32_t reg_nxt[32]{};
  bool busy_nxt[32]{};
  unsigned int reorder_nxt[32]{};

  Registers() = default;
  void initialize();
  void flush();
  bool read(unsigned int k, uint32_t &result, unsigned int &reo);
  void write(unsigned int k, uint32_t value);
};


#endif //RISC_V_REGISTERS_H
