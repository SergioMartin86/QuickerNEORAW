#pragma once

// Base controller class
// by eien86

#include <cstdint>
#include <jaffarCommon/exceptions.hpp>
#include <jaffarCommon/json.hpp>
#include <string>
#include <sstream>

namespace jaffar
{

struct input_t
{
  bool buttonCode = false;
  bool buttonFire = false;
  bool buttonUp = false;
  bool buttonDown = false;
  bool buttonLeft = false;
  bool buttonRight = false;
};

class InputParser
{
public:

  InputParser(const nlohmann::json &config)
  {
  }

  inline input_t parseInputString(const std::string &inputString) const
  {
    // Storage for the input
    input_t input;

    // Converting input into a stream for parsing
    std::istringstream ss(inputString);

    // Parsing controller 1 inputs
    parseInput(input, ss, inputString);

    // If its not the end of the stream, then extra values remain and its invalid
    ss.get();
    if (ss.eof() == false) reportBadInputString(inputString);

    // Returning input
    return input;
  };

  private:

  static inline void reportBadInputString(const std::string &inputString)
  {
    JAFFAR_THROW_LOGIC("Could not decode input string: '%s'\n", inputString.c_str());
  }

  static void parseInput(input_t& input, std::istringstream& ss, const std::string &inputString)
  {
    // Currently read character
    char c;

    // Cleaning code
    input.buttonCode = false;
    input.buttonFire = false;
    input.buttonUp = false;
    input.buttonDown = false;
    input.buttonLeft = false;
    input.buttonRight = false;

    // Code
    c = ss.get();
    if (c != '.' && c != 'C') reportBadInputString(inputString);
    if (c == 'C') input.buttonCode = true;

    // Up
    c = ss.get();
    if (c != '.' && c != 'U') reportBadInputString(inputString);
    if (c == 'U') input.buttonUp = true;

    // Down
    c = ss.get();
    if (c != '.' && c != 'D') reportBadInputString(inputString);
    if (c == 'D') input.buttonDown = true;

    // Left
    c = ss.get();
    if (c != '.' && c != 'L') reportBadInputString(inputString);
    if (c == 'L') input.buttonLeft = true;

    // Right
    c = ss.get();
    if (c != '.' && c != 'R') reportBadInputString(inputString);
    if (c == 'R') input.buttonRight = true;

    // Fire
    c = ss.get();
    if (c != '.' && c != 'F') reportBadInputString(inputString);
    if (c == 'F') input.buttonFire = true;
  }

}; // class Controller

} // namespace jaffar