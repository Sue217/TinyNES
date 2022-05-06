#include <MainBus.hpp>
#include <CpuOpcodes.hpp>
#include <CPU.hpp>
#include <Log.hpp>

CPU::CPU(MainBus& mem) : m_bus(mem) {
  f_b = 0;
  f_u = 1;
}

// void CPU::interrupt(InterruptType type)

void CPU::reset() {
  reset(readAddress(RESET));
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
  // Data opcode = m_bus.read(r_pc++);
  ++m_cycles;
  if (m_skipCycles-- > 1) {
    return;
  }
  m_skipCycles = 0;
  /* Generate Program Status Words(PSW)
  int psw = f_n << 7 |
            f_v << 6 |
            f_u << 5 |
            f_b << 4 |
            f_d << 3 |
            f_i << 2 |
            f_z << 1 |
            f_c;
  */

  Data opcode = m_bus.read(r_pc++);
  auto cycleLength = operationCycles[opcode];

  if (cycleLength && (executeImplied(opcode) || executeBranch(opcode) ||
                      executeType1(opcode) || executeType2(opcode))) {
    m_skipCycles += cycleLength;
  } else {
    LOG(Error) << "Unrecognized opcode: " << std::hex << +opcode << std::endl;
  }
}

Address CPU::readAddress(Address addr) {
  return m_bus.read(addr) | m_bus.read(addr + 1) << 8;
}

void CPU::push(Data value) {
  m_bus.write(0x100 | r_sp, value);
  --r_sp;
}

Data CPU::pull() {
  return m_bus.read(0x100 | ++r_sp);
}

void CPU::setPageCrossed(Address x, Address y, int inc) {
  // Page is determined by the high byte
  if ((x & 0xff00) != (y & 0xff00)) {
    m_skipCycles += inc;
  }
}

void CPU::set_ZN(Data value) {
  f_z = !value;
  f_n = value & 0x80;
}

bool CPU::executeImplied(Data opcode) {
  switch (static_cast<operationImplied> (opcode)) {
    case NOP: break;
    case JSR:
      /*
       * This instruction transfers control of the PC to a subroutine
       * location but leaves a return pointer on the stack to allow the
       * user to return to perform the next instruction in the main program
       * after the subroutine is complete. To accomplish this, JSR instruction
       * stores the PC address which points to the last byte of the jump
       * instruction onto the stack using the SP. The stack byte contains the PC
       * high first, followed by PC low. The JSR then transfers the addresses
       * following the jump instruction to the PC low and the PC high, thereby
       * directing the program to begin at that new address.
       * The symbolic notation for this is PC + 2 v, (PC + 1) -> PCL, (PC + 2) -> PCH.
       */
      push(static_cast<Data> ((r_pc + 1) >> 8));
      push(static_cast<Data> (r_pc + 1));
      r_pc = readAddress(r_pc);
      break;
    case RTI:
      {
        Data flags = pull();
        f_n = flags & 0x80;
        f_v = flags & 0x40;
        f_d = flags & 0x8;
        f_i = flags & 0x4;
        f_z = flags & 0x2;
        f_c = flags & 0x1;
        /*
         * 7 6 5 3 2 1 0
         * N V B D I Z C
         * √ √   √ √ √ √
        */
      }
      r_pc = pull();
      r_pc |= pull() << 8;
      break;
    case JMP:
      r_pc = readAddress(r_pc);
      break;
    case JMPI:
      {
        Address location = readAddress(r_pc);
        /* 
         * 6502 has a bug such that the when the vector of anindirect address begins at the last byte of a page,
         * the second byte is fetched from the beginning of that page rather than the beginning of the next
         * Recreating here:
        */ 
        Address page = location & 0xff00;
        r_pc = m_bus.read(location) | m_bus.read(page | ((location + 1) & 0xff)) << 8;
      }
      break;
    case PHP:
      {
        auto pre = f_b;
        f_b = 1; // PHP pushes with the B flag as 1, no matter what
        Data flags = f_n << 7 |
                     f_v << 6 |
                     f_u << 5 |
                     f_b << 4 |
                     f_d << 3 |
                     f_i << 2 |
                     f_z << 1 |
                     f_c;
        f_b = pre;
        push(flags);
      }
      break;
    case PLP:
      {
        Data flags = pull();
        f_n = flags & 0x80;
        f_v = flags & 0x40;
        f_d = flags & 0x8;
        f_i = flags & 0x4;
        f_z = flags & 0x2;
        f_c = flags & 0x1;
      }
      break;
    case PLA:
      r_acc = pull();
      break;
    case PHA:
      push(r_acc);
      break;
    case DEX:
      --r_x;
      set_ZN(r_x);
      break;
    case DEY:
      --r_y;
      set_ZN(r_y);
      break;
    case TAX:
      r_x = r_acc;
      set_ZN(r_x);
      break;
    case TXA:
      r_acc = r_x;
      set_ZN(r_acc);
      break;
    case TAY:
      r_y = r_acc;
      set_ZN(r_y);
      break;
    case TYA:
      r_acc = r_y;
      set_ZN(r_acc);
      break;
    case TSX:
      r_x = r_sp;
      set_ZN(r_x);
      break;
    case TXS:
      r_sp = r_x;
      break;
    case INX:
      ++r_x;
      set_ZN(r_x);
      break;
    case INY:
      ++r_y;
      set_ZN(r_y);
      break;
    case CLC:
      f_c = 0;
      break;
    case SEC:
      f_c = 1;
      break;
    case CLI:
      f_i = 0;
      break;
    case SEI:
      f_i = 1;
      break;
    case CLD:
      f_d = 0;
      break;
    case SED:
      f_d = 1;
      break;
    case CLV:
      f_v = 0;
      break;
    default:
      return false;
  };
  return true;
}

bool CPU::executeBranch(Data opcode) {
  if ((opcode & branchInstructionMask) == branchInstructionMaskResult) {
    // branch is initialized to the condition required (for the flag specified later)
    bool branch = opcode & branchConditionMask;

    // set branch to true if the given condition is met by the given flag
    // We use `XNOR` here, it is true if either both operands are true or false
    switch (opcode >> branchOnFlagShift) {
      case Negative:
        branch = !(branch ^ f_n);
        break;
      case Overflow:
        branch = !(branch ^ f_v);
        break;
      case Carry:
        branch = !(branch ^ f_c);
        break;
      case Zero:
        branch = !(branch ^ f_z);
        break;
      default:
        return false;
    }

    if (branch) {
      int8_t offset = m_bus.read(r_pc++);
      ++m_skipCycles;
      auto new_pc = static_cast<Address> (r_pc + offset);
      setPageCrossed(r_pc, new_pc, 2);
      r_pc = new_pc;
    } else {
      ++r_pc;
    }
    return true;
  }
  return false;
}

bool CPU::executeType1(Data opcode) {
  if ((opcode & instructionModeMask) == 0x1) {
    Address location = 0; // Location of the operand, could be in RAM
    auto op = static_cast<operation1> ((opcode & operationMask) >> operationShift);
    auto addr = static_cast<addrMode1> ((opcode & addrModeMask) >> addrModeShift);
    
    switch (addr) {
      case IndexedIndirectX:
        // M[R[r_x]]
        {
          Data zero_addr = r_x + m_bus.read(r_pc++);
          /* Addresses wrap in zero page mode, thus pass through a mask */
          location = m_bus.read(zero_addr & 0xff) | m_bus.read((zero_addr + 1) & 0xff) << 8;
        }
        break;
      case ZeroPage:
        location = m_bus.read(r_pc++);
        break;
      case Immediate:
        // destination = source($Imm)
        location = r_pc++;
        break;
      case Absolute:
        // M[Imm]
        location = readAddress(r_pc);
        r_pc += 2;
        break;
      case IndirectY:
        {
          Data zero_addr = m_bus.read(r_pc++);
          location = m_bus.read(zero_addr & 0xff) | m_bus.read((zero_addr + 1) & 0xff) << 8;
          if (op != STA) {
            setPageCrossed(location, location + r_y);
            location += r_y;
          }
        }
        break;
      case IndexedX:
        // M[R[r_pc] + R[r_x]]
        // Address wraps around in the zero page
        location = (m_bus.read(r_pc++) + r_x) & 0xff;
        break;
      case AbsoluteY:
        location = readAddress(r_pc);
        r_pc += 2;
        break;
      case AbsoluteX:
        location = readAddress(r_pc);
        r_pc += 2;
        if (op != STA) {
          setPageCrossed(location, location + r_x);
        }
        location += r_x;
        break;
      default:
        return false;
    }

    switch (op) {
      /* 
       * The accumulator really acts as two functions:
       * 1) It is one of the primary storage points for the machine;
       * 2) It is the point at which intermediate results are normally stored.
      */
      case ORA:
        r_acc |= m_bus.read(location);
        set_ZN(r_acc);
        break;
      case AND:
        r_acc &= m_bus.read(location);
        set_ZN(r_acc);
        break;
      case EOR:
        r_acc ^= m_bus.read(location);
        set_ZN(r_acc);
        break;
      case ADC:
        {
          Data operand = m_bus.read(location);
          std::uint16_t sum = r_acc + operand + f_c;
          //! Carry forward or UNSIGNED OVERFLOW
          f_c = sum & 0x100;
          /*
           * SIGNED OVERFLOW, would only happen if the `sign` of sum is
           * different from BOTH the operands
           * Overflow: (+)+(+)=(-)
           * Underflow: (-)+(-)=(+)
          */
          f_v = (r_acc ^ sum) & (operand ^ sum) & 0x80;
          r_acc = static_cast<Data> (sum);
          set_ZN(r_acc);
        }
        break;
      case STA:
        m_bus.write(location, r_acc);
        break;
      case LDA:
        r_acc = m_bus.read(location);
        set_ZN(r_acc);
        break;
      case SBC:
        /*
         * The carry flag(f_c) is set if the result is greater than or equal to 0.
         * The carry flag(f_c) is reset when the result is less than 0, indicating a borrow.
         * The overflow flag(f_v) is set when the result exceeds +127 or -127, otherwise it is reset.
         * The negative flag(f_n) is set if the result in the accumulator has bit 7 on, otherwise it is reset.
         * The Z flag(f_z) is set if the result in the accumulator is 0, otherwise it is reset.
        */
        {
          // High carry means "no borrow", thus negate and subtract
          std::uint16_t subtrahend = m_bus.read(location);
          std::uint16_t diff = r_acc - subtrahend - !f_c;
          // If the 9th bit is 1, the resulting number is negative => borrow => low carry
          f_c = !(diff & 0x100);
          /*
           * Same as ADC, except instead of the subtrahend, 
           * substitute with it's one complement
          */
          f_v = (r_acc ^ diff) & (~subtrahend ^ diff) & 0x80;
          r_acc = diff; //! r_acc = static_cast<Data> (diff);
          set_ZN(diff);
        }
        break;
      case CMP:
        {
          std::uint16_t diff = r_acc - m_bus.read(location);
          f_c = !(diff & 0x100);
          set_ZN(diff);
        }
        break;
      default:
        return false;
    }
    return true;
  }
  return false;
}

bool CPU::executeType2(Data opcode) {
  if ((opcode & instructionModeMask) == 0x2) {
    Address location = 0;
    auto op = static_cast<operation2> ((opcode & operationMask) >> operationShift);
    auto addr_mode = static_cast<addrMode2> ((opcode & addrModeMask) >> addrModeShift);

    switch (addr_mode) {
      case Immediate_:
        location = r_pc++;
        break;
      case ZeroPage_:
        location = m_bus.read(r_pc++);
        break;
      case Accumulator:
        break;
      case Absolute_:
        location = readAddress(r_pc);
        r_pc += 2;
        break;
      case Indexed:
        {
          location = m_bus.read(r_pc++);
          Data index;
          if (op == LDX || op == STX) {
            index = r_y;
          } else {
            index = r_x;
          }
          setPageCrossed(location, location + index);
          location += index;
        }
        break;
      default:
        return false;
    }

    std::uint16_t operand = 0;
    switch (op) {
      case ASL:
      case ROL:
        if (addr_mode == Accumulator) {
          auto prev_c = f_c;
          f_c = r_acc & 0x80;
          r_acc <<= 1;
          // If Rotating, set the bit-0 to the the previous carry
          r_acc = r_acc | (prev_c && (op == ROL));
          set_ZN(r_acc);
        } else {
          auto prev_c = f_c;
          operand = m_bus.read(location);
          f_c = operand & 0x80;
          operand = operand << 1 | (prev_c && (op == ROL));
          set_ZN(operand);
          m_bus.write(location, operand);
        }
        break;
      case LSR:
      case ROR:
        if (addr_mode == Accumulator) {
          auto prev_c = f_c;
          f_c = r_acc & 1;
          r_acc >>= 1;
          // If Rotating, set the bit-7 to the previous carry
          r_acc = r_acc | (prev_c && (op == ROR)) << 7;
          set_ZN(r_acc);
        } else {
          auto prev_c = f_c;
          operand = m_bus.read(location);
          f_c = operand & 1;
          operand = operand >> 1 | (prev_c && (op == ROR)) << 7;
          set_ZN(operand);
          m_bus.write(location, operand);
        }
        break;
      case STX:
        m_bus.write(location, r_x);
        break;
      case LDX:
        r_x = m_bus.read(location);
        set_ZN(r_x);
        break;
      case DEC:
        {
          auto tmp = m_bus.read(location) - 1;
          set_ZN(tmp);
          m_bus.write(location, tmp);
        }
        break;
      case INC:
        {
          auto tmp = m_bus.read(location) + 1;
          set_ZN(tmp);
          m_bus.write(location, tmp);
        }
        break;
      default:
        return false;
    }
    return true;
  }
  return false;
}

bool CPU::executeType0(Data opcode) {
  if ((opcode & instructionModeMask) == 0x0) {
    Address location = 0;
    auto op = static_cast<operation0>((opcode & operationMask) >> operationShift);
    auto addr = static_cast<addrMode2> ((opcode & addrModeMask) >> addrModeShift);
    
    switch (addr) {
      case Immediate_:
        location = r_pc++;
        break;
      case ZeroPage_:
        location = m_bus.read(r_pc++);
        break;
      case Absolute_:
        location = readAddress(r_pc);
        r_pc += 2;
        break;
      case Indexed:
        // Address wraps around in the zero page
        location = (m_bus.read(r_pc++) + r_x) & 0xff;
        break;
      case AbsoluteIndexed:
        location = readAddress(r_pc);
        r_pc += 2;
        setPageCrossed(location, location + r_x);
        location += r_x;
        break;
      default:
        return false;
    }

    std::uint16_t operand = 0;
    switch (op) {
      case BIT:
        operand = m_bus.read(location);
        f_z = !(r_acc & operand);
        f_v = operand & 0x40;
        f_n = operand & 0x80;
        break;
      case STY:
        m_bus.write(location, r_y);
        break;
      case LDY:
        r_y = m_bus.read(location);
        set_ZN(r_y);
        break;
      case CPY:
        {
          std::uint16_t diff = r_y - m_bus.read(location);
          f_c = !(diff & 0x100);
          set_ZN(diff);
        }
        break;
      case CPX:
        {
          std::uint16_t diff = r_x - m_bus.read(location);
          f_c = !(diff & 0x100);
          set_ZN(diff);
        }
        break;
      default:
        return false;
    }
    return true;
  }
  return false;
}