#include <MainBus.hpp>

/* 2kB Interal RAM, mirrored 4 times */
MainBus::MainBus() : m_RAM(0x800) {}

/* 
 * 2kB = 0x800, 8kB = 0x2000
 * RAM Bit Extensions by `addr & 0x7ff`
*/

Data MainBus::read(const Address& addr) const {
  if (addr < 0x2000) {
    return m_RAM[addr & 0x7ff];
  }
  return 0;
}

void MainBus::write(const Address& addr, const Data& value) {
  if (addr < 0x2000) {
    m_RAM[addr & 0x7ff] = value;
  }
}