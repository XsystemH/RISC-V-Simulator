//
// Created by 13199 on 2024/7/28.
//

#ifndef RISC_V_DECODER_H
#define RISC_V_DECODER_H

#include <cstdint>
#include <iostream>
#include "Instruction.h"
#include "Memory.h"
#include "ROB.h"
#include "RS.h"
#include "LSB.h"
#include "CDB.h"

class Decoder {
private:
  Memory *mem{};
public:
  ADDRESS PC;
  ADDRESS PC_nxt;
  Instruction instr{};
  bool stall;

  Decoder();

  void linkMemory(Memory *m);

  void flush();

  void fetch();

  void decode(ROB &rob, RS &rs, LSB &lsb, Registers & regs);
};

#endif //RISC_V_DECODER_H
