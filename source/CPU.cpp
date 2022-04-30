#include "MainBus.hpp"
#include "CPUOpcodes.hpp"
#include "CPU.hpp"

CPU::CPU(MainBus& mem) : m_bus(mem) {}

// void CPU::interrupt(InterruptType type)

void CPU::reset() {
  reset(read(resetVector));
}

/* 
 * Stack pointer starts at top of page 1, but lands at 0xFD (rather than 0xFF)
 * after the hardware startup is complete.
*/

void CPU::reset(Address start) {
  m_skipCycles = m_cycles = 0;
  r_acc = r_x = r_y = 0;
  f_i = true;
  f_c = f_z = f_d = f_v = f_n = false;
  r_pc = start;
  r_sp = 0xfd;
}

void CPU::step() {
  Data opcode = m_bus.read(r_pc++);
}

void CPU::push(Data value) {
  m_bus.write(0x100 | r_sp, value);
  --r_sp;
}

void CPU::pull() {
  return m_bus.read(0x100 | ++r_sp);
}
