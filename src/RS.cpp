//
// Created by 13199 on 2024/7/30.
//
#include <iostream>
#include "RS.h"

void RS::initialize() {
  size_nxt = 0;
  for (auto & i : sta_nxt) {
    i = RS_INFO{};
  }
}

void RS::flush() {
//  std::cerr << "RS" << std::endl;
//  std::cerr << "------------------------------------" << std::endl;
//  std::cerr << "OP  |j |vj |qj |k |vk |qk |dest |imm" << std::endl;
  for (int i = 0; i < RS_SIZE; i++) {
    sta[i] = sta_nxt[i];
//    if (sta[i].busy) sta[i].debug();
  }
  size = size_nxt;
//  std::cerr << "------------------------------------" << std::endl;
}

bool RS::full() const {
  return size == RS_SIZE;
}

bool RS::empty() const {
  return size == 0;
}

void RS::execute(CDB &cdb) {
  if (empty()) return;
  bool flag = false;
  RS_INFO toALU{};
  for (int i = 0; i < RS_SIZE; i++) {
    if (!sta[i].busy) continue;
    if (sta[i].ready()) {
      toALU = sta[i];
      sta_nxt[i].busy = false;
      size_nxt = size_nxt - 1;
      flag = true;
      break;
    }
  }
  if (!flag) return; // nothing in RS is ready
//  std::cerr << "RS executing: " << op_name[int(toALU.op)] << " " << toALU.raw << std::endl;
  unsigned int dest = toALU.dest;
  uint32_t value = 0;
  ADDRESS new_PC;
  switch (toALU.op) {
    case Op_Type::lui:
    case Op_Type::auipc:
    case Op_Type::jal: // jumping to PC when decoding PC
      value = toALU.imm; // PC + 4
      break;
    case Op_Type::jalr:
      value = toALU.imm; // PC + 4
      new_PC = toALU.PC + toALU.vj; // imm(SEXT) + rs1
      break;
    case Op_Type::beq:
      value = (toALU.vj == toALU.vk);
      new_PC = value ? toALU.PC : (toALU.PC - toALU.imm + 4);
      break;
    case Op_Type::bne:
      value = (toALU.vj != toALU.vk);
      new_PC = value ? toALU.PC : (toALU.PC - toALU.imm + 4);
      break;
    case Op_Type::blt:
      value = (static_cast<int32_t>(toALU.vj) < static_cast<int32_t>(toALU.vk));
      new_PC = value ? toALU.PC : (toALU.PC - toALU.imm + 4);
      break;
    case Op_Type::bge:
      value = (static_cast<int32_t>(toALU.vj) >= static_cast<int32_t>(toALU.vk));
      new_PC = value ? toALU.PC : (toALU.PC - toALU.imm + 4);
      break;
    case Op_Type::bltu:
      value = (toALU.vj < toALU.vk);
      new_PC = value ? toALU.PC : (toALU.PC - toALU.imm + 4);
      break;
    case Op_Type::bgeu:
      value = (toALU.vj >= toALU.vk);
      new_PC = value ? toALU.PC : (toALU.PC - toALU.imm + 4);
      break;
    case Op_Type::addi:
      value = toALU.vj + toALU.imm;
      break;
    case Op_Type::slti:
      value = (static_cast<int32_t>(toALU.vj) < static_cast<int32_t>(toALU.imm));
      break;
    case Op_Type::sltiu:
      value = (toALU.vj < toALU.imm);
      break;
    case Op_Type::xori:
      value = toALU.vj ^ toALU.imm;
      break;
    case Op_Type::ori:
      value = toALU.vj | toALU.imm;
      break;
    case Op_Type::andi:
      value = toALU.vj & toALU.imm;
      break;
    case Op_Type::slli:
      value = toALU.vj << toALU.imm;
      break;
    case Op_Type::srli:
      value = toALU.vj >> toALU.imm;
      break;
    case Op_Type::srai:
      value = static_cast<int32_t>(toALU.vj) >> toALU.imm;
      break;
    case Op_Type::add:
      value = toALU.vj + toALU.vk;
      break;
    case Op_Type::sub:
      value = static_cast<int32_t>(toALU.vj) - static_cast<int32_t>(toALU.vk);
      break;
    case Op_Type::sll:
      value = toALU.vj << (toALU.vk & 0x1f);
      break;
    case Op_Type::slt:
      value = (static_cast<int32_t>(toALU.vj) < static_cast<int32_t>(toALU.vk));
      break;
    case Op_Type::sltu:
      value = toALU.vj < toALU.vk;
      break;
    case Op_Type::xor_:
      value = toALU.vj ^ toALU.vk;
      break;
    case Op_Type::srl:
      value = toALU.vj >> (toALU.vk & 0x1f);
      break;
    case Op_Type::sra:
      value = static_cast<int32_t>(toALU.vj) >> (toALU.vk & 0x1f);
      break;
    case Op_Type::or_:
      value = toALU.vj | toALU.vk;
      break;
    case Op_Type::and_:
      value = toALU.vj & toALU.vk;
      break;
    case Op_Type::exit:
    case Op_Type::empty:
      break;
    default:
      std::cerr << "Wrong Decode to RS: " << int(toALU.op) << std::endl;
  }
//  rob.buffer_nxt[dest].state = State::execute;
//  rob.buffer_nxt[dest].value = value;
  cdb.RStoROB = true;
  cdb.RS_destROB = dest;
  cdb.RS_valueROB = value;
  cdb.PC = new_PC;
  cdb.RStoLB = true;
  cdb.RS_destLB = dest;
  cdb.RS_valueLB = value;
  cdb.RStoRS = true;
  cdb.RS_destRS = dest;
  cdb.RS_valueRS = value;
}

void RS::listen(CDB &cdb) {
  if (cdb.LBtoRS) {
    unsigned int dest = cdb.LB_destRS;
    uint32_t value = cdb.LB_valueRS;
    for (int i = 0; i < RS_SIZE; i++) {
      if (!sta_nxt[i].busy) continue;
      if (!sta_nxt[i].j && sta_nxt[i].qj == dest) {
        sta_nxt[i].j = true;
        sta_nxt[i].vj = value;
      }
      if (!sta_nxt[i].k && sta_nxt[i].qk == dest) {
        sta_nxt[i].k = true;
        sta_nxt[i].vk = value;
      }
    }
  }
  if (cdb.RStoRS) {
    unsigned int dest = cdb.RS_destRS;
    uint32_t value = cdb.RS_valueRS;
    for (int i = 0; i < RS_SIZE; i++) {
      if (!sta_nxt[i].busy) continue;
      if (!sta_nxt[i].j && sta_nxt[i].qj == dest) {
        sta_nxt[i].j = true;
        sta_nxt[i].vj = value;
      }
      if (!sta_nxt[i].k && sta_nxt[i].qk == dest) {
        sta_nxt[i].k = true;
        sta_nxt[i].vk = value;
      }
    }
  }
  if (cdb.clear) {
    size_nxt = 0;
    for(auto & i : sta_nxt) {
      i.busy = false;
    }
  }
}
