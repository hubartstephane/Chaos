#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/**
 * StringTools : this is an utility class to help handling strings
 */

class StringTools
{
public:

  /** returns true whether the character is valid for a variable name */
  static bool IsVariableCharacter(char c);
  /** returns true whether the name is a standard variable name */
  static bool IsVariableName(char const * name);
  /** create a string from a format */
  static std::string Printf(char const * format, ...);
  /** create a string from a format */
  static std::string VPrintf(char const * format, va_list va);
  /** remove end line */
  static std::string RemoveEndLine(std::string src);
  /** returns a spaces made string */
  static std::string IndentString(size_t count);

};

}; // namespace chaos
