#include <stdio.h>
#include <stdint.h>

#include "CPU.h"
#include "Bus.h"

int main() {
  Bus b = Bus();
  CPU6502 cpu = CPU6502();

  b.cpu = cpu;
  cpu.connectToBus(&b);

  return 0;
}
