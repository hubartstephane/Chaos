#include <chaos/StringTools.h>

namespace chaos
{
	namespace StringTools
	{
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
	} // namespace StringTools

}; // namespace chaos
