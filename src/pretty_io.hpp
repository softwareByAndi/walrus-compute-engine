#pragma once

#ifndef WALRUS_COMPUTE_PRETTY_IO_HPP
#define WALRUS_COMPUTE_PRETTY_IO_HPP

#include <iostream>
#include <string>
#include <vector>

// this is an iostream wrapper that adds colors to the terminal output
// to enable easier reading / linting of console output

namespace io {
  const std::string ESC = "\033[";

  // an enum of available colors for easy selection
  enum Color {
    BLACK,
    RED,
    GREEN,
    ORANGE,
    BLUE,
    PURPLE,
    CYAN,
    GRAY,
    DARK_GRAY,
    LIGHT_RED,
    LIGHT_GREEN,
    YELLOW,
    LIGHT_BLUE,
    LIGHT_PURPLE,
    LIGHT_CYAN,
    LIGHT_GRAY,
    RESET,
  };

  // this vector maps the enum values to their actual codes.
  // using the default enum value as an index into this array.
  const std::vector<std::string> COLORS = {
    ESC + "0;30m", // BLACK
    ESC + "0;31m", // RED
    ESC + "0;32m", // GREEN
    ESC + "0;33m", // ORANGE
    ESC + "0;34m", // BLUE
    ESC + "0;35m", // PURPLE
    ESC + "0;36m", // CYAN
    ESC + "0;37m", // GRAY
    ESC + "1;30m", // DARK_GRAY
    ESC + "1;31m", // LIGHT_RED
    ESC + "1;32m", // LIGHT_GREEN
    ESC + "1;33m", // YELLOW
    ESC + "1;34m", // LIGHT_BLUE
    ESC + "1;35m", // LIGHT_PURPLE
    ESC + "1;36m", // LIGHT_CYAN
    ESC + "1;37m", // LIGHT_GRAY
    ESC + "0m", // RESET
  };

  /**
   * @brief wraps the text in terminal color codes.
   *
   * This function prepends & appends the chosen color code
   * to either side of the provided string.
   *
   * @param color The selected color to make the string -- as selected from `io::Colors` in `pretty_io.hpp`
   * @param text The text to be wrapped in colors
   *
   * @return string -- the provided text, wrapped in terminal color codes
   */
  std::string to_color_string(Color color, std::string text);

  /**
   * @description converts an integer to string and wraps it in terminal color codes
   *
   * @description This function prepends & appends the chosen color code
   * to either side of the provided integer.
   *
   * @param color The selected color to make the string -- as selected from `io::Colors` in `pretty_io.hpp`
   * @param num The integer to be wrapped in colors
   *
   * @return string -- a string of the provided integer, wrapped in terminal color codes
   */
  std::string to_color_string(Color color, int num);

  /**
   * @description colors the text in green or red and prints to terminal via `std::cout`
   *
   * @param exists <b>if true:</b> then wraps text in green and prepends ` + ` to the text\n
   * <b>if false:</b> then wraps the text in red, and prepends ` - ` to the text
   * @param text the text to wrap and print
   * @param newLine if true, prints a new line
   *
   * @return void
   *
   * @example ` + hello world`
   * @example ` - hello world`
   */
  void printExists(bool exists, std::string text, bool newLine = true);

  /**
   * @description prints all colors to to the terminal
   */
  void printColorTest();
}

#endif // WALRUS_COMPUTE_PRETTY_IO_HPP
