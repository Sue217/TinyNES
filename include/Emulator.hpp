#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <CPU.hpp>
#include <Cartridge.hpp>
#include <MainBus.hpp>
#include <Mapper.hpp>

class Emulator {
 public:
  Emulator();
  void run(std::string rom_path);

 private:
  MainBus m_bus;
  CPU m_cpu;
  Cartridge m_cartridge;
  std::unique_ptr<Mapper> m_mapper;
};

#endif  // _EMULATOR_H_