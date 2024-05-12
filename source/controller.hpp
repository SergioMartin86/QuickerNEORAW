#pragma once

// Base controller class
// by eien86

#include <cstdint>
#include <string>
#include <sstream>

namespace rawspace
{

class Controller
{
public:

  struct input_t
  {
    bool buttonFire = false;
    bool buttonUp = false;
    bool buttonDown = false;
    bool buttonLeft = false;
    bool buttonRight = false;
  };

  inline bool parseInputString(const std::string& input)
  {
    // Parse valid flag
    bool isValid = true;

    // Converting input into a stream for parsing
    std::istringstream ss(input);

    // Parsing controller 1 inputs
    isValid &= parseControllerInputs(_input, ss);

    // If its not the end of the stream, then extra values remain and its invalid
    ss.get();
    if (ss.eof() == false) isValid = false;

    // Returning valid flag
    return isValid;
  };

  inline input_t getInput() { return _input; }

  private:

  static bool parseJoyPadInput(input_t& input, std::istringstream& ss)
  {
    // Currently read character
    char c;

    // Cleaning code
    input.buttonFire = false;
    input.buttonUp = false;
    input.buttonDown = false;
    input.buttonLeft = false;
    input.buttonRight = false;

    // Up
    c = ss.get();
    if (c != '.' && c != 'U') return false;
    if (c == 'U') input.buttonUp = true;

    // Down
    c = ss.get();
    if (c != '.' && c != 'D') return false;
    if (c == 'D') input.buttonDown = true;

    // Left
    c = ss.get();
    if (c != '.' && c != 'L') return false;
    if (c == 'L') input.buttonLeft = true;

    // Right
    c = ss.get();
    if (c != '.' && c != 'R') return false;
    if (c == 'R') input.buttonRight = true;

    // Fire
    c = ss.get();
    if (c != '.' && c != 'F') return false;
    if (c == 'F') input.buttonFire = true;

    return true;
  }

  static bool parseControllerInputs(input_t& input, std::istringstream& ss)
  {
    // Parse valid flag
    bool isValid = true; 
 
    // Parsing joypad code
    isValid &= parseJoyPadInput(input, ss);

    // Return valid flag
    return isValid;
  }


  input_t _input;
}; // class Controller

} // namespace quickNES