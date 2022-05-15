#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <chrono>
#include <CPU.hpp>
#include <Cartridge.hpp>
#include <MainBus.hpp>
#include <Mapper.hpp>
#include <SFML/Graphics.hpp>
#include <VirtualScreen.hpp>
#include <PPU.hpp>
#include <PictureBus.hpp>
#include <Controller.hpp>

constexpr int nesVideoWidth = ScanlineVisibleDots;
constexpr int nesVideoHeight = VisibleScanlines;

class Emulator {
 public:
  Emulator();
  void run(std::string rom_path);
  void setVideoWidth(int width);
  void setVideoHeight(int height);
  void setVideoScale(float scale);
  void setKeys(std::vector<sf::Keyboard::Key>& p1, std::vector<sf::Keyboard::Key>& p2);

 private:
  void DMA(Data page);
  CPU m_cpu;
  PPU m_ppu;
  MainBus m_bus;
  Cartridge m_cartridge;

  PictureBus m_pictureBus;
  sf::RenderWindow m_window;
  VirtualScreen m_emulatorScreen;
  std::unique_ptr<Mapper> m_mapper;

  Controller m_controller1, m_controller2;

  // screen zoom
  float m_screenScale;
  // time
  std::chrono::high_resolution_clock::time_point m_cycleTimer;
  std::chrono::high_resolution_clock::duration m_elapsedTime;
  std::chrono::nanoseconds m_cpuCycleDuration;
};

#endif  // _EMULATOR_H_