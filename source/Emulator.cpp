#include <Emulator.hpp>
#include <Log.hpp>

Emulator::Emulator() : m_cpu(m_bus), m_screenScale(2.f) {}

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

  // //TODO PPU

  if (!m_bus.setMapper(m_mapper.get())) {
    return;
  }

  m_cpu.reset(); //! Cause segmentation fault

  m_window.create(sf::VideoMode(nesVideoWidth * m_screenScale, nesVideoHeight * m_screenScale),
                                "NES", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
  m_window.setVerticalSyncEnabled(true);
  m_emulatorScreen.create(nesVideoWidth, nesVideoHeight, m_screenScale, sf::Color::White);
  
  sf::Event event;
  sf::CircleShape shape(nesVideoHeight);
  // fill color
  shape.setFillColor(sf::Color::White);
  
  while (m_window.isOpen()) {
    while (m_window.pollEvent(event)) {
      if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
        m_window.close();
        return;
      }
      m_window.draw(shape);
      m_window.display();
    }
  }
}