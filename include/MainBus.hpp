#ifndef _MAIN_BUS_H_
#define _MAIN_BUS_H_

#include <vector>
#include <Cartridge.hpp>
#include <Mapper.hpp>

class MainBus {
 public:
  MainBus();
  MainBus(Cartridge& cartridge);  //& For Test
  Data read(const Address& addr) const;
  void write(const Address& addr, const Data& value);
  bool setMapper(Mapper* mapper);

 private:
  // save message of RAM
  std::vector<Data> m_RAM;
  std::vector<Data> m_extRAM;  // extend RAM
  Cartridge cartridge;
  Mapper* m_mapper;
};

#endif  // _MAIN_BUS_H_