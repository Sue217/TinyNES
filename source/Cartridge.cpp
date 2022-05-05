#include <fstream>
#include <Cartridge.hpp>
#include <Log.hpp>

const std::vector<Data>& Cartridge::getROM() const { return m_PRG_ROM; }
const std::vector<Data>& Cartridge::getVROM() const { return m_CHR_ROM; }

bool Cartridge::loadFromFile(std::string path) {
  std::ifstream romFile(path, std::ios_base::binary | std::ios_base::in);
  if (!romFile) {
    LOG(Error) << "Could not open ROM file from path: " << path << std::endl;
    return false;
  }

  LOG(Info) << "Reading ROM from path: " << path << std::endl;

  // read header in .NES file
  std::vector<Data> header;
  header.resize(0x10);

  if (!romFile.read(reinterpret_cast<char*>(&header[0]), 0x10)) {
    LOG(Error) << "Reading iNES header failed." << std::endl;
    return false;
  }

  if (std::string{&header[0], &header[4]} != "NES\x1A") {
    LOG(Error) << "Not a valid iNES image. Magic number: " << std::hex
               << header[0] << " " << header[1] << " " << header[2] << " "
               << int(header[3]) << std::endl
               << "Valid magic number : N E S 1a" << std::endl;
    return false;
  }

  LOG(Info) << "Reading header, it dictates: " << std::endl;

  Data banks = header[4];
  LOG(Info) << "16KB PRG-ROM Banks: " << +banks << std::endl;
  if (!banks) {
    LOG(Error) << "ROM has no PRG-ROM banks. Loading ROM failed." << std::endl;
    return false;
  }
  // vidio banks
  Data vbanks = header[5];
  LOG(Info) << "8KB CHR-ROM Banks: " << +vbanks << std::endl;
  // Emulator does not currently support .NES files using Trainer format
  if (header[6] & 0x4) {
    LOG(Error) << "Trainer is not supported." << std::endl;
    return false;
  }

  if ((header[0xA] & 0x3) == 0x2 || (header[0xA] & 0x1)) {
    LOG(Error) << "PAL ROM not supported." << std::endl;
    return false;
  } else {
    LOG(Info) << "ROM is NTSC compatible." << std::endl;
  }

  // PRG-ROM 16KB banks
  // write `0x4000(16KB) * banks` in `m_PGR_ROM`
  m_PRG_ROM.resize(0x4000 * banks);
  if (!romFile.read(reinterpret_cast<char*>(&m_PRG_ROM[0]), 0x4000 * banks)) {
    LOG(Error) << "Reading PRG-ROM from image file failed." << std::endl;
    return false;
  }
  for (int i = 0; i < 20; i++) {
    std::cout << std::hex << static_cast<int>(m_PRG_ROM[i]) << " ";
  }
  std::cout << std::endl;
  // CHR-ROM 8KB banks
  if (vbanks) {
    m_CHR_ROM.resize(0x2000 * vbanks);
    if (!romFile.read(reinterpret_cast<char*>(&m_CHR_ROM[0]), 0x2000 * vbanks)) {
      LOG(Error) << "Reading CHR-ROM from image file failed." << std::endl;
      return false;
    }
  } else {
    LOG(Info) << "Cartridge with CHR-RAM." << std::endl;
  }
  return true;
}