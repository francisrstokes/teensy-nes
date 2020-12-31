#include "Bus.h"

Bus::Bus() {}
Bus::~Bus() {}

void Bus::write(uint16_t address, uint8_t value) {
  if (address >= 0 && address <= 0xffff) {
    ram[address] = value;
  }
};

uint8_t Bus::read(uint16_t address) {
  if (address >= 0 && address <= 0xffff) {
    return ram[address];
  }
  return 0;
};
