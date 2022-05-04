#ifndef _CPU_
#define _CPU_

#include <cstdint>
#include <MainBus.hpp>

class CPU {
 public:
  enum InterruptType {
    _IRQ_,
    _NMI_,
    _BRK_
  };

  CPU(MainBus& mem);

  //! Assuming sequential execution, for asynchronously calling this with Execute, further work needed
  // void interrupt(InterruptType type);

  void reset();
  void reset(Address start);
  void step();
  // void log();

  Address get_PC() {
    return r_pc;
  }

 private:
  // Instructions are split into five sets to make decoding easier
  // These functions return true if they succeed
  // Simulate instructions execution
  bool executeImplied(Data opcode);
  bool executeBranch(Data opcode);
  bool executeType1(Data opcode);
  bool executeType2(Data opcode);
  bool executeType0(Data opcode);

  Address readAddress(Address& addr);
  void push(Data value);
  Data pull();

  //! If x and y in different pages, increases the m_skipCycles by `inc`
  void setPageCrossed(Address x, Address y, int inc = 1);
  void set_ZN(Data value);

  int m_skipCycles;
  int m_cycles;

  // Registers
  std::uint16_t r_pc;
  std::uint8_t r_p; // processor status (PSW/PSR)
  std::uint8_t r_sp;
  std::uint8_t r_ix;
  std::uint8_t r_iy;
  std::uint8_t r_acc;

  // Status flags
  bool f_c; // Carry           >>> 1 = true           0
  bool f_z; // Zero            >>> 1 = result zero    1
  bool f_i; // IRQ disable     >>> 1 = disable        2
  bool f_d; // Decimal mode    >>> 1 = true           3
  bool f_b; // Brk command     >>> 1 = brk            4
  bool f_u; // unused (always 1)                      5#
  bool f_v; // Overflow        >>> 1 = true           6
  bool f_n; // Negative        >>> 1 = neg            7

  f_b = 0;
  f_u = 1;

  MainBus& m_bus;
};

#endif // _CPU_