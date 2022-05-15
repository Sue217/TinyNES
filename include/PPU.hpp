#ifndef _PPU_H_
#define _PPU_H_

#include <vector>
#include <Cartridge.hpp>
#include <PictureBus.hpp>
#include <VirtualScreen.hpp>
#include <PaletteColors.hpp>

constexpr int ScanlineCycleLength = 341;
constexpr int ScanlineEndCycle = 340;

//^ NES image resolution is 256 * 240
constexpr int VisibleScanlines = 240;
constexpr int ScanlineVisibleDots = 256;

constexpr int FrameEndScanline = 261;

constexpr int AttributeOffset = 0x3C0;

class PPU {
 public:
  PPU(PictureBus &bus, VirtualScreen &screen);
  void step();
  void reset();
  void setInterruptCallback(std::function<void(void)> cb);

  void doDMA(const Data *page_ptr);

  // Callbacks mapped to CPU address space
  // Addresses written to by the program
  void control(Data ctrl);
  void setMask(Data mask);
  void setOAMAddress(Data addr);
  void setDataAddress(Data addr);
  void setScroll(Data scroll);
  void setData(Data data);
  // Read by the program
  Data getStatus();
  Data getData();
  Data getOAMData();
  void setOAMData(Data value);

 private:
  Data read(Address addr);
  Data readOAM(Data addr);
  void writeOAM(Data addr, Data value);
  PictureBus& m_bus;
  VirtualScreen& m_screen;

  std::function<void(void)> m_vblankCallback;

  std::vector<Data> m_spriteMemory;
  std::vector<Data> m_scanlineSprites;

  enum State {
    PreRender,
    Render,
    PostRender,
    VerticalBlank
  } m_pipelineState;

  int m_cycle;
  int m_scanline;
  bool m_evenFrame;

  bool m_vblank;
  bool m_sprZeroHit;

  // Registers
  Address m_dataAddress;
  Address m_tempAddress;
  Data m_fineXScroll;
  bool m_firstWrite;
  Data m_dataBuffer;
  Data m_spriteDataAddress;

  // Setup flags and variables
  bool m_longSprites;
  bool m_generateInterrupt;
  bool m_greyscaleMode;
  bool m_showSprites;
  bool m_showBackground;
  bool m_hideEdgeSprites;
  bool m_hideEdgeBackground;

  enum CharacterPage {
    Low,
    High,
  } m_bgPage, m_sprPage;

  Address m_dataAddrIncrement;

  std::vector<std::vector<sf::Color>> m_pictureBuffer;
};

#endif  // _PPU_H_
