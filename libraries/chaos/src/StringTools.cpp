#include <chaos/StringTools.h>
#include <chaos/MyBase64.h>

namespace chaos
{
	namespace StringTools
	{
		std::string TimeToString(bool full_string)
		{
			return TimeToString(std::time(0), full_string);
		}

		std::string TimeToString(std::time_t t, bool full_string)
		{
			// decompose time
			struct tm tm;
			localtime_s(&tm, &t);
			// format string
			return chaos::StringTools::Printf(
				(full_string)? "%04d%02d%02d_%02dh%02dm%02ds" : "%04d%02d%02d",
				tm.tm_year + 1900, 
				tm.tm_mon + 1, 
				tm.tm_mday, 
				tm.tm_hour, 
				tm.tm_min, 
				tm.tm_sec);
		}

		char const * SkipNumber(char const * c, bool reverse_condition)
		{
			while (*c != 0)
			{
				if (!std::isdigit(*c) ^ reverse_condition) // all characters already skipped
					break;
				++c;
			}
			return c;
		}

		int SkipAndAtoi(char const * c)
		{
			while (*c != 0 && !std::isdigit(*c))
				++c;
			return atoi(c);
		}

		bool IsVariableCharacter(char c)
		{
			return (std::isalnum(c) || c == '_');
		}

		bool IsVariableName(char const * name, bool strict) 
		{
			assert(name != nullptr);

			// an empty string is not a variable name !!
			if (name[0] == 0)
				return false;
			// accept or not names starting with a number ?
			int start_character = 0;
			if (strict)
			{
				if (!std::isalpha(name[0]) && name[0] != '_') 
					return false;
				start_character = 1;
			}
			// other characters
			for (int i = start_character ; name[i] != 0 ; ++i)
				if (!std::isalnum(name[i]) && name[i] != '_')
					return false;
			return true;
		}

		std::string Printf(char const * format, ...)
		{
			assert(format != nullptr);
			std::string result;
			va_list va;
			va_start(va, format);
			result = VPrintf(format, va);
			va_end(va);
			return result;
		}

		std::string VPrintf(char const * format, va_list va) // well, this function needs to have a smarted behavior for too long output
		{  
			assert(format != nullptr);
			char buffer[4096];
			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, va); // doesn't count for the zero  
			return buffer;
		}

		std::string RemoveEndOfLines(std::string const & src)
		{
			size_t len = src.length();
			while (len > 0)
			{
				char c = src[len - 1];
				if (c != '\n' && c != '\r')
					break;
				--len;
			}
			return std::string(src.c_str(), len);
		}

		std::string IndentString(size_t count)
		{
			return std::string().insert(0, count, ' ');
		}	

		char const * SkipHEXPrefix(char const * c)
		{
			if (c[0] == '0' && (c[1] == 'x' || c[1] == 'X')) // skip hexadecimal prefix if any
				c += 2;
			return c;
		}

		unsigned int GetHexValueForChar(char c)
		{
			if (c >= '0' && c <= '9')
				return (c - '0');
			if (c >= 'a' && c <= 'f')
				return (c - 'a') + 10;
			if (c >= 'A' && c <= 'F')
				return (c - 'A') + 10;
			return std::numeric_limits<unsigned int>::max();
		}

		unsigned int AtoiH(char const * str)
		{
			unsigned int result = 0;

			str = SkipHEXPrefix(str);

			int i = 0;
			while (str[i] != 0)
			{
				unsigned int letter_value = GetHexValueForChar(str[i]);
				if (letter_value >= 16)
					break;
				result = (result << 4) | letter_value;
				++i;
			}
			return result;
		}

		std::string TrimBase64String(char const * src)
		{
			assert(src != nullptr);

			// escape the non 64 characters and extract the buffer to read
			int i = 0;
			while (src[i] != 0 && !MyBase64::IsBase64(src[i]))
				++i;
			int j = i;
			while (src[j] != 0 && MyBase64::IsBase64(src[j]))
				++j;

			return std::string(&src[i], j - i);
		}

		std::string TrimString(char const * src, bool trim_left, bool trim_right)
		{
			assert(src != nullptr);

			int i = 0;
			if (trim_left)
				while (src[i] != 0 && std::isspace(src[i]))
					++i;
			int j = strlen(src) - 1;
			if (trim_right)
				while (j > i && std::isspace(src[j]))
					--j;

			return std::string(&src[i], j - i + 1);
		}

		std::vector<std::string> Split(char const * src, char delimiter)
		{
			std::vector<std::string> result;
			if (src == nullptr)
				return result;

			while (*src != 0)
			{
				char const * separator = strchr(src, delimiter);
				if (separator == nullptr)
				{
					result.push_back(src); // last string
					break;
				}
				else
				{
					result.push_back(std::string(src, separator)); // last string
					src = separator + 1;				
				}			
			}

			//boost::split(result, src, [](char c){return c == ';';});		 // only works with std::string as src
			return result;
		}

		int Strcmp(char const * src1, char const * src2)
		{
			return strcmp(src1, src2);
		}
		int Strcmp(char const * src1, std::string const & src2)
		{
			return Strcmp(src1, src2.c_str());
		}
		int Strcmp(std::string const & src1, char const * src2)
		{
			return Strcmp(src1.c_str(), src2);
		}
		int Strcmp(std::string const & src1, std::string const & src2)
		{
			return Strcmp(src1.c_str(), src2.c_str());
		}

		int Stricmp(char const * src1, char const * src2)
		{
			// special cases
			if (src1 == nullptr)
				src1 = "";
			if (src2 == nullptr)
				src2 = "";
			// standard
			return _strcmpi(src1, src2);
		}
		int Stricmp(char const * src1, std::string const & src2)
		{
			return Stricmp(src1, src2.c_str());
		}
		int Stricmp(std::string const & src1, char const * src2)
		{
			return Stricmp(src1.c_str(), src2);
		}
		int Stricmp(std::string const & src1, std::string const & src2)
		{
			return Stricmp(src1.c_str(), src2.c_str());
		}

		int Strncmp(char const * src1, char const * src2, size_t count)
		{
			return strncmp(src1, src2, count);
		}
		int Strncmp(char const * src1, std::string const & src2, size_t count)
		{
			return Strncmp(src1, src2.c_str(), count);
		}
		int Strncmp(std::string const & src1, char const * src2, size_t count)
		{
			return Strncmp(src1.c_str(), src2, count);
		}
		int Strncmp(std::string const & src1, std::string const & src2, size_t count)
		{
			return Strncmp(src1.c_str(), src2.c_str(), count);
		}

		int Strnicmp(char const * src1, char const * src2, size_t count)
		{
			return _strnicmp(src1, src2, count);
		}
		int Strnicmp(char const * src1, std::string const & src2, size_t count)
		{
			return Strnicmp(src1, src2.c_str(), count);
		}
		int Strnicmp(std::string const & src1, char const * src2, size_t count)
		{
			return Strnicmp(src1.c_str(), src2, count);
		}
		int Strnicmp(std::string const & src1, std::string const & src2, size_t count)
		{
			return Strnicmp(src1.c_str(), src2.c_str(), count);
		}

		bool IsEmpty(char const * src)
		{
			return (src == nullptr) || (src[0] == 0);
		}
		bool IsEmpty(std::string const & src)
		{
			return IsEmpty(src.c_str());
		}

	} // namespace StringTools

}; // namespace chaos
