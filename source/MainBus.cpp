#include <iostream>
#include <Log.hpp>
#include <MainBus.hpp>
#include <Cartridge.hpp>

/* CPU Memory Map
--------------------------------------- $10000
 Upper Bank of Cartridge ROM
--------------------------------------- $C000
 Lower Bank of Cartridge ROM
--------------------------------------- $8000
 Cartridge RAM (may be battery-backed)
--------------------------------------- $6000
 Expansion Modules
--------------------------------------- $5000
 Input/Output
--------------------------------------- $2000
 2kB Internal RAM, mirrored 4 times
--------------------------------------- $0000
*/

/* 2kB Interal RAM, mirrored 4 times */
MainBus::MainBus() : m_RAM(0x800), m_mapper(nullptr) {}
//& For Test
// MainBus::MainBus(Cartridge& cartridge) : m_RAM(0x800), cartridge(cartridge) {}

/*
 * 2kB = 0x800, 8kB = 0x2000
 * RAM Bit Extensions by `addr & 0x7ff`
*/

Data MainBus::read(const Address& addr) const {
  if (addr < 0x2000) {
    return m_RAM[addr & 0x7ff];
  } else if (addr < 0x4020) {
    if (addr < 0x4000) {
      auto it = m_readCallbacks.find(static_cast<IORegisters> (addr & 0x2007));
      if (it != m_readCallbacks.end()) {
        return (it->second)();
      } else {
        LOG(InfoVerbose) << "No read callback registered for I/O register at: " << std::hex << +addr << std::endl;
      }
    } else if (0x4014 <= addr && addr < 0x4018) {
      auto it = m_readCallbacks.find(static_cast<IORegisters> (addr));
      if (it != m_readCallbacks.end()) {
        return (it->second)();
      } else {
        LOG(InfoVerbose) << "No read callback registered for I/O register at: " << std::hex << +addr << std::endl;
      }
    } else {
      LOG(InfoVerbose) << "Read access attempt at: " << std::hex << +addr << std::endl;
    }
  } else if (addr < 0x6000) {
    LOG(InfoVerbose) << "Expansion ROM read attempted. This is currently unsupported" << std::endl;
  } else if (addr < 0x8000) {
    if (m_mapper->hasExtendedRAM()) {
      return m_extRAM[addr - 0x6000];
    }
  } else {
    return m_mapper->readPRG(addr);
  }
  return 0;
}

void MainBus::write(const Address& addr, const Data& value) {
  if (addr < 0x2000) {
    m_RAM[addr & 0x7ff] = value;
  } else if (addr < 0x4020) {
    if (addr < 0x4000) { // PPU registers, mirrored
      auto it = m_writeCallbacks.find(static_cast<IORegisters> (addr & 0x2007));
      if (it != m_writeCallbacks.end()) {
        (it->second)(value);
        // Second object is the pointer to the function object
        // Dereference the function pointer and call it
      } else {
        // std::cerr << 1 << '\n';
        LOG(InfoVerbose) << "No write callback registered for I/O register at: " << std::hex << +addr << std::endl;
      }
    } else if (0x4014 <= addr && addr < 0x4017) { // only some registers
      auto it = m_writeCallbacks.find(static_cast<IORegisters> (addr));
      // std::cerr << 2 << '\n';
      // std::cerr << std::hex << (int) addr << '\n';
      // std::cerr << m_readCallbacks.size() << '\n';
      if (it != m_writeCallbacks.end()) {
        (it->second)(value);
      } else {
        LOG(InfoVerbose) << "No write callback registered for I/O register at: " << std::hex << +addr << std::endl;
      }
    } else {
      // std::cerr << 3 << '\n';
      LOG(InfoVerbose) << "Write access attmept at: " << std::hex << +addr << std::endl;
    }
  } else if (addr < 0x6000) {
    LOG(InfoVerbose) << "Expansion ROM access attempted. This is currently unsupported" << std::endl;
  } else if (addr < 0x8000) {
    if (m_mapper->hasExtendedRAM()) {
      m_extRAM[addr - 0x6000] = value;
    }
  } else {
    m_mapper->writePRG(addr, value);
  }
}

const Data* MainBus::getPagePtr(Data page) {
  Address addr = page << 8;
  if (addr < 0x2000) {
    return &m_RAM[addr & 0x7ff];
  } else if (addr < 0x4020) {
    LOG(Error) << "Register address memory pointer access attempt" << std::endl;
  } else if (addr < 0x6000) {
    LOG(Error) << "Expansion ROM access attempted, which is unsupported" << std::endl;
  } else if (addr < 0x8000) {
    if (m_mapper->hasExtendedRAM()) {
      return &m_extRAM[addr - 0x6000];
    }
  }
  return nullptr;
}

bool MainBus::setMapper(Mapper* mapper) {
  m_mapper = mapper;
  if (!mapper) {
    LOG(Error) << "Mapper pointer is nullptr." << std::endl;
    return false;
  }
  if (mapper->hasExtendedRAM()) {
    m_extRAM.resize(0x2000);
  }
  return true;
}

bool MainBus::setWriteCallback(IORegisters reg, std::function<void(Data)> callback) {
  if (!callback) {
    LOG(Error) << "callback argument is nullptr." << std::endl;
    return false;
  }
  return m_writeCallbacks.emplace(reg, callback).second;
}

bool MainBus::setReadCallback(IORegisters reg, std::function<Data(void)> callback) {
  if (!callback) {
    LOG(Error) << "callback argument is nullptr." << std::endl;
    return false;
  }
  return m_readCallbacks.emplace(reg, callback).second;
}