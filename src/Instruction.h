//
// Created by 13199 on 2024/7/31.
//

#ifndef RISC_V_INSTRUCTION_H
#define RISC_V_INSTRUCTION_H

#include <cstdint>
#include <string>

typedef uint32_t ADDRESS;

enum class Op_Type {
  lui,
  auipc,
  jal,
  jalr,
  beq, bne, blt, bge, bltu, bgeu,
  lb, lh, lw, lbu, lhu,
  sb, sh, sw,
  addi, slti, sltiu, xori, ori, andi, slli, srli, srai,
  add, sub, sll, slt, sltu, xor_, srl, sra, or_, and_,
  exit, empty
};

static std::string op_name[] = {
    "lui",
    "auipc",
    "jal",
    "jalr",
    "beq ", "bne ", "blt ", "bge ", "bltu ", "bgeu ",
    "lb  ", "lh  ", "lw  ", "lbu ", "lhu ",
    "sb  ", "sh  ", "sw  ",
    "addi", "slti", "sltiu", "xori", "ori ", "andi", "slli", "srli", "srai",
    "add ", "sub ", "sll ", "slt ", "sltu", "xor_", "srl ", "sra ", "or_ ", "and_",
    "exit", "empty"
};

class Instruction {
public:
  Op_Type op;
  ADDRESS PC;
  uint32_t imm;
  uint32_t raw;
  uint32_t opcode;
  uint32_t rd;
  uint32_t func3;
  uint32_t rs1;
  uint32_t rs2;
  uint32_t func7;
  Instruction() = default;
};

#endif //RISC_V_INSTRUCTION_H
