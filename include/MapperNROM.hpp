#ifndef _MAPPERNROM_H_
#define _MAPPERNROM_H_

#include <Mapper.hpp>

class MapperNROM : public Mapper {
 public:
  MapperNROM(Cartridge& cart);
  void writePRG(Address addr, Data value);
  Data readPRG(Address addr);
  const Data* getPagePtr(Address addr);

  Data readCHR(Address addr);
  void writeCHR(Address addr, Data value);

 private:
  bool m_oneBank;
  bool m_usesCharacterRAM;

  std::vector<Data> m_characterRAM;
};

#endif  // _MAPPERNROM_H_