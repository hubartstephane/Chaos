#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/** 
 * LogTools : deserve to output some logs
 */

class LogTools
{
public:

  /** output an log message */
  static void Log(char const * format, ...);
  /** output an log message */
  static void VLog(char const * format, va_list va);
  /** output an error message */
  static void Error(char const * format, ...);
  /** output an error message */
  static void VError(char const * format,va_list va);

  /** display a box with a text */
  static void DisplayTitle(char const * title);
};

}; // namespace chaos

