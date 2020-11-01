#include <chaos/Chaos.h>


namespace chaos
{

	void LogTools::Log(char const * format, ...)
	{
		va_list va;
		va_start(va, format);
		VLog(format, va);
		va_end(va);
	}

	void LogTools::VLog(char const * format, va_list va)
	{
		char buffer[4096];
		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, va); // doesn't count for the zero  
		std::cout << buffer << '\n';
	}

	void LogTools::Error(char const * format, ...)
	{
		va_list va;
		va_start(va, format);
		VError(format, va);
		va_end(va);
	}

	void LogTools::VError(char const * format,va_list va)
	{
		char buffer[4096];
		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, va); // doesn't count for the zero  
		std::cerr << buffer << '\n';
	}

	void LogTools::DisplayTitle(char const * title)
	{
		assert(title != nullptr);

		size_t l = 12 + strlen(title);

		std::string s;
		for (size_t i = 0 ; i < l ; ++i)
			s += "=";

		std::cout << '\n';
		std::cout << s << '\n';
		std::cout << "===   " << title << "   ===" << '\n';
		std::cout << s << '\n';
		std::cout << '\n';
	}

}; // namespace chaos