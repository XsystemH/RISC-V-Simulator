//
// Created by 13199 on 2024/7/29.
//

#ifndef RISC_V_RS_H
#define RISC_V_RS_H

#include "Instruction.h"
#include "LoopList.h"
#include "CDB.h"

const int RS_SIZE = 8;

struct RS_INFO {
  bool busy;
  Op_Type op;
  uint32_t vj, vk;
  unsigned int qj, qk;
  bool j, k;
  uint32_t imm;
  ADDRESS PC;
  unsigned int dest;
  uint32_t raw;

  bool ready() const {
    return j && k;
  }
  void debug() const {
    std::cerr << op_name[int(op)] << " | "
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

class RS {
public:
  RS_INFO sta[RS_SIZE];
  std::size_t size;
  RS_INFO sta_nxt[RS_SIZE];
  std::size_t size_nxt;

//  RS();
  void initialize();
  void flush();
  bool full() const;
  bool empty() const;
  void execute(CDB &cdb);
  void listen(CDB &cdb);
};


#endif //RISC_V_RS_H
