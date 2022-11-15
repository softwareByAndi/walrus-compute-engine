#pragma once

#ifndef WALRUS_COMPUTE_PRETTY_IO_HPP
#define WALRUS_COMPUTE_PRETTY_IO_HPP

#include <iostream>
#include <string>
#include <vector>

namespace io {
  const std::string ESC = "\033[";

  enum Color {
    BLACK,
    RED,
    GREEN,
    ORANGE,
    BLUE,
    PURPLE,
    CYAN,
    LIGHT_GRAY,
    DARK_GRAY,
    LIGHT_RED,
    LIGHT_GREEN,
    YELLOW,
    LIGHT_BLUE,
    LIGHT_PURPLE,
    LIGHT_CYAN,
    WHITE,
    RESET,
  };
  const std::vector<std::string> COLORS = {
    ESC + "0;30m", // BLACK
    ESC + "0;31m", // RED
    ESC + "0;32m", // GREEN
    ESC + "0;33m", // ORANGE
    ESC + "0;34m", // BLUE
    ESC + "0;35m", // PURPLE
    ESC + "0;36m", // CYAN
    ESC + "0;37m", // LIGHT_GRAY
    ESC + "1;30m", // DARK_GRAY
    ESC + "1;31m", // LIGHT_RED
    ESC + "1;32m", // LIGHT_GREEN
    ESC + "1;33m", // YELLOW
    ESC + "1;34m", // LIGHT_BLUE
    ESC + "1;35m", // LIGHT_PURPLE
    ESC + "1;36m", // LIGHT_CYAN
    ESC + "1;37m", // WHITE
    ESC + "0m", // RESET
  };

  std::string to_color_string(Color color, std::string text);

  std::string to_color_string(Color color, int num);

  void printExists(bool exists, std::string text, bool newLine = true);
}

#endif // WALRUS_COMPUTE_PRETTY_IO_HPP
