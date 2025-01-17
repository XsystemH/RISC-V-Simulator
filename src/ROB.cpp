//
// Created by 13199 on 2024/7/30.
//
#include <stdexcept>
#include <iostream>
#include "ROB.h"

void ROB::initialize(Registers *registers, Memory *memory) {
  regs = registers;
  mem = memory;
  buffer_nxt.initialize();
}

void ROB::flush() {
//  std::cerr << "ROB" << std::endl;
//  std::cerr << "------------------------------------" << std::endl;
//  std::cerr << "id | OP  | state | dest | value | PC" << std::endl;
  buffer.head = buffer_nxt.head;
  buffer.tail = buffer_nxt.tail;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = buffer_nxt[i];
//    if (buffer[i].busy) {
//      std::cerr << "#" << i << " ";
//      buffer[i].debug();
//    }
  }
//  std::cerr << "------------------------------" << std::endl;
}

bool ROB::full() {
  return buffer.full();
}

void ROB::commit(CDB &cdb, ADDRESS &PC_nxt, bool &stall) {
  if (buffer.empty()) return;
  if (buffer[buffer.head].state == State::execute) {
//    std::cerr << "Commit: " << std::hex << buffer[buffer.head].instr.raw << " " << op_name[int(buffer[buffer.head].instr.op)] << " " << buffer[buffer.head].value << std::endl;
    bool flag = false;
    if (buffer[buffer.head].instr.op == Op_Type::exit) throw std::runtime_error("HALT");
    if (buffer[buffer.head].instr.op == Op_Type::jalr) {
      stall = false;
      PC_nxt = buffer[buffer.head].PC;
    }
    if (Op_Type::beq <= buffer[buffer.head].instr.op && buffer[buffer.head].instr.op <= Op_Type::bgeu) {
      if (buffer[buffer.head].value != buffer[buffer.head].guess) {
//        std::cerr << "guess failed" << std::endl;
        ROB_INFO it = buffer[buffer.head];
        PC_nxt = it.value ? (it.instr.PC + it.instr.imm) : (it.instr.PC + 4);
        predictors[it.instr.PC].correction(false);
        flag = true;
      } else {
        predictors[buffer[buffer.head].instr.PC].correction(true);
//        std::cerr << "guess succeed" << std::endl;
      }
    }
    if (buffer[buffer.head].instr.op == Op_Type::jalr) {
      PC_nxt = buffer[buffer.head].PC;
    }
    if (flag) {
      buffer_nxt.clear();
      cdb.clear = true;
      if (stall) stall = false;
    } else {
      unsigned int rob_id;
      rob_id = buffer_nxt.head;

      if (rob_id == regs->reorder_nxt[buffer_nxt[rob_id].dest]) {
        regs->busy_nxt[buffer_nxt[rob_id].dest] = false;
      }
      regs->write(buffer_nxt[rob_id].dest, buffer_nxt[rob_id].value);
      // regs
      if (Op_Type::sb <= buffer_nxt[rob_id].instr.op && buffer_nxt[rob_id].instr.op <= Op_Type::sw) {
        mem->write(buffer_nxt[rob_id].PC, buffer_nxt[rob_id].value);
        cdb.SB_Commit = true;
      }
      // mem

      cdb.toSB = true;
      cdb.destSB = rob_id;
      cdb.valueSB = buffer_nxt[buffer_nxt.head].value;
      buffer_nxt.pop();
    }
  }
}

void ROB::listen(CDB &cdb) {
  if (cdb.RStoROB) {
    buffer_nxt[cdb.RS_destROB].state = State::execute;
    buffer_nxt[cdb.RS_destROB].value = cdb.RS_valueROB;
    buffer_nxt[cdb.RS_destROB].PC = cdb.PC;
  }
  if (cdb.LSBtoROB) {
    buffer_nxt[cdb.LSB_destROB].state = State::execute;
    buffer_nxt[cdb.LSB_destROB].PC = cdb.LSB_addrROB;
    buffer_nxt[cdb.LSB_destROB].value = cdb.LSB_valueROB;
  }
}
