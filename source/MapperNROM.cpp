#include <MapperNROM.hpp>
#include <Log.hpp>
#include <cassert>
// #include <algo/debug.h>

MapperNROM::MapperNROM(Cartridge& cart) : Mapper(cart, Mapper::NROM) {
  if (cart.getROM().size() == 0x4000) {
    m_oneBank = true; // 1 bank
  } else {
    m_oneBank = false; // 2 banks
  }

  // debug(cart.getVROM().size(), cart.getVROM());
  if (cart.getVROM().size() == 0) {
    m_usesCharacterRAM = true;
    m_characterRAM.resize(0x2000);
    LOG(Info) << "Uses character RAM." << std::endl;
  } else {
    LOG(Info) << "Using CHR-ROM" << std::endl;
    m_usesCharacterRAM = false;
  }
}

Data MapperNROM::readPRG(Address addr) {
  if (!m_oneBank) {
    return m_cartridge.getROM()[addr - 0x8000];
  } else {
    // mirrored
    return m_cartridge.getROM()[(addr - 0x8000) & 0x3fff];
  }
}

void MapperNROM::writePRG(Address addr, Data value) {
  LOG(InfoVerbose) << "ROM memory write attempt at " << +addr << " to set " << +value << std::endl;
}

const Data* MapperNROM::getPagePtr(Address addr) {
  if (!m_oneBank) {
    return &m_cartridge.getROM()[addr - 0x8000];
  } else {
    // mirrored
    return &m_cartridge.getROM()[(addr - 0x8000) & 0x3fff];
  }
}

Data MapperNROM::readCHR(Address addr) {
  if (m_usesCharacterRAM) {
    assert(0 <= addr && addr < 0x2000);
    return m_characterRAM[addr];
  } else {
    return m_cartridge.getVROM()[addr];
  }
}

void MapperNROM::writeCHR(Address addr, Data value) {
  if (m_usesCharacterRAM) {
    assert(0 <= addr && addr < 0x2000);
    m_characterRAM[addr] = value;
  } else {
    LOG(Info) << "Read-only CHR memory write attempt at " << std::hex << addr << std::endl;
  }
}