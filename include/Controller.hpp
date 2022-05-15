#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <cstdint>
#include <vector>
#include <SFML/Window.hpp>

using Data = std::uint8_t;

class Controller {
 public:
  Controller();
  enum Buttons {
    A,
    B,
    Select,
    Start,
    Up,
    Down,
    Left,
    Right,
    TotalButtons,
  };

  Data read();
  void strobe(Data b);
  void setKeyBindings(const std::vector<sf::Keyboard::Key>& keys);

 private:
  bool m_strobe;
  unsigned int m_keyStates;

  std::vector<sf::Keyboard::Key> m_keyBindings;
  //         sf::Keyboard::Key m_keyBindings[TotalButtons];
};

#endif  // _CONTROLLER_H
