#include <Emulator.hpp>
#include <Log.hpp>

Emulator::Emulator() : m_cpu(m_bus) {}

void Emulator::run(std::string rom_path) {
  if (!m_cartridge.loadFromFile(rom_path)) {
    LOG(Error) << "Unable to load ROM from file: " << rom_path << std::endl;
    return;
  }
  
  m_mapper = Mapper::createMapper(static_cast<Mapper::Type> (m_cartridge.getMapper()), m_cartridge);

  if (!m_mapper) {
    LOG(Error) << "Creating Mapper failed. Probably unsupported." << std::endl;
    return;
  }

  //TODO PPU

  if (!m_bus.setMapper(m_mapper.get())) {
    return;
  }

  m_cpu.reset();
}