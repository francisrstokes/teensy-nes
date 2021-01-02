#include <stdio.h>
#include "CPU.h"
#include "Bus.h"

CPU6502::CPU6502() {}
CPU6502::~CPU6502() {}

// Addressing Modes
void CPU6502::AM_IMP() {
  addressingMode = Implicit;
};

void CPU6502::AM_ACC() {
  currentValue = a;
  addressingMode = Accumulator;
};

void CPU6502::AM_IMM() {
  currentAddress = pc++;
  currentValue = read(currentAddress);
  addressingMode = Immediate;
};

void CPU6502::AM_ZP() {
  currentAddress = read(pc++);
  currentValue = read(currentAddress);
  addressingMode = ZeroPage;
};

void CPU6502::AM_ZPX() {
  currentAddress = read(pc++) + x;
  currentValue = read(currentAddress);
  addressingMode = ZeroPageX;
};

void CPU6502::AM_ZPY() {
  currentAddress = read(pc++) + y;
  currentValue = read(currentAddress);
  addressingMode = ZeroPageY;
};

void CPU6502::AM_REL() {
  currentAddress = pc;
  currentValue = read(currentAddress);
  addressingMode = Relative;
};

void CPU6502::AM_ABS() {
  if (pc & 0xff == 0xff) pageBoundaryCrossed = 1;
  currentAddress = read(pc) | (read(pc + 1) << 8);
  currentValue = read(currentAddress);
  pc += 2;
  addressingMode = Absolute;
};

void CPU6502::AM_ABX() {
  if (pc & 0xff == 0xff) pageBoundaryCrossed = 1;
  currentAddress = (read(pc) | (read(pc + 1) << 8)) + x;
  currentValue = read(currentAddress);
  pc += 2;
  addressingMode = AbsoluteX;
};

void CPU6502::AM_ABY() {
  if (pc & 0xff == 0xff) pageBoundaryCrossed = 1;
  currentAddress = (read(pc) | (read(pc + 1) << 8)) + y;
  currentValue = read(currentAddress);
  pc += 2;
  addressingMode = AbsoluteY;
};

void CPU6502::AM_IND() {
  if (pc & 0xff == 0xff) pageBoundaryCrossed = 1;
  currentAddress = read(pc) | (read(pc + 1) << 8);
  currentValue = read(currentAddress);
  pc += 2;
  addressingMode = Indirect;
};

void CPU6502::AM_INX() {
  if (pc & 0xff == 0xff) pageBoundaryCrossed = 1;
  currentAddress = read((uint8_t)(read(pc) + x)) | (read((uint8_t)(read(pc) + x + 1)) << 8);
  currentValue = read(currentAddress);
  pc++;
  addressingMode = IndirectX;
};

void CPU6502::AM_INY() {
  if (pc & 0xff == 0xff) pageBoundaryCrossed = 1;
  currentAddress = (read(read(pc)) | (read(read(pc) + 1) << 8)) + y;
  currentValue = read(currentAddress);
  pc++;
  addressingMode = IndirectY;
};

// Instructions
void CPU6502::I_ADC() {
  uint8_t sum = a + currentValue + BIT_VALUE(p, CARRY_BIT);

  ASSIGN_BIT(p, OVERFLOW_BIT, ((~(a ^ currentValue)) & (a ^ sum) & 0x80) >> 7);
  ASSIGN_BIT(p, CARRY_BIT, sum < a);
  ASSIGN_BIT(p, NEGATIVE_BIT, (sum & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, sum == 0);

  a = sum;

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case AbsoluteY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case IndirectX: cycles += 6; break;
    case IndirectY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 5;
      break;
    }
    default: break;
  }
};

void CPU6502::I_AND() {
  a &= currentValue;

  ASSIGN_BIT(p, NEGATIVE_BIT, (a & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, a == 0);

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case AbsoluteY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case IndirectX: cycles += 6; break;
    case IndirectY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 5;
      break;
    }
    default: break;
  }
};

void CPU6502::I_ASL() {
  // Clear the status register and set the carry bit to bit 7
  p = currentValue & 0x80;

  uint8_t newValue = currentValue << 1;
  ASSIGN_BIT(p, NEGATIVE_BIT, (newValue & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, newValue == 0);

  if (addressingMode == Accumulator) {
    a = newValue;
  } else {
    write(currentAddress, newValue);
  }

  switch (addressingMode) {
    case Accumulator: cycles += 2; break;
    case ZeroPage: cycles += 5; break;
    case ZeroPageX: cycles += 6; break;
    case Absolute: cycles += 6; break;
    case AbsoluteX: cycles += 7; break;
    default: break;
  }
};

void CPU6502::I_BCC() {
  if (BIT_VALUE(p, CARRY_BIT) == 0) {
    uint16_t oldPc = pc;
    pc += (int8_t)currentValue;
    cycles += (pc & 0xff00 != oldPc & 0xff00) ? 2 : 1;
  }
  cycles += 2;
};

void CPU6502::I_BCS() {
  if (BIT_VALUE(p, CARRY_BIT)) {
    uint16_t oldPc = pc;
    pc += (int8_t)currentValue;
    cycles += (pc & 0xff00 != oldPc & 0xff00) ? 2 : 1;
  }
  cycles += 2;
};

void CPU6502::I_BEQ() {
  if (BIT_VALUE(p, ZERO_BIT) == 0) {
    uint16_t oldPc = pc;
    pc += (int8_t)currentValue;
    cycles += (pc & 0xff00 != oldPc & 0xff00) ? 2 : 1;
  }
  cycles += 2;
};

void CPU6502::I_BIT() {
  uint8_t res = a & currentValue;

  ASSIGN_BIT(p, NEGATIVE_BIT, (res & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, res == 0);
  ASSIGN_BIT(p, OVERFLOW_BIT, (res & 0x40) >> 6);

  switch (addressingMode) {
    case ZeroPage: cycles += 3; break;
    case Absolute: cycles += 4; break;
    default: break;
  }
};

void CPU6502::I_BMI() {
  if (BIT_VALUE(p, NEGATIVE_BIT)) {
    uint16_t oldPc = pc;
    pc += (int8_t)currentValue;
    cycles += (pc & 0xff00 != oldPc & 0xff00) ? 2 : 1;
  }
  cycles += 2;
};

void CPU6502::I_BNE() {
  if (BIT_VALUE(p, ZERO_BIT)) {
    uint16_t oldPc = pc;
    pc += (int8_t)currentValue;
    cycles += (pc & 0xff00 != oldPc & 0xff00) ? 2 : 1;
  }
  cycles += 2;
};

void CPU6502::I_BPL() {
  if (BIT_VALUE(p, NEGATIVE_BIT) == 0) {
    uint16_t oldPc = pc;
    pc += (int8_t)currentValue;
    cycles += (pc & 0xff00 != oldPc & 0xff00) ? 2 : 1;
  }
  cycles += 2;
};

void CPU6502::I_BRK() {
  SET_BIT(p, BREAK_BIT);
  cycles += 7;
};

void CPU6502::I_BVC() {
  if (BIT_VALUE(p, OVERFLOW_BIT) == 0) {
    uint16_t oldPc = pc;
    pc += (int8_t)currentValue;
    cycles += (pc & 0xff00 != oldPc & 0xff00) ? 2 : 1;
  }
  cycles += 2;
};

void CPU6502::I_BVS() {
  if (BIT_VALUE(p, OVERFLOW_BIT)) {
    uint16_t oldPc = pc;
    pc += (int8_t)currentValue;
    cycles += (pc & 0xff00 != oldPc & 0xff00) ? 2 : 1;
  }
  cycles += 2;
};

void CPU6502::I_CLC() {
  CLEAR_BIT(p, CARRY_BIT);
  cycles += 2;
};

void CPU6502::I_CLD() {
  CLEAR_BIT(p, DECIMAL_BIT);
  cycles += 2;
};

void CPU6502::I_CLI() {
  CLEAR_BIT(p, INTERRUPT_BIT);
  cycles += 2;
};

void CPU6502::I_CLV() {
  CLEAR_BIT(p, OVERFLOW_BIT);
  cycles += 2;
};

void CPU6502::I_CMP() {
  ASSIGN_BIT(p, CARRY_BIT, a > currentValue);
  ASSIGN_BIT(p, ZERO_BIT, a == currentValue);
  ASSIGN_BIT(p, NEGATIVE_BIT, (currentValue & 0x80) >> 7);

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case AbsoluteY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case IndirectX: cycles += 6; break;
    case IndirectY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 5;
      break;
    }
    default: break;
  }
};

void CPU6502::I_CPX() {
  ASSIGN_BIT(p, CARRY_BIT, x > currentValue);
  ASSIGN_BIT(p, ZERO_BIT, x == currentValue);
  ASSIGN_BIT(p, NEGATIVE_BIT, (currentValue & 0x80) >> 7);

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case Absolute: cycles += 4; break;
    default: break;
  }
};

void CPU6502::I_CPY() {
  ASSIGN_BIT(p, CARRY_BIT, y > currentValue);
  ASSIGN_BIT(p, ZERO_BIT, y == currentValue);
  ASSIGN_BIT(p, NEGATIVE_BIT, (currentValue & 0x80) >> 7);

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case Absolute: cycles += 4; break;
    default: break;
  }
};

void CPU6502::I_DEC() {
  uint8_t res = currentValue - 1;

  ASSIGN_BIT(p, ZERO_BIT, res == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (res & 0x80) >> 7);

  write(currentAddress, res);

  switch (addressingMode) {
    case ZeroPage: cycles += 5; break;
    case ZeroPageX: cycles += 6; break;
    case Absolute: cycles += 6; break;
    case AbsoluteX: cycles += 7; break;
    default: break;
  }
};

void CPU6502::I_DEX() {
  x -= 1;
  cycles += 2;
  ASSIGN_BIT(p, ZERO_BIT, x == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (x & 0x80) >> 7);
};

void CPU6502::I_DEY() {
  y -= 1;
  cycles += 2;
  ASSIGN_BIT(p, ZERO_BIT, y == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (y & 0x80) >> 7);
};

void CPU6502::I_EOR() {
  a ^= currentValue;

  ASSIGN_BIT(p, ZERO_BIT, a == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (a & 0x80) >> 7);

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case AbsoluteY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case IndirectX: cycles += 6; break;
    case IndirectY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 5;
      break;
    }
    default: break;
  }
};

void CPU6502::I_INC() {
  uint8_t res = currentValue + 1;

  ASSIGN_BIT(p, ZERO_BIT, res == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (res & 0x80) >> 7);

  write(currentAddress, res);

  switch (addressingMode) {
    case ZeroPage: cycles += 5; break;
    case ZeroPageX: cycles += 6; break;
    case Absolute: cycles += 6; break;
    case AbsoluteX: cycles += 7; break;
    default: break;
  }
};

void CPU6502::I_INX() {
  x += 1;
  cycles += 2;

  ASSIGN_BIT(p, ZERO_BIT, x == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (x & 0x80) >> 7);
};

void CPU6502::I_INY() {
  y += 1;
  cycles += 2;

  ASSIGN_BIT(p, ZERO_BIT, y == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (y & 0x80) >> 7);
};

void CPU6502::I_JMP() {
  pc = currentAddress;
  switch (addressingMode) {
    case Absolute: cycles += 3; break;
    case Indirect: cycles += 5; break;
    default: break;
  }
};

void CPU6502::I_JSR() {
  push(pc - 1);
  pc = currentAddress;
  cycles += 6;
};

void CPU6502::I_LDA() {
  a = currentValue;

  ASSIGN_BIT(p, ZERO_BIT, a == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (a & 0x80) >> 7);

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case AbsoluteY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case IndirectX: cycles += 6; break;
    case IndirectY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 5;
      break;
    }
    default: break;
  }
};

void CPU6502::I_LDX() {
  x = currentValue;

  ASSIGN_BIT(p, ZERO_BIT, x == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (x & 0x80) >> 7);

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageY: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    default: break;
  }
};

void CPU6502::I_LDY() {
  y = currentValue;

  ASSIGN_BIT(p, ZERO_BIT, y == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (y & 0x80) >> 7);

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    default: break;
  }
};

void CPU6502::I_LSR() {
  uint8_t res = currentValue >> 1;
  ASSIGN_BIT(p, CARRY_BIT, BIT_VALUE(currentValue, 0));

  ASSIGN_BIT(p, ZERO_BIT, res == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (res & 0x80) >> 7);

  if (addressingMode == Accumulator) {
    a = res;
  } else {
    write(currentAddress, res);
  }

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: cycles += 4; break;
    default: break;
  }
};

void CPU6502::I_NOP() {
  cycles += 2;
};

void CPU6502::I_ORA() {
  a |= currentValue;

  ASSIGN_BIT(p, ZERO_BIT, a == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (a & 0x80) >> 7);

   switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case AbsoluteY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case IndirectX: cycles += 6; break;
    case IndirectY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 5;
      break;
    }
    default: break;
  }
};

void CPU6502::I_PHA() {
  push(a);
  cycles += 3;
};

void CPU6502::I_PHP() {
  push(p);
  cycles += 3;
};

void CPU6502::I_PLA() {
  a = pop();
  cycles += 4;
};

void CPU6502::I_PLP() {
  p = pop();
  cycles += 4;
};

void CPU6502::I_ROL() {
  uint8_t res = (currentValue << 1) | BIT_VALUE(p, CARRY_BIT);

  ASSIGN_BIT(p, CARRY_BIT, BIT_VALUE(currentValue, 7));
  ASSIGN_BIT(p, ZERO_BIT, res == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (res & 0x80) >> 7);

  if (addressingMode == Accumulator) {
    a = res;
  } else {
    write(currentAddress, res);
  }

  switch (addressingMode) {
    case Accumulator: cycles += 2; break;
    case ZeroPage: cycles += 5; break;
    case ZeroPageX: cycles += 6; break;
    case Absolute: cycles += 6; break;
    case AbsoluteX: cycles += 7; break;
    default: break;
  }
};

void CPU6502::I_ROR() {
  uint8_t res = (BIT_VALUE(p, CARRY_BIT) << 7) | (currentValue >> 1);

  ASSIGN_BIT(p, CARRY_BIT, BIT_VALUE(currentValue, 0));
  ASSIGN_BIT(p, ZERO_BIT, res == 0);
  ASSIGN_BIT(p, NEGATIVE_BIT, (res & 0x80) >> 7);

  if (addressingMode == Accumulator) {
    a = res;
  } else {
    write(currentAddress, res);
  }

  switch (addressingMode) {
    case Accumulator: cycles += 2; break;
    case ZeroPage: cycles += 5; break;
    case ZeroPageX: cycles += 6; break;
    case Absolute: cycles += 6; break;
    case AbsoluteX: cycles += 7; break;
    default: break;
  }
};

void CPU6502::I_RTI() {
  p = pop();
  pc = pop();
  cycles += 6;
};

void CPU6502::I_RTS() {
  p = pop();
  pc = pop();
  cycles += 6;
};

void CPU6502::I_SBC() {
  uint8_t sum = a - currentValue - BIT_VALUE(p, CARRY_BIT);

  ASSIGN_BIT(p, OVERFLOW_BIT, ((~(a ^ currentValue)) & (a ^ sum) & 0x80) >> 7);
  ASSIGN_BIT(p, CARRY_BIT, sum > a);
  ASSIGN_BIT(p, NEGATIVE_BIT, (sum & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, sum == 0);

  a = sum;

  switch (addressingMode) {
    case Immediate: cycles += 2; break;
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case AbsoluteY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 4;
      break;
    }
    case IndirectX: cycles += 6; break;
    case IndirectY: {
      if (pageBoundaryCrossed) cycles++;
      cycles += 5;
      break;
    }
    default: break;
  }
};

void CPU6502::I_SEC() {
  SET_BIT(p, CARRY_BIT);
  cycles += 2;
};

void CPU6502::I_SED() {
  SET_BIT(p, DECIMAL_BIT);
  cycles += 2;
};

void CPU6502::I_SEI() {
  SET_BIT(p, INTERRUPT_BIT);
  cycles += 2;
};

void CPU6502::I_STA() {
  write(currentAddress, a);

  switch (addressingMode) {
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    case AbsoluteX: cycles += 5; break;
    case AbsoluteY: cycles += 5; break;
    case IndirectX: cycles += 6; break;
    case IndirectY: cycles += 6; break;
    default: break;
  }
};

void CPU6502::I_STX() {
  write(currentAddress, x);

  switch (addressingMode) {
    case ZeroPage: cycles += 3; break;
    case ZeroPageY: cycles += 4; break;
    case Absolute: cycles += 4; break;
    default: break;
  }
};

void CPU6502::I_STY() {
  write(currentAddress, x);

  switch (addressingMode) {
    case ZeroPage: cycles += 3; break;
    case ZeroPageX: cycles += 4; break;
    case Absolute: cycles += 4; break;
    default: break;
  }
};

void CPU6502::I_TAX() {
  x = a;
  ASSIGN_BIT(p, NEGATIVE_BIT, (x & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, x == 0);
  cycles += 2;
};

void CPU6502::I_TAY() {
  y = a;
  ASSIGN_BIT(p, NEGATIVE_BIT, (y & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, y == 0);
  cycles += 2;
};

void CPU6502::I_TSX() {
  x = sp;
  ASSIGN_BIT(p, NEGATIVE_BIT, (x & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, x == 0);
  cycles += 2;
};

void CPU6502::I_TXA() {
  a = x;
  ASSIGN_BIT(p, NEGATIVE_BIT, (a & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, a == 0);
  cycles += 2;
};

void CPU6502::I_TXS() {
  sp = x;
  cycles += 2;
};

void CPU6502::I_TYA() {
  a = y;
  ASSIGN_BIT(p, NEGATIVE_BIT, (a & 0x80) >> 7);
  ASSIGN_BIT(p, ZERO_BIT, a == 0);
  cycles += 2;
};

// High Level CPU Control
void CPU6502::step() {
  cycles = 0;
  pageBoundaryCrossed = 0;
  uint8_t opcode = read(pc++);

  switch (opcode) {
    // ADC
    case 0x69: { AM_IMM(); return I_ADC(); }
    case 0x65: { AM_ZP(); return I_ADC(); }
    case 0x75: { AM_ZPX(); return I_ADC(); }
    case 0x6D: { AM_ABS(); return I_ADC(); }
    case 0x7D: { AM_ABX(); return I_ADC(); }
    case 0x79: { AM_ABY(); return I_ADC(); }
    case 0x61: { AM_INX(); return I_ADC(); }
    case 0x71: { AM_INY(); return I_ADC(); }

    // AND
    case 0x29: { AM_IMM(); return I_AND(); }
    case 0x25: { AM_ZP(); return I_AND(); }
    case 0x35: { AM_ZPX(); return I_AND(); }
    case 0x2D: { AM_ABS(); return I_AND(); }
    case 0x3D: { AM_ABX(); return I_AND(); }
    case 0x39: { AM_ABY(); return I_AND(); }
    case 0x21: { AM_INX(); return I_AND(); }
    case 0x31: { AM_INY(); return I_AND(); }

    // ASL
    case 0x0A: { AM_ACC(); return I_ASL(); }
    case 0x06: { AM_ZP(); return I_ASL(); }
    case 0x16: { AM_ZPX(); return I_ASL(); }
    case 0x0E: { AM_ABS(); return I_ASL(); }
    case 0x1E: { AM_ABX(); return I_ASL(); }

    // BCC
    case 0x90: { AM_REL(); return I_BCC(); }

    // BCS
    case 0xB0: { AM_REL(); return I_BCS(); }

    // BIT
    case 0x24: { AM_ZP(); return I_BIT(); }
    case 0x2C: { AM_ABS(); return I_BIT(); }

    // BMI
    case 0x30: { AM_REL(); return I_BMI(); }

    // BNE
    case 0xD0: { AM_REL(); return I_BNE(); }

    // BPL
    case 0x10: { AM_REL(); return I_BPL(); }

    // BRK
    case 0x00: { AM_IMP(); return I_BRK(); }

    // BVC
    case 0x50: { AM_REL(); return I_BVC(); }

    // BVS
    case 0x70: { AM_REL(); return I_BVS(); }

    // CLC
    case 0x18: { AM_IMP(); return I_CLC(); }

    // CLI
    case 0x58: { AM_IMP(); return I_CLI(); }

    // CLV
    case 0xB8: { AM_IMP(); return I_CLV(); }

    // CMP
    case 0xC9: { AM_IMM(); return I_CMP(); }
    case 0xC5: { AM_ZP(); return I_CMP(); }
    case 0xD5: { AM_ZPX(); return I_CMP(); }
    case 0xCD: { AM_ABS(); return I_CMP(); }
    case 0xDD: { AM_ABX(); return I_CMP(); }
    case 0xD9: { AM_ABY(); return I_CMP(); }
    case 0xC1: { AM_INX(); return I_CMP(); }
    case 0xD1: { AM_INY(); return I_CMP(); }

    // CPX
    case 0xE0: { AM_IMM(); return I_CPX(); }
    case 0xE4: { AM_ZP(); return I_CPX(); }
    case 0xEC: { AM_ABS(); return I_CPX(); }

    // CPY
    case 0xC0: { AM_IMM(); return I_CPY(); }
    case 0xC4: { AM_ZP(); return I_CPY(); }
    case 0xCC: { AM_ABS(); return I_CPY(); }

    // DEC
    case 0xC6: { AM_ZP(); return I_DEC(); }
    case 0xD6: { AM_ZPX(); return I_DEC(); }
    case 0xCE: { AM_ABS(); return I_DEC(); }
    case 0xDE: { AM_ABX(); return I_DEC(); }

    // DEX
    case 0xCA: { AM_IMP(); return I_DEX(); }

    // DEY
    case 0x88: { AM_IMP(); return I_DEY(); }

    // EOR
    case 0x49: { AM_IMM(); return I_EOR(); }
    case 0x45: { AM_ZP(); return I_EOR(); }
    case 0x55: { AM_ZPX(); return I_EOR(); }
    case 0x4D: { AM_ABS(); return I_EOR(); }
    case 0x5D: { AM_ABX(); return I_EOR(); }
    case 0x59: { AM_ABY(); return I_EOR(); }
    case 0x41: { AM_INX(); return I_EOR(); }
    case 0x51: { AM_INY(); return I_EOR(); }

    // INC
    case 0xE6: { AM_ZP(); return I_INC(); }
    case 0xF6: { AM_ZPX(); return I_INC(); }
    case 0xEE: { AM_ABS(); return I_INC(); }
    case 0xFE: { AM_ABX(); return I_INC(); }

    // INX
    case 0xE8: { AM_IMP(); return I_INX(); }

    // INY
    case 0xC8: { AM_IMP(); return I_INY(); }

    // JMP
    case 0x4C: { AM_ABS(); return I_JMP(); }
    case 0x6C: { AM_IND(); return I_JMP(); }

    // JSR
    case 0x20: { AM_ABS(); return I_JSR(); }

    // LDA
    case 0xA9: { AM_IMM(); return I_LDA(); }
    case 0xA5: { AM_ZP(); return I_LDA(); }
    case 0xB5: { AM_ZPX(); return I_LDA(); }
    case 0xAD: { AM_ABS(); return I_LDA(); }
    case 0xBD: { AM_ABX(); return I_LDA(); }
    case 0xB9: { AM_ABY(); return I_LDA(); }
    case 0xA1: { AM_INX(); return I_LDA(); }
    case 0xB1: { AM_INY(); return I_LDA(); }

    // LDX
    case 0xA2: { AM_IMM(); return I_LDX(); }
    case 0xA6: { AM_ZP(); return I_LDX(); }
    case 0xB6: { AM_ZPY(); return I_LDX(); }
    case 0xAE: { AM_ABS(); return I_LDX(); }
    case 0xBE: { AM_ABY(); return I_LDX(); }

    // LDY
    case 0xA0: { AM_IMM(); return I_LDY(); }
    case 0xA4: { AM_ZP(); return I_LDY(); }
    case 0xB4: { AM_ZPX(); return I_LDY(); }
    case 0xAC: { AM_ABS(); return I_LDY(); }
    case 0xBC: { AM_ABX(); return I_LDY(); }

    // LSR
    case 0x4A: { AM_ACC(); return I_LSR(); }
    case 0x46: { AM_ZP(); return I_LSR(); }
    case 0x56: { AM_ZPX(); return I_LSR(); }
    case 0x4E: { AM_ABS(); return I_LSR(); }
    case 0x5E: { AM_ABX(); return I_LSR(); }

    // NOP
    case 0xEA: { AM_IMP(); return I_NOP(); }

    // ORA
    case 0x09: { AM_IMM(); return I_ORA(); }
    case 0x05: { AM_ZP(); return I_ORA(); }
    case 0x15: { AM_ZPX(); return I_ORA(); }
    case 0x0D: { AM_ABS(); return I_ORA(); }
    case 0x1D: { AM_ABX(); return I_ORA(); }
    case 0x19: { AM_ABY(); return I_ORA(); }
    case 0x01: { AM_INX(); return I_ORA(); }
    case 0x11: { AM_INY(); return I_ORA(); }

    // PHA
    case 0x48: { AM_IMP(); return I_PHA(); }

    // PHP
    case 0x08: { AM_IMP(); return I_PHP(); }

    // PLA
    case 0x68: { AM_IMP(); return I_PLA(); }

    // PLP
    case 0x28: { AM_IMP(); return I_PLP(); }

    // ROL
    case 0x2A: { AM_ACC(); return I_ROL(); }
    case 0x26: { AM_ZP(); return I_ROL(); }
    case 0x36: { AM_ZPX(); return I_ROL(); }
    case 0x2E: { AM_ABS(); return I_ROL(); }
    case 0x3E: { AM_ABX(); return I_ROL(); }

    // ROR
    case 0x6A: { AM_ACC(); return I_ROR(); }
    case 0x66: { AM_ZP(); return I_ROR(); }
    case 0x76: { AM_ZPX(); return I_ROR(); }
    case 0x6E: { AM_ABS(); return I_ROR(); }
    case 0x7E: { AM_ABX(); return I_ROR(); }

    // RTI
    case 0x40: { AM_IMP(); return I_RTI(); }

    // RTS
    case 0x60: { AM_IMP(); return I_RTS(); }

    // SBC
    case 0xE9: { AM_IMM(); return I_SBC(); }
    case 0xE5: { AM_ZP(); return I_SBC(); }
    case 0xF5: { AM_ZPX(); return I_SBC(); }
    case 0xED: { AM_ABS(); return I_SBC(); }
    case 0xFD: { AM_ABX(); return I_SBC(); }
    case 0xF9: { AM_ABY(); return I_SBC(); }
    case 0xE1: { AM_INX(); return I_SBC(); }
    case 0xF1: { AM_INY(); return I_SBC(); }

    // SEC
    case 0x38: { AM_IMP(); return I_SEC(); }

    // SED
    case 0xF8: { AM_IMP(); return I_SED(); }

    // SEI
    case 0x78: { AM_IMP(); return I_SEI(); }

    // STA
    case 0x85: { AM_ZP(); return I_STA(); }
    case 0x95: { AM_ZPX(); return I_STA(); }
    case 0x8D: { AM_ABS(); return I_STA(); }
    case 0x9D: { AM_ABX(); return I_STA(); }
    case 0x99: { AM_ABY(); return I_STA(); }
    case 0x81: { AM_INX(); return I_STA(); }
    case 0x91: { AM_INY(); return I_STA(); }

    // STX
    case 0x86: { AM_ZP(); return I_STX(); }
    case 0x96: { AM_ZPY(); return I_STX(); }
    case 0x8E: { AM_ABS(); return I_STX(); }

    // STY
    case 0x84: { AM_ZP(); return I_STY(); }
    case 0x94: { AM_ZPX(); return I_STY(); }
    case 0x8C: { AM_ABS(); return I_STY(); }

    // TAX
    case 0xAA: { AM_IMP(); return I_TAX(); }

    // TAY
    case 0xA8: { AM_IMP(); return I_TAY(); }

    // TSX
    case 0xBA: { AM_IMP(); return I_TSX(); }

    // TXA
    case 0x8A: { AM_IMP(); return I_TXA(); }

    // TXS
    case 0x9A: { AM_IMP(); return I_TXS(); }

    // TYA
    case 0x98: { AM_IMP(); return I_TYA(); }

    default: {
      break;
    }
  }
};

void CPU6502::connectToBus(Bus* b) {
  bus = b;
};

void CPU6502::write(uint16_t address, uint8_t value) {
  (*bus).write(address, value);
};

uint8_t CPU6502::read(uint16_t address) {
  return (*bus).read(address);
};

void CPU6502::push(uint8_t value) {
  (*bus).write((1 << 8) | sp--, value);
};

uint8_t CPU6502::pop() {
  return (*bus).read(((1 << 8) | sp++) + 1);
};

void CPU6502::printCPUState() {
  printf("A=%hhx X=%hhx Y=%hhx\n", a, x, y);
  printf(
    "N=%hhx V=%hhx B=%hhx D=%hhx I=%hhx Z=%hhx C=%hhx\n",
    BIT_VALUE(p, NEGATIVE_BIT),
    BIT_VALUE(p, OVERFLOW_BIT),
    BIT_VALUE(p, BREAK_BIT),
    BIT_VALUE(p, DECIMAL_BIT),
    BIT_VALUE(p, INTERRUPT_BIT),
    BIT_VALUE(p, ZERO_BIT),
    BIT_VALUE(p, CARRY_BIT)
  );
}
