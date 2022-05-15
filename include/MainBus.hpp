#ifndef _MAIN_BUS_H_
#define _MAIN_BUS_H_

#include <map>
#include <vector>
#include <Cartridge.hpp>
#include <Mapper.hpp>

//& http://fms.komkon.org/EMUL8/NES.html#LABF
enum IORegisters {
  PPUCTRL = 0x2000,
  PPUMASK,
  PPUSTATUS,
  OAMADDR,  // Sprite Memory Address
  OAMDATA,  // Sprite Memory Data
  PPUSCROL, // Screen Scroll Offsets
  PPUADDR,
  PPUDATA,
  OAMDMA = 0x4014, // DMA access to the Sprite Memory
  JOY1 = 0x4016,   // Joystick1 + Strobe
  JOY2 = 0x4017,   // Joystick2 + Strobe
};

class MainBus {
 public:
  MainBus();
  MainBus(Cartridge& cartridge);  //& For Test
  Data read(const Address& addr) const;
  void write(const Address& addr, const Data& value);
  bool setMapper(Mapper* mapper);
  bool setWriteCallback(IORegisters reg, std::function<void(Data)> callback);
  bool setReadCallback(IORegisters reg, std::function<Data(void)> callback);
  const Data* getPagePtr(Data page);

 private:
  // save message of RAM
  std::vector<Data> m_RAM;
  std::vector<Data> m_extRAM;  // extend RAM
  Cartridge cartridge;
  Mapper* m_mapper;
  std::map<IORegisters, std::function<void(Data)>> m_writeCallbacks;
  std::map<IORegisters, std::function<Data(void)>> m_readCallbacks;
};

#endif  // _MAIN_BUS_H_