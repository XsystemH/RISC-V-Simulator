//
// Created by 13199 on 2024/8/2.
//

#ifndef RISC_V_PREDICTOR_H
#define RISC_V_PREDICTOR_H

#include "Instruction.h"

class Predictor {
private:
  uint32_t state;
public:
  Predictor() {
    state = 2;
  }
  bool predict() const {
    return state >= 2;
  }
  void correction(bool ans) {
    if (ans) {
      if (state == 1) state = 0;
      if (state == 2) state = 3;
    } else {
      if (state <= 1) state++;
      if (state >= 2) state--;
    }
  }
};

static Predictor predictors[0x10000];

#endif //RISC_V_PREDICTOR_H
