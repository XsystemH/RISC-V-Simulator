//
// Created by 13199 on 2024/7/30.
//
#include "LSB.h"

void LSB::linkMemory(Memory *memory) {
  mem = memory;
  buffer_nxt.initialize();
}

void LSB::flush() {
//    std::cerr << "LSB" << std::endl;
//    std::cerr << "-----------------------------------" << std::endl;
//    std::cerr << "OP  |j |vj |qj |k |vk |qk |dest |imm" << std::endl;
  buffer.head = buffer_nxt.head;
  buffer.tail = buffer_nxt.tail;
  for (int i = 0; i < LSB_SIZE; i++) {
    buffer[i] = buffer_nxt[i];
//    if (buffer[i].busy) buffer[i].debug();
  }
//  std::cerr << "-----------------------------------" << std::endl;
}

bool LSB::full() {
  return buffer.full();
}

void LSB::execute(CDB &cdb) {
  if (buffer.empty()) return;
  if (!buffer[buffer.head].ready()) return;
//  std::cerr << "LSB executing: " << op_name[int(buffer[buffer.head].op)] << std::endl;
  if (buffer[buffer.head].op <= Op_Type::lhu) { // load
    ADDRESS address;
    unsigned int offset;
    uint32_t value;
    switch (buffer[buffer.head].op) {
      case Op_Type::lb :
        address = buffer[buffer.head].vj + buffer[buffer.head].imm;
        offset = address % 4;
        address = address - offset;
        value = mem->read(address);
        value = value >> (offset * 8);
        value &= 0xff;
        if (value >> 7) {
          value |= 0xffffff00;
        }
        break;
      case Op_Type::lh :
        address = buffer[buffer.head].vj + buffer[buffer.head].imm;
        offset = address % 4;
        address = address - offset;
        value = mem->read(address);
        value = value >> (offset * 8);
        value &= 0xffff;
        if (value >> 15) {
          value |= 0xffff0000;
        }
        break;
      case Op_Type::lw :
        address = buffer[buffer.head].vj + buffer[buffer.head].imm;
        value = mem->read(address);
        break;
      case Op_Type::lbu :
        address = buffer[buffer.head].vj + buffer[buffer.head].imm;
        offset = address % 4;
        address = address - offset;
        value = mem->read(address);
        value = value >> (offset * 8);
        value &= 0xff;
        break;
      case Op_Type::lhu :
        address = buffer[buffer.head].vj + buffer[buffer.head].imm;
        offset = address % 4;
        address = address - offset;
        value = mem->read(address);
        value = value >> (offset * 8);
        value &= 0xffff;
        break;
      default:
        std::cerr <<"Wrong decode" << std::endl;
        break;
    }
    // compute address
    unsigned int dest = buffer[buffer.head].dest;
//    rob.buffer_nxt[dest].state = State::execute;
//    rob.buffer_nxt[dest].value = value;
    cdb.LSBtoROB = true;
    cdb.LSB_destROB = dest;
    cdb.LSB_addrROB = address;
    cdb.LSB_valueROB = value;
    cdb.LBtoRS = true;
    cdb.LB_destRS = dest;
    cdb.LB_valueRS = value;
    cdb.LBtoLB = true;
    cdb.LB_destLB = dest;
    cdb.LB_valueLB = value;
    buffer_nxt.pop();
  }
  else { // store
    ADDRESS address;
    unsigned int offset;
    uint32_t value;
    switch (buffer[buffer.head].op) {
      case Op_Type::sb :
        address = buffer[buffer.head].vj + buffer[buffer.head].imm;
        offset = address % 4;
        address = address - offset;
        value = buffer[buffer.head].vk & 0xff; // low 8 bits
        value = value << (offset * 8);
        switch (offset) {
          case 0:
            value += mem->read(address) & 0xffffff00;
            break;
          case 1:
            value += mem->read(address) & 0xffff00ff;
            break;
          case 2:
            value += mem->read(address) & 0xff00ffff;
            break;
          case 3:
            value += mem->read(address) & 0x00ffffff;
            break;
          default:
            std::cerr << "Are you Kidding?";
        }
        break;
      case Op_Type::sh :
        address = buffer[buffer.head].vj + buffer[buffer.head].imm;
        offset = address % 4;
        address = address - offset;
        value = buffer[buffer.head].vk & 0xffff; // low 16 bits
        value = value << (offset * 8);
        switch (offset) {
          case 0:
            value += mem->read(address) & 0xffff0000;
            break;
          case 1:
            value += mem->read(address) & 0xff0000ff;
            break;
          case 2:
            value += mem->read(address) & 0x0000ffff;
            break;
          default:
            std::cerr << "Are you Kidding?";
        }
        break;
      case Op_Type::sw :
        address = buffer[buffer.head].vj + buffer[buffer.head].imm;
        value = buffer[buffer.head].vk;
        break;
      default:
        std::cerr <<"Wrong decode" << std::endl;
        break;
    }
    // compute address and value
    unsigned int dest = buffer[buffer.head].dest;
    cdb.LSBtoROB = true;
    cdb.LSB_destROB = dest;
    cdb.LSB_addrROB = address;
    cdb.LSB_valueROB = value;
  }
}

void LSB::listen(CDB &cdb) {
  if (cdb.RStoLB) {
    unsigned int dest = cdb.RS_destLB;
    uint32_t value = cdb.RS_valueLB;
    for (int i = 0; i < LSB_SIZE; i++) {
      if (!buffer_nxt[i].busy) continue;
      if (!buffer_nxt[i].j && buffer_nxt[i].qj == dest) {
        buffer_nxt[i].j = true;
        buffer_nxt[i].vj = value;
      } // get rs1 only
    }
  }
  if (cdb.LBtoLB) {
    unsigned int dest = cdb.LB_destLB;
    uint32_t value = cdb.LB_valueLB;
    for (int i = 0; i < LSB_SIZE; i++) {
      if (!buffer_nxt[i].busy) continue;
      if (!buffer_nxt[i].j && buffer_nxt[i].qj == dest) {
        buffer_nxt[i].j = true;
        buffer_nxt[i].vj = value;
      } // get rs1 only
    }
  }
  if (cdb.toSB) {
    unsigned int dest = cdb.destSB;
    uint32_t value = cdb.valueSB;
    for (int i = 0; i < LSB_SIZE; i++) { // broadcast
      if (!buffer_nxt[i].busy) continue;
      if (!buffer_nxt[i].k && buffer_nxt[i].qk == dest) {
        buffer_nxt[i].k = true;
        buffer_nxt[i].vk = value;
      }
    } // rs2
  }
  if (cdb.SB_Commit) {
    buffer_nxt.pop();
  }
  if (cdb.clear) {
    buffer_nxt.clear();
  }
}

bool LSB_INFO::ready() const {
  return j && k;
}
