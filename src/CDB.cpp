//
// Created by 13199 on 2024/7/31.
//
#include "CDB.h"

void CDB::flush() {
  LBtoRS = RStoLB = toSB = RStoROB = LSBtoROB = clear = false;
  LBtoLB = RStoRS = false;
}