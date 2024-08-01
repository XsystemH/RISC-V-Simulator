//
// Created by 13199 on 2024/7/29.
//

#ifndef RISC_V_LSB_H
#define RISC_V_LSB_H

#include "Memory.h"
#include "Registers.h"
#include "LoopList.h"
#include "CDB.h"

const int LSB_SIZE = 8;

struct LSB_INFO {
  bool busy = false;
  Op_Type op = Op_Type::empty;
  uint32_t vj = 0, vk = 0;
  unsigned int qj = 0, qk = 0;
  bool j = false, k = false;
  uint32_t imm = 0;
  unsigned int dest = 0;

  bool ready() const;
  void debug() const {
    std::cerr << op_name[int(op)] << "| "
              << j  << " | "
              << vj << " | "
              << qj << " | "
              << k  << " | "
              << vk << " | "
              << qk << " | "
              << dest << " | "
              << imm << std::endl;
  }
};

class LSB {
private:
  Registers *regs;
  Memory *mem;
public:
  LoopList<LSB_INFO, LSB_SIZE> buffer;
  LoopList<LSB_INFO, LSB_SIZE> buffer_nxt;

public:
  void linkMemory(Memory *memory);
  void flush();
  bool full();
  void execute(CDB &cdb);
  void listen(CDB &cdb);
};


#endif //RISC_V_LSB_H
