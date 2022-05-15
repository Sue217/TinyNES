#ifndef _MAPPER_H_
#define _MAPPER_H_

#include <memory>
#include <functional>
#include <Cartridge.hpp>

enum nameTableMirroring {
  Horizontal = 0,
  Vertical = 1,
  FourScreen = 8,
  OneScreenLower,
  OneScreenHigher,
};

class Mapper {
 public:
  enum Type {
    NROM = 0,
    SxROM = 1,
    UxROM = 2,
    CNROM = 3,
    // MMC3 = 4,
    // AxROM = 7,
    // ColorDreams = 11,
    // GxROM = 66,
  };

  Mapper(Cartridge& cart, Type t) : m_cartridge(cart), m_type(t) {};
  virtual void writePRG(Address addr, Data value) = 0;
  virtual Data readPRG(Address addr) = 0;
  virtual const Data* getPagePtr(Address addr) = 0;  // for DMAs

  virtual Data readCHR(Address addr) = 0;
  virtual void writeCHR(Address addr, Data value) = 0;

  virtual nameTableMirroring getNameTableMirroring();

  bool inline hasExtendedRAM() { return m_cartridge.hasExtendedRAM(); }

  static std::unique_ptr<Mapper> createMapper(
      Type mapper_t, Cartridge& cart, std::function<void(void)> mirroring_cb = nullptr);
  virtual ~Mapper() = default;

 protected:
  Cartridge& m_cartridge;
  Type m_type;
};

#endif  // _MAPPER_H_