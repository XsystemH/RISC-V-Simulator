//
// Created by 13199 on 2024/7/30.
//
#include "Decoder.h"

// Define masks and shifts for the fields of the instruction
const uint32_t OPCODE_MASK = 0x7F;
const uint32_t RD_MASK = 0xF80;
const uint32_t FUNC3_MASK = 0x7000;
const uint32_t RS1_MASK = 0xF8000;
const uint32_t RS2_MASK = 0x1F00000;
const uint32_t FUNC7_MASK = 0xFE000000;

const uint32_t OPCODE_SHIFT = 0;
const uint32_t RD_SHIFT = 7;
const uint32_t FUNC3_SHIFT = 12;
const uint32_t RS1_SHIFT = 15;
const uint32_t RS2_SHIFT = 20;
const uint32_t FUNC7_SHIFT = 25;

const uint32_t IMM_20_MASK = 0x80000000;
const uint32_t IMM_10_1_MASK = 0x7FE00000;
const uint32_t IMM_11_MASK = 0x00100000;
const uint32_t IMM_19_12_MASK = 0x000FF000;

const uint32_t IMM_20_SHIFT = 31;
const uint32_t IMM_10_1_SHIFT = 21;
const uint32_t IMM_11_SHIFT = 20;
const uint32_t IMM_19_12_SHIFT = 12;

// Function to extract a field from the instruction
uint32_t extractField(uint32_t instruction, uint32_t mask, uint32_t shift) {
  return (instruction & mask) >> shift;
}

uint32_t extractJTypeImm(uint32_t instruction) {
  uint32_t imm20 = (instruction & IMM_20_MASK) >> IMM_20_SHIFT;
  uint32_t imm10_1 = (instruction & IMM_10_1_MASK) >> IMM_10_1_SHIFT;
  uint32_t imm11 = (instruction & IMM_11_MASK) >> IMM_11_SHIFT;
  uint32_t imm19_12 = (instruction & IMM_19_12_MASK) >> IMM_19_12_SHIFT;

  // Combine the parts and sign-extend the immediate value
  uint32_t imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
  // Sign-extend the 21-bit immediate value
  if (imm20) {
    imm |= 0xFFF00000;
  }
  return imm;
}

Decoder::Decoder() {
  PC = 0x00000000;
  PC_nxt = 0x00000000;
  stall = false;
}

void Decoder::linkMemory(Memory *m) {
  mem = m;
}

void Decoder::flush() {
  PC = PC_nxt;
}

void Decoder::fetch() {
  instr.raw = mem->read(PC);
  instr.opcode = extractField(instr.raw, OPCODE_MASK, OPCODE_SHIFT);
  instr.rd = extractField(instr.raw, RD_MASK, RD_SHIFT);
  instr.func3 = extractField(instr.raw, FUNC3_MASK, FUNC3_SHIFT);
  instr.rs1 = extractField(instr.raw, RS1_MASK, RS1_SHIFT);
  instr.rs2 = extractField(instr.raw, RS2_MASK, RS2_SHIFT);
  instr.func7 = extractField(instr.raw, FUNC7_MASK, FUNC7_SHIFT);
//  std::cerr << "PC at 0x" << std::hex << PC << std::endl;
}

void Decoder::decode(ROB &rob, RS &rs, LSB &lsb, Registers &regs) {
  fetch();
  if (rob.full() || stall) {
    PC_nxt = PC;
    return;
  }
//  std::cerr << "Decoding: 0x" << std::hex << instr.raw << std::endl;
  bool guess;
  switch (instr.opcode) {
    case 0b0110111: // lui -> Reg
      instr.op = Op_Type::lui;
      instr.imm = extractField(instr.raw, 0xfffff000, 0);
      PC_nxt = PC + 4;
      break;
    case 0b0010111: // auipc -> Reg
      instr.op = Op_Type::auipc;
      instr.imm = extractField(instr.raw, 0xfffff000, 0) + PC;
      PC_nxt = PC + 4;
      break;
    case 0b1101111: // jal -> Reg and PC
      instr.op = Op_Type::jal;
      instr.imm = PC + 4;
      PC_nxt = extractJTypeImm(instr.raw) + PC;
      break;
    case 0b1100111: // jalr -> Reg(R&W) and PC
      instr.op = Op_Type::jalr;
      instr.imm = PC + 4;
      instr.PC = (instr.func7 << 5) + instr.rs2;
      if (instr.func7 >> 6) {
        instr.PC |= 0xfffff000;
      }
      stall = true;
      break;
    case 0b1100011: { // B -> Reg(R) and PC
      instr.imm = ((instr.rd >> 1) << 1) + ((instr.func7 & 0x3f) << 5)
                  + ((instr.rd & 1) << 11) + ((instr.func7 >> 6) << 12);
      if (instr.func7 >> 6) {
        instr.imm |= 0xffffe000;
      }
      instr.PC = PC;
      guess = predictors[PC].predict();
      PC_nxt = guess ? (PC + instr.imm) : (PC + 4);
      switch (instr.func3) {
        case 0b000:
          instr.op = Op_Type::beq;
          break;
        case 0b001:
          instr.op = Op_Type::bne;
          break;
        case 0b100:
          instr.op = Op_Type::blt;
          break;
        case 0b101:
          instr.op = Op_Type::bge;
          break;
        case 0b110:
          instr.op = Op_Type::bltu;
          break;
        case 0b111:
          instr.op = Op_Type::bgeu;
          break;
      }
      break;
    }
    case 0b0000011: { // I -> Mem(R) Reg(W)
      instr.imm = (instr.func7 << 5) + instr.rs2;
      if (instr.func7 >> 6) {
        instr.imm |= 0xfffff000;
      }
      PC_nxt = PC + 4;
      switch (instr.func3) {
        case 0b000:
          instr.op = Op_Type::lb;
          break;
        case 0b001:
          instr.op = Op_Type::lh;
          break;
        case 0b010:
          instr.op = Op_Type::lw;
          break;
        case 0b100:
          instr.op = Op_Type::lbu;
          break;
        case 0b101:
          instr.op = Op_Type::lhu;
          break;
      }
      break;
    }
    case 0b0100011: { // S -> Reg(R) Mem(W)
      instr.imm = (instr.func7 << 5) + instr.rd;
      if (instr.func7 >> 6) {
        instr.imm |= 0xfffff000;
      }
      PC_nxt = PC + 4;
      switch (instr.func3) {
        case 0b000:
          instr.op = Op_Type::sb;
          break;
        case 0b001:
          instr.op = Op_Type::sh;
          break;
        case 0b010:
          instr.op = Op_Type::sw;
          break;
      }
      break;
    }
    case 0b0010011: {
      instr.imm = (instr.func7 << 5) + instr.rs2;
      if (instr.func7 >> 6) {
        instr.imm |= 0xfffff000;
      }
      PC_nxt = PC + 4;
      switch (instr.func3) {
        case 0b000:
          instr.op = Op_Type::addi;
          break;
        case 0b010:
          instr.op = Op_Type::slti;
          break;
        case 0b011:
          instr.op = Op_Type::sltiu;
          break;
        case 0b100:
          instr.op = Op_Type::xori;
          break;
        case 0b110:
          instr.op = Op_Type::ori;
          break;
        case 0b111:
          instr.op = Op_Type::andi;
          break;
        case 0b001:
          instr.op = Op_Type::slli;
          instr.imm = instr.rs2; // shamt
          break;
        case 0b101:
          if (!instr.func7) {
            instr.op = Op_Type::srli;
          } else {
            instr.op = Op_Type::srai;
          }
          instr.imm = instr.rs2; // shamt
          break;
      }
      if (instr.raw == 0x0ff00513) {
        instr.op = Op_Type::exit;
      }
      break;
    }
    case 0b0110011: {
      PC_nxt = PC + 4;
      switch (instr.func3) {
        case 0b000:
          if (!instr.func7) {
            instr.op = Op_Type::add;
          } else {
            instr.op = Op_Type::sub;
          }
          break;
        case 0b001:
          instr.op = Op_Type::sll;
          break;
        case 0b010:
          instr.op = Op_Type::slt;
          break;
        case 0b011:
          instr.op = Op_Type::sltu;
          break;
        case 0b100:
          instr.op = Op_Type::xor_;
          break;
        case 0b101:
          if (!instr.func7) {
            instr.op = Op_Type::srl;
          } else {
            instr.op = Op_Type::sra;
          }
          break;
        case 0b110:
          instr.op = Op_Type::or_;
          break;
        case 0b111:
          instr.op = Op_Type::and_;
          break;
      }
      break;
    }
    default:
      PC_nxt = PC + 4;
      instr.op = Op_Type::empty;
      std::cerr << "No Instruction" << std::endl;
      return;
  }
  if (Op_Type::lb <= instr.op && instr.op <= Op_Type::sw) {
    if (!lsb.full()) {
//      lsb.issue(instr, rob.issue(instr));
      ROB_INFO robInfo{};
      robInfo.busy = true;
      robInfo.instr = instr;
      robInfo.state = State::issue;
      switch (instr.op) {
        case Op_Type::beq:
        case Op_Type::bne:
        case Op_Type::blt:
        case Op_Type::bge:
        case Op_Type::bltu:
        case Op_Type::bgeu:
        case Op_Type::sb:
        case Op_Type::sh:
        case Op_Type::sw:
        case Op_Type::exit:
        case Op_Type::empty:
          robInfo.dest = 0;
          break;
        default:
          robInfo.dest = instr.rd;
      }
      unsigned int dest = rob.buffer_nxt.push(robInfo);
      if (robInfo.dest) { // x0 always 0
        regs.busy_nxt[robInfo.dest] = true;
        regs.reorder_nxt[robInfo.dest] = dest;
      }
      // rob.issue()
      LSB_INFO lsbInfo{};
      lsbInfo.busy = true;
      lsbInfo.op = instr.op;
      lsbInfo.dest = dest;
      lsbInfo.j = regs.read(instr.rs1, lsbInfo.vj, lsbInfo.qj);
      if (!lsbInfo.j && rob.buffer[lsbInfo.qj].state == State::execute) {
        lsbInfo.j = true;
        lsbInfo.vj = rob.buffer[lsbInfo.qj].value;
      }
      if (instr.op <= Op_Type::lhu) { // load
        lsbInfo.k = true;
      }
      else { // store
        lsbInfo.k = regs.read(instr.rs2, lsbInfo.vk, lsbInfo.qk);
      }
      lsbInfo.imm = instr.imm;
      lsb.buffer_nxt.push(lsbInfo);
      // lsb.issue()
      PC_nxt = PC + 4;
    }
    else {
      PC_nxt = PC;
    }
  }
  else {
    if (!rs.full()) {
//      rs.issue(instr, rob, rob.issue(instr));
      ROB_INFO robInfo{};
      robInfo.busy = true;
      robInfo.instr = instr;
      robInfo.state = State::issue;
      robInfo.guess = guess;
      switch (instr.op) {
        case Op_Type::beq:
        case Op_Type::bne:
        case Op_Type::blt:
        case Op_Type::bge:
        case Op_Type::bltu:
        case Op_Type::bgeu:
        case Op_Type::sb:
        case Op_Type::sh:
        case Op_Type::sw:
        case Op_Type::exit:
        case Op_Type::empty:
          robInfo.dest = 0;
          break;
        default:
          robInfo.dest = instr.rd;
      }
      unsigned int dest = rob.buffer_nxt.push(robInfo);
      if (robInfo.dest) { // x0 always 0
        regs.busy_nxt[robInfo.dest] = true;
        regs.reorder_nxt[robInfo.dest] = dest;
      }
      // rob.issue()
      RS_INFO rsInfo{};
      rsInfo.busy = true;
      rsInfo.op = instr.op;
      if (Op_Type::jalr <= rsInfo.op && rsInfo.op <= Op_Type::and_) {
        rsInfo.j = regs.read(instr.rs1, rsInfo.vj, rsInfo.qj);
        if (!rsInfo.j && rob.buffer[rsInfo.qj].state == State::execute) {
          rsInfo.j = true;
          rsInfo.vj = rob.buffer[rsInfo.qj].value;
        }
      } else {
        rsInfo.j = true;
      }
      if ((Op_Type::beq <= rsInfo.op && rsInfo.op <= Op_Type::bgeu)
        || (Op_Type::sb <= rsInfo.op && rsInfo.op <= Op_Type::sw)
        || (Op_Type::add <= rsInfo.op && rsInfo.op <= Op_Type::and_)) {
        rsInfo.k = regs.read(instr.rs2, rsInfo.vk, rsInfo.qk);
        if (!rsInfo.k && rob.buffer[rsInfo.qk].state == State::execute) {
          rsInfo.k = true;
          rsInfo.vk = rob.buffer[rsInfo.qk].value;
        }
      } else {
        rsInfo.k = true;
      }
      rsInfo.imm = instr.imm;
      rsInfo.PC = instr.PC;
      rsInfo.dest = dest;
      rsInfo.raw = instr.raw;
      for (int i = 0; i < RS_SIZE; i++) {
        if (!rs.sta[i].busy) {
          rs.sta_nxt[i] = rsInfo;
          break;
        }
      }
      rs.size_nxt = rs.size_nxt + 1;
      // rs.issue()
    }
    else {
      PC_nxt = PC;
    }
  }
}
