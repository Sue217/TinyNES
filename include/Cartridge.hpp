#ifndef _CARTRIDGE_H_
#define _CARTRIDGE_H_

#include <cstdint>
#include <string>
#include <vector>

/* R6502 CPU with a 16-bits Address Bus and a 8-bits Data Bus */

using Data = std::uint8_t;
using Address = std::uint16_t;

class Cartridge {
 public:
  Cartridge();
  bool loadFromFile(std::string path);
  const std::vector<Data>& getROM() const;
  const std::vector<Data>& getVROM() const;
  Data getMapper();
  Data getNameTableMirroring();
  bool hasExtendedRAM();

 private:
  std::vector<Data> m_PRG_ROM;  // store NES program data
  std::vector<Data> m_CHR_ROM;  // store NES image data
  Data m_nameTableMirroring;
  Data m_mapperNumber;
  bool m_extendedRAM;
  //^ bool m_chrRAM;
};

#endif  // _CARTRIDGE_H_