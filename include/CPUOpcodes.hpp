#ifndef _CPU_OPCODES_
#define _CPU_OPCODES_

/* MCS 6502 CPU operation codes definition */

/* 
 * When the 6502 starts up it loads an address vector from 0xFFFC/0xFFFD, sets its 
 * program counter to that address, and start fetching instructions from there.
*/
constexpr auto resetVector = 0xfffc; // 0xfffd

#endif  // _CPU_OPCODES_