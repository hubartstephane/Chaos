#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/**
 * StringTools : a namespace for string related functions
 */

namespace StringTools
{

  /** returns true whether the character is valid for a variable name */
  bool IsVariableCharacter(char c);
  /** returns true whether the name is a standard variable name */
  bool IsVariableName(char const * name);
  /** create a string from a format */
  std::string Printf(char const * format, ...);
  /** create a string from a format */
  std::string VPrintf(char const * format, va_list va);
  /** remove end line */
  std::string RemoveEndOfLines(std::string const & src);
  /** returns a spaces made string */
  std::string IndentString(size_t count);

	/** ci_less : a case insensitive comparator */
	struct ci_less
	{
		bool operator ()(std::string const & s1, std::string const & s2) const
		{		
			return (_stricmp(s1.c_str(), s2.c_str()) < 0);
		}

		bool operator ()(char const * s1, char const * s2) const
		{		
			return (_stricmp(s1, s2) < 0);
		}
	};

	/** ci_greater : a case insensitive comparator */
	struct ci_greater
	{
		bool operator ()(std::string const & s1, std::string const & s2) const
		{		
			return (_stricmp(s1.c_str(), s2.c_str()) > 0);
		}

		bool operator ()(char const * s1, char const * s2) const
		{		
			return (_stricmp(s1, s2) > 0);
		}
	};
}; // namespace StringTools

}; // namespace chaos
