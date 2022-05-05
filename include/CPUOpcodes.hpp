#ifndef _CPU_OPCODES_
#define _CPU_OPCODES_

/* MCS 6502 CPU operation codes definition */

/* 
 * When the 6502 starts up it loads an address vector from 0xFFFC/0xFFFD, sets its 
 * program counter to that address, and start fetching instructions from there.
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

constexpr auto NMI = 0xfffa;
constexpr auto RESET = 0xfffc;
constexpr auto IRQ = 0xfffe;

enum branchOnFlag {
  Negative,
  Overflow,
  Carry,
  Zero,
};

enum operation1 {
  ORA, /* ACC:=ACC or {adr} */
  AND, /* ACC:=ACC&{adr} */
  EOR, /* ACC:=ACC exor {adr} */
  ADC, /* ACC:=ACC+{adr} */
  STA, /* STOREACC {adr}:=ACC */
  LDA, /* LOADACC ACC:={adr} */
  CMP, /* ACC-{adr} */
  SBC, /* ACC:=ACC-{adr} */
};

enum operation2 {
  ASL, /* {adr}:={adr}<<1 */
  ROL, /* {adr}:={adr}*2+C */
  LSR, /* {adr}:={adr}/2 */
  ROR, /* {adr}:={adr}/2+C*128 */
  STX, /* {adr}:=X */
  LDX, /* X:={adr} */
  DEC, /* {adr}:={adr}-1 */
  INC, /* {adr}:={adr}+1 */
};

enum operation0 {
  BIT = 1, /* N:=b7 V:=b6 Z:=ACC&{adr} */
  STY = 4, /* STOREY {adr}:=Y */
  LDY,     /* LOADY Y:={adr} */
  CPY,     /* Y-{adr} */
  CPX,     /* X-{adr} */
};

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
  /* Jump/Flag commands */
  BRK = 0x00,
  JSR = 0x20,
  RTI = 0x40,
  RTS = 0x60,
  JMP = 0x4C,
  JMPI = 0x6C, //JMP Indirect
  CLC = 0x18,
  SEC = 0x38,
  CLI = 0x58,
  SEI = 0x78,
  CLV = 0xb8,
  CLD = 0xd8,
  SED = 0xf8,
  NOP = 0xea,

 /* Move commands */
  TAX = 0xaa,
  TXA = 0x8a,
  TAY = 0xa8,
  TYA = 0x98,
  TSX = 0xba,
  TXS = 0x9a,
  PLA = 0x68,
  PHA = 0x48,
  PLP = 0x28,
  PHP = 0x08,

  /* Logical and arithmetic commands */
  DEX = 0xca,
  DEY = 0x88,
  INX = 0xe8,
  INY = 0xc8,
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