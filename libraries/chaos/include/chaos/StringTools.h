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
