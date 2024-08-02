//
// Created by 13199 on 2024/7/25.
//

#ifndef RISC_V_CPU_H
#define RISC_V_CPU_H
#include "Memory.h"
#include "Registers.h"
#include "Decoder.h"
#include "RS.h"
#include "LSB.h"
#include "ROB.h"
#include "CDB.h"

class CPU {
private:
  Memory *memory;
  Registers registers;
  Decoder decoder;
  RS rs;
  LSB lsb;
  ROB rob;
  CDB cdb;

public:
  CPU(Memory *mem) {
    memory = mem;
    decoder.linkMemory(mem);
    lsb.linkMemory(mem);
    registers.initialize();
    rs.initialize();
    rob.initialize(&registers, memory);
  }
  uint32_t run() {
    unsigned int clk = 0;
    while(true) {
      clk++;
//      std::cerr << "==============================" << std::endl;
//      std::cerr << "clk: " << clk - 1 << std::endl;
//      if (clk % 20 == 0) {
//        getchar();
//      }

      registers.flush();
      decoder.flush();
      rs.flush();
      lsb.flush();
      rob.flush();
      cdb.flush();

      decoder.decode(rob, rs, lsb, registers); // 发射
      rs.execute(cdb);
      lsb.execute(cdb); // 执行
      try {
        rob.commit(cdb, decoder.PC_nxt, decoder.stall); // 写回
      } catch (const std::runtime_error& e) {
        break;
      }

      rs.listen(cdb);
      lsb.listen(cdb);
      rob.listen(cdb);
      registers.listen(cdb);
    }
    return registers.reg[10] & 0xff;
  }
};

#endif //RISC_V_CPU_H
