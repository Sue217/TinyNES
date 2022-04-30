#ifndef _MAIN_BUS_H_
#define _MAIN_BUS_H_

#include "Cartridge.hpp"
#include <vector>

class MainBus {
 public:
  MainBus() = default;
  Data read(const Address& addr) const;
  void write(const Address& addr, const Data& value);
 private:
  // save message of RAM
  std::vector<Data> m_RAM;
  std::vector<Data> m_extRAM; // extend RAM
};

#endif // _MAIN_BUS_H_