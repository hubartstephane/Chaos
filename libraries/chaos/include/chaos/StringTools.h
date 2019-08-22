#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	/**
	* StringTools : a namespace for string related functions
	*/

	namespace StringTools
	{
		/** skip numerics characters (or not) */
		char const * SkipNumber(char const * c, bool reverse_condition = false);
		/** skip non numerics characters and call atoi */
		int SkipAndAtoi(char const * c);
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

		/** returns a string representation of now  */
		std::string TimeToString(bool full_string);
		/** returns a string representation of a date */
		std::string TimeToString(std::time_t t, bool full_string);

		/** skip 0x prefix if existing */
		char const * SkipHEXPrefix(char const * c);
		/** get the value of a character in hexadeximal meaning (0-15) */
		unsigned int GetHexValueForChar(char c);
		/** parse a string into hexadeximal value (with or without 0x prefix) */    
		unsigned int AtoiH(char const * str);

		/** trim string to the left and to the right to only keep the base64 characters */
		std::string TrimBase64String(char const * src);
		/** trim string to the left and to the right */
		std::string TrimString(char const * src, bool trim_left, bool trim_right);

		/** split the input string in comma separated values */
		std::vector<std::string> Split(char const * src, char delimiter);

		/** some comparaison short cuts */
		int Strcmp(char const * src1, char const * src2);
		/** some comparaison short cuts */
		int Strcmp(char const * src1, std::string const & src2);
		/** some comparaison short cuts */
		int Strcmp(std::string const & src1, char const * src2);
		/** some comparaison short cuts */
		int Strcmp(std::string const & src1, std::string const & src2);
		/** some comparaison short cuts */
		int Stricmp(char const * src1, char const * src2);
		/** some comparaison short cuts */
		int Stricmp(char const * src1, std::string const & src2);
		/** some comparaison short cuts */
		int Stricmp(std::string const & src1, char const * src2);
		/** some comparaison short cuts */
		int Stricmp(std::string const & src1, std::string const & src2);

		/** ci_less : a case insensitive comparator */
		struct ci_less
		{
			bool operator ()(std::string const & s1, std::string const & s2) const
			{		
				return (StringTools::Stricmp(s1, s2) < 0);
			}

			bool operator ()(char const * s1, char const * s2) const
			{		
				return (StringTools::Stricmp(s1, s2) < 0);
			}
		};

		/** ci_greater : a case insensitive comparator */
		struct ci_greater
		{
			bool operator ()(std::string const & s1, std::string const & s2) const
			{		
				return (StringTools::Stricmp(s1, s2) > 0);
			}

			bool operator ()(char const * s1, char const * s2) const
			{		
				return (StringTools::Stricmp(s1, s2) > 0);
			}
		};

	}; // namespace StringTools

}; // namespace chaos
