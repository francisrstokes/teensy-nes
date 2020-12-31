#pragma once

#include <stdint.h>
#include "CPU.h"

#define RAM_SIZE 1024 * 64

class CPU6502;

class Bus {
  public:
    Bus();
    ~Bus();

    // Devices connected to the bus
    CPU6502 cpu;
    uint8_t ram[RAM_SIZE];

    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
};
