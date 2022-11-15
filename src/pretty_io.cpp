#include <iostream>
#include <string>
#include <vector>

#include "pretty_io.hpp"

namespace io {

  std::string to_color_string(Color color, std::string text) {
    return COLORS[color] + text + COLORS[Color::RESET];
  }

  std::string to_color_string(Color color, int num) {
    return to_color_string(color, std::to_string(num));
  }

  void printExists(bool exists, std::string text, bool newLine) {
    if (exists) { std::cout << COLORS[Color::GREEN]; }
    else { std::cout << COLORS[Color::RED]; }
    std::cout << " - " << text << COLORS[Color::RESET];;
    if (newLine) { std::cout << std::endl; }
  }

  void printColorTest() {
    for (unsigned int i = 0; i < COLORS.size(); i++) {
      std::cout << to_color_string(static_cast<Color>(i), "COLOR test 1 2 3") << std::endl;
    }
  }
}