//
// Created by 13199 on 2024/7/29.
//

#ifndef RISC_V_ROB_H
#define RISC_V_ROB_H

#include "Instruction.h"
#include "Registers.h"
#include "LoopList.h"
#include "CDB.h"

const int BUFFER_SIZE = 16;

enum class State {
  issue,
  execute,
  commit,
};

struct ROB_INFO {
  bool busy;
  Instruction instr;
  State state;
  unsigned int dest;
  uint32_t value;
  ADDRESS PC;

  void debug() const {
    std::cerr << op_name[int(instr.op)] << " | "
              << (state == State::issue ? "issue " : "execute") << "|  "
              << dest << "    |"
              << value << "    | "
              << PC << std::endl;
  }
};

class ROB {
private:
  Registers *regs;
public:
  LoopList<ROB_INFO, BUFFER_SIZE> buffer;
  LoopList<ROB_INFO, BUFFER_SIZE> buffer_nxt;

public:
  void initialize(Registers *registers);
  void flush();
  bool full();
  void commit(CDB &cdb, ADDRESS &PC_nxt, bool &stall);
  void listen(CDB &cdb);
};


#endif //RISC_V_ROB_H
