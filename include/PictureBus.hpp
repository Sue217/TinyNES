#ifndef _PICTUREBUS_H_
#define _PICTUREBUS_H_

#include <Cartridge.hpp>
#include <Mapper.hpp>
#include <vector>

class PictureBus {
 public:
  PictureBus();
  Data read(Address addr);
  void write(Address addr, Data value);
  bool setMapper(Mapper* mapper);
  Data readPalette(Data paletteAddr);
  void updateMirroring();

 private:
  std::vector<Data> m_RAM;
  /*
   * std::size_t is the type of any sizeof expression and as is guaranteed to
   * be able to express the maximum size of any object
  */
  //^ indices where they start in RAM vector
  std::size_t NameTable0, NameTable1, NameTable2, NameTable3;
  std::vector<Data> m_palette;
  Mapper* m_mapper;
};

#endif  // _PICTUREBUS_H_