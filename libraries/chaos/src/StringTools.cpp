#include <chaos/StringTools.h>
#include <chaos/MyBase64.h>

namespace chaos
{
	namespace StringTools
	{
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

		bool IsVariableName(char const * name)
		{
			assert(name != nullptr);
			if (!std::isalpha(name[0])) // an empty string is not a variable name !!
				return false;
			for (int i = 1 ; name[i] != 0 ; ++i)
				if (!std::isalnum(name[i]) || name[i] == '_')
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

	} // namespace StringTools

}; // namespace chaos
