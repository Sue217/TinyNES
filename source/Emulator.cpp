#include <thread>
#include <chrono>
#include <Emulator.hpp>
#include <Log.hpp>

/*
 * [Player1]
A      = J
B      = K
Select = RShift
Start  = Return
Up     = W
Down   = S
Left   = A
Right  = D
*/
std::vector<sf::Keyboard::Key> player1 = { 
  sf::Keyboard::Key::J, 
  sf::Keyboard::Key::K, 
  sf::Keyboard::Key::RShift,
  sf::Keyboard::Key::Return,
  sf::Keyboard::Key::W,
  sf::Keyboard::Key::S,
  sf::Keyboard::Key::A,
  sf::Keyboard::Key::D,
};

/*
 * [Player2]
A      = Numpad5
B      = Numpad6
Select = Numpad8
Start  = Numpad9
Up     = Up
Down   = Down
Left   = Left
Right  = Right
*/
std::vector<sf::Keyboard::Key> player2 = { 
  sf::Keyboard::Key::Numpad5, 
  sf::Keyboard::Key::Numpad6, 
  sf::Keyboard::Key::Numpad8,
  sf::Keyboard::Key::Numpad9,
  sf::Keyboard::Key::Up,
  sf::Keyboard::Key::Down,
  sf::Keyboard::Key::Left,
  sf::Keyboard::Key::Right,
};

Emulator::Emulator() :
  m_cpu(m_bus),
  m_ppu(m_pictureBus, m_emulatorScreen),
  m_screenScale(2.f),
  m_cycleTimer(),
  m_cpuCycleDuration(std::chrono::nanoseconds(559)) {
    if (!m_bus.setReadCallback(PPUSTATUS, [&](void) { return m_ppu.getStatus(); }) ||
        !m_bus.setReadCallback(PPUDATA, [&](void) { return m_ppu.getData(); }) ||
        !m_bus.setReadCallback(JOY1, [&](void) { return m_controller1.read(); }) ||
        !m_bus.setReadCallback(JOY2, [&](void) { return m_controller2.read(); }) ||
        !m_bus.setReadCallback(OAMDATA, [&](void) { return m_ppu.getOAMData(); })) {
      LOG(Error) << "Critical error: Failed to set I/O callbacks" << std::endl;
    }

    if (!m_bus.setWriteCallback(PPUCTRL, [&](Data b) { m_ppu.control(b); }) ||
        !m_bus.setWriteCallback(PPUMASK, [&](Data b) { m_ppu.setMask(b); }) ||
        !m_bus.setWriteCallback(OAMADDR, [&](Data b) { m_ppu.setOAMAddress(b); }) ||
        !m_bus.setWriteCallback(PPUADDR, [&](Data b) { m_ppu.setDataAddress(b); }) ||
        !m_bus.setWriteCallback(PPUSCROL, [&](Data b) { m_ppu.setScroll(b); }) ||
        !m_bus.setWriteCallback(PPUDATA, [&](Data b) { m_ppu.setData(b); }) ||
        !m_bus.setWriteCallback(OAMDMA, [&](Data b) { DMA(b); }) ||
        !m_bus.setWriteCallback(JOY1, [&](Data b) { m_controller1.strobe(b); m_controller2.strobe(b); }) ||
        !m_bus.setWriteCallback(OAMDATA, [&](Data b) { m_ppu.setOAMData(b); })) {
      LOG(Error) << "Critical error: Failed to set I/O callbacks" << std::endl;
    }

    m_ppu.setInterruptCallback([&]() { m_cpu.interrupt(CPU::_NMI_); });
    m_controller1.setKeyBindings(player1);
    m_controller2.setKeyBindings(player2);
  }

void Emulator::run(std::string rom_path) {
  if (!m_cartridge.loadFromFile(rom_path)) return;

  m_mapper = Mapper::createMapper(
      static_cast<Mapper::Type>(m_cartridge.getMapper()), m_cartridge,
      [&]() { m_pictureBus.updateMirroring(); });
  if (!m_mapper) {
    LOG(Error) << "Creating Mapper failed. Probably unsupported." << std::endl;
    return;
  }

  if (!m_bus.setMapper(m_mapper.get()) || !m_pictureBus.setMapper(m_mapper.get())) return;

  m_cpu.reset();
  m_ppu.reset();

  m_window.create(sf::VideoMode(nesVideoWidth * m_screenScale, nesVideoHeight * m_screenScale),
                  "NES", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
  m_window.setVerticalSyncEnabled(true);
  m_emulatorScreen.create(nesVideoWidth, nesVideoHeight, m_screenScale, sf::Color::White);

  m_cycleTimer = std::chrono::high_resolution_clock::now();
  m_elapsedTime = m_cycleTimer - m_cycleTimer;

  sf::Event event;
  bool focus = true;
  bool pause = false;
  while (m_window.isOpen()) {
    while (m_window.pollEvent(event)) {
      if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
        m_window.close();
        return;
      } else if (event.type == sf::Event::GainedFocus) {
        focus = true;
        m_cycleTimer = std::chrono::high_resolution_clock::now();
      } else if (event.type == sf::Event::LostFocus) {
        focus = false;
      } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2) {
        pause = !pause;
        if (!pause) m_cycleTimer = std::chrono::high_resolution_clock::now();
      } else if (pause && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F3) {
        for (int i = 0; i < 29781; ++i) { // Around one frame
          // PPU
          m_ppu.step();
          m_ppu.step();
          m_ppu.step();
          // CPU
          m_cpu.step();
        }
      } else if (focus && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F4) {
        Log::get().setLevel(Info);
      } else if (focus && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F5) {
        Log::get().setLevel(InfoVerbose);
      } else if (focus && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F6) {
        Log::get().setLevel(CpuTrace);
      }
    }

    if (focus && !pause) {
      m_elapsedTime += std::chrono::high_resolution_clock::now() - m_cycleTimer;
      m_cycleTimer = std::chrono::high_resolution_clock::now();

      while (m_elapsedTime > m_cpuCycleDuration) {
        // PPU
        m_ppu.step();
        m_ppu.step();
        m_ppu.step();
        // CPU
        m_cpu.step();

        m_elapsedTime -= m_cpuCycleDuration;
      }

      m_window.draw(m_emulatorScreen);
      m_window.display();
    } else {
      sf::sleep(sf::milliseconds(1000 / 60));
      // std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));
      // 1/60 second
    }
  }
}

void Emulator::DMA(Data page) {
  m_cpu.skipDMACycles();
  auto page_ptr = m_bus.getPagePtr(page);
  m_ppu.doDMA(page_ptr);
}

void Emulator::setVideoHeight(int height) {
  m_screenScale = height / float(nesVideoHeight);
  LOG(Info) << "Scale: " << m_screenScale
            << " set. Screen: " << int(nesVideoWidth * m_screenScale) << "x"
            << int(nesVideoHeight * m_screenScale) << std::endl;
}

void Emulator::setVideoWidth(int width) {
  m_screenScale = width / float(nesVideoWidth);
  LOG(Info) << "Scale: " << m_screenScale
            << " set. Screen: " << int(nesVideoWidth * m_screenScale) << "x"
            << int(nesVideoHeight * m_screenScale) << std::endl;
}
void Emulator::setVideoScale(float scale) {
  m_screenScale = scale;
  LOG(Info) << "Scale: " << m_screenScale
            << " set. Screen: " << int(nesVideoWidth * m_screenScale) << "x"
            << int(nesVideoHeight * m_screenScale) << std::endl;
}

void Emulator::setKeys(std::vector<sf::Keyboard::Key>& p1, std::vector<sf::Keyboard::Key>& p2) {
  m_controller1.setKeyBindings(p1);
  m_controller2.setKeyBindings(p2);
}
