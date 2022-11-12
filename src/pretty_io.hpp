#pragma once

#ifndef WALRUS_COMPUTE_PRETTY_IO_HPP
#define WALRUS_COMPUTE_PRETTY_IO_HPP

#include <iostream>
#include <string>
#include <vector>

namespace io {
  enum Colors {
    DEFAULT = 0,
    BLACK = 30,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    WHITE = 37
  };

  std::string color(Colors color, std::string text);

  void printExists(bool exists, std::string text, bool newLine = true);
}

#endif // WALRUS_COMPUTE_PRETTY_IO_HPP
