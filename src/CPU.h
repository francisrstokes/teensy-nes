#pragma once

#include <stdint.h>

#define NEGATIVE_BIT   7
#define OVERFLOW_BIT   6
#define BREAK_BIT      4
#define DECIMAL_BIT    3
#define INTERRUPT_BIT  2
#define ZERO_BIT       1
#define CARRY_BIT      0

#define STATUS_NEGATIVE   (1 << NEGATIVE_BIT)
#define STATUS_OVERFLOW   (1 << OVERFLOW_BIT)
#define STATUS_BREAK      (1 << BREAK_BIT)
#define STATUS_DECIMAL    (1 << DECIMAL_BIT)
#define STATUS_INTERRUPT  (1 << INTERRUPT_BIT)
#define STATUS_ZERO       (1 << ZERO_BIT)
#define STATUS_CARRY      CARRY_BIT

#define BIT_VALUE(b,i) ((b & (1 << i)) >> i)
#define SET_BIT(b,i) b |= (1 << i)
#define CLEAR_BIT(b,i) b &= ~(1 << i)
#define ASSIGN_BIT(b,i,v) if (v) { SET_BIT(b,i); } else { CLEAR_BIT(b,i); }

enum AddressingMode {
  Implicit,
  Accumulator,
  Immediate,
  ZeroPage,
  ZeroPageX,
  ZeroPageY,
  Relative,
  Absolute,
  AbsoluteX,
  AbsoluteY,
  Indirect,
  IndirectX,
  IndirectY
};

class Bus;

class CPU6502 {
  private:
    Bus* bus;
    uint8_t currentValue;
    uint16_t currentAddress;
    uint8_t cycles;
    uint8_t pageBoundaryCrossed;
    AddressingMode addressingMode;

  public:
    CPU6502();
    ~CPU6502();

    uint8_t a;   // Accumulator
    uint8_t x;   // X
    uint8_t y;   // Y
    uint16_t pc; // Program Counter
    uint8_t sp;  // Stack Pointer
    uint8_t p;   // Status Register

    // Addressing Modes
    void AM_IMP();
    void AM_ACC();
    void AM_IMM();
    void AM_ZP();
    void AM_ZPX();
    void AM_ZPY();
    void AM_REL();
    void AM_ABS();
    void AM_ABX();
    void AM_ABY();
    void AM_IND();
    void AM_INX();
    void AM_INY();

    // Instructions
    void I_ADC();
    void I_AND();
    void I_ASL();
    void I_BCC();
    void I_BCS();
    void I_BEQ();
    void I_BIT();
    void I_BMI();
    void I_BNE();
    void I_BPL();
    void I_BRK();
    void I_BVC();
    void I_BVS();
    void I_CLC();
    void I_CLD();
    void I_CLI();
    void I_CLV();
    void I_CMP();
    void I_CPX();
    void I_CPY();
    void I_DEC();
    void I_DEX();
    void I_DEY();
    void I_EOR();
    void I_INC();
    void I_INX();
    void I_INY();
    void I_JMP();
    void I_JSR();
    void I_LDA();
    void I_LDX();
    void I_LDY();
    void I_LSR();
    void I_NOP();
    void I_ORA();
    void I_PHA();
    void I_PHP();
    void I_PLA();
    void I_PLP();
    void I_ROL();
    void I_ROR();
    void I_RTI();
    void I_RTS();
    void I_SBC();
    void I_SEC();
    void I_SED();
    void I_SEI();
    void I_STA();
    void I_STX();
    void I_STY();
    void I_TAX();
    void I_TAY();
    void I_TSX();
    void I_TXA();
    void I_TXS();
    void I_TYA();

    // High Level CPU Control
    void connectToBus(Bus* b);

    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);

    void push(uint8_t value);
    uint8_t pop();

    void step();
    void printCPUState();
};
