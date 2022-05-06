#include <Mapper.hpp>
#include <MapperNROM.hpp>

nameTableMirroring Mapper::getNameTableMirroring() {
  return static_cast<nameTableMirroring> (m_cartridge.getNameTableMirroring());
}

std::unique_ptr<Mapper> Mapper::createMapper(Mapper::Type mapper_t, Cartridge& cart, std::function<void(void)> mirroring_cb) {
  std::unique_ptr<Mapper> ret(nullptr);
  switch (mapper_t) {
    case NROM:
      ret.reset(new MapperNROM(cart));
      break;
    case SxROM:
      // TODO
      break;
    case UxROM:
      // TODO
      break;
    case CNROM:
      // TODO
      break;
    default:
      break;
  }
  return ret;
}