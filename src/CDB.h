//
// Created by 13199 on 2024/7/31.
//

#ifndef RISC_V_CDB_H
#define RISC_V_CDB_H

#include <cstdint>
#include "Instruction.h"

class CDB {
public:
  bool LBtoRS;
  unsigned int LB_destRS;
  uint32_t LB_valueRS;
  bool RStoRS;
  unsigned int RS_destRS;
  uint32_t RS_valueRS;
  bool LBtoLB;
  unsigned int LB_destLB;
  uint32_t LB_valueLB;
  bool RStoLB;
  unsigned int RS_destLB;
  uint32_t RS_valueLB;
  bool toSB;
  unsigned int destSB;
  uint32_t valueSB;
  bool RStoROB;
  unsigned int RS_destROB;
  uint32_t RS_valueROB;
  ADDRESS PC;
  bool LSBtoROB;
  unsigned int LSB_destROB;
  uint32_t LSB_valueROB;
  ADDRESS LSB_addrROB;
  bool clear;

  void flush();
};


#endif //RISC_V_CDB_H
