#include <iostream>
#include <string>
#include <vector>

#include "pretty_io.hpp"

namespace io {

  std::string color(Colors color, std::string text) {
    return "\033[" + std::to_string(color) + "m" + text + "\033[0m";
  }

  void printExists(bool exists, std::string text, bool newLine) {
    if (exists) { std::cout << color(Colors::GREEN, " + " + text); }
    else { std::cout << color(Colors::RED, " - " + text); }
    if (newLine) { std::cout << std::endl; }
  }

}