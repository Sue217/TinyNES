#ifndef _VIRTUALSCREEN_H_
#define _VIRTUALSCREEN_H_

#include <SFML/Graphics.hpp>

class VirtualScreen : public sf::Drawable {
 public:
  void create(unsigned int width, unsigned int height, float pixel_size, sf::Color color);
  void setPixel(std::size_t x, std::size_t y, sf::Color color);

 private:
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
  // unsigned vector
  sf::Vector2u m_screenSize;
  float m_pixelSize;  // virtual pixel size in real pixels
  sf::VertexArray m_vertices;
};

#endif  // _VIRTUALSCREEN_H_
