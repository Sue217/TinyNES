#include <iostream>
#include <CPU.hpp>
#include <Cartridge.hpp>
#include <MainBus.hpp>

int main() {
  Cartridge cartridge;
  cartridge.loadFromFile("./example/example.nes");
  MainBus bus(cartridge);
  CPU test(bus);

  test.reset();
  std::cout << "[+]After reset, PC locates: 0x" << std::hex << test.get_pc() << std::endl;

  // CPU fetching and decoding instructions
  int cycle = 4;
  while (cycle--) {
    test.step();
  }
  Data value = test.get_acc();
  //! Type Conversion
  std::cout << "[+]After executing \"4+2\" operation, the value of ACC is: "
            << static_cast<int>(value) << std::endl;
  return 0;
}