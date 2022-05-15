#ifndef _CPU_OPCODES_
#define _CPU_OPCODES_

/* MCS 6502 CPU operation codes definition */

/*
 *        7  6  5    4  3  2    1  0
 * Data: [op op op] [ad ad ad] [im im]
 *       operation  address  instruction mode(00~10)
*/

constexpr auto instructionModeMask = 0x3;

constexpr auto operationMask = 0xe0;
constexpr auto operationShift = 5;

constexpr auto addrModeMask = 0x1c;
constexpr auto addrModeShift = 2;

constexpr auto branchInstructionMask = 0x1f;
constexpr auto branchInstructionMaskResult = 0x10;
constexpr auto branchConditionMask = 0x20;
constexpr auto branchOnFlagShift = 6;

/* 
 * When the 6502 starts up it loads an address vector from 0xFFFC/0xFFFD, sets its 
 * program counter to that address, and start fetching instructions from there.
*/

constexpr auto NMI = 0xfffa;
constexpr auto RESET = 0xfffc;
constexpr auto IRQ = 0xfffe;

/*
 * imm = #$00
 * zp = $00
 * zpx = $00,X
 * zpy = $00,Y
 * izx = ($00,X)
 * izy = ($00),Y
 * abs = $0000
 * abx = $0000,X
 * aby = $0000,Y
 * ind = ($0000)
 * rel = $0000 (PC-relative)
*/

enum branchOnFlag {
  Negative,
  Overflow,
  Carry,
  Zero
};

enum operation1 {
  ORA,
  AND,
  EOR,
  ADC,
  STA,
  LDA,
  CMP,
  SBC,
};

enum operation2 {
  ASL,
  ROL,
  LSR,
  ROR,
  STX,
  LDX,
  DEC,
  INC,
};

enum operation0 {
  BIT = 1,
  STY = 4,
  LDY,
  CPY,
  CPX,
};

enum addrMode1 {
  IndexedIndirectX,
  ZeroPage,
  Immediate,
  Absolute,
  IndirectY,
  IndexedX,
  AbsoluteY,
  AbsoluteX,
};

enum addrMode2 {
  Immediate_,
  ZeroPage_,
  Accumulator,
  Absolute_,
  Indexed = 5,
  AbsoluteIndexed = 7,
};

enum operationImplied {
  NOP = 0xea,
  BRK = 0x00,
  JSR = 0x20,
  RTI = 0x40,
  RTS = 0x60,

  JMP = 0x4C,
  JMPI = 0x6C,  // JMP Indirect

  PHP = 0x08,
  PLP = 0x28,
  PHA = 0x48,
  PLA = 0x68,

  DEY = 0x88,
  DEX = 0xca,
  TAY = 0xa8,
  INY = 0xc8,
  INX = 0xe8,

  CLC = 0x18,
  SEC = 0x38,
  CLI = 0x58,
  SEI = 0x78,
  TYA = 0x98,
  CLV = 0xb8,
  CLD = 0xd8,
  SED = 0xf8,

  TXA = 0x8a,
  TXS = 0x9a,
  TAX = 0xaa,
  TSX = 0xba,
};

/*
 * Number of clocks required per instruction
 * 0 implies unused opcode
*/
int operationCycles[0x100] = {
  7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
  6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
  6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
  6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
  0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
  2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
  2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,
  2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,
  2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
  2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 2, 4, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
};



#endif  // _CPU_OPCODES_