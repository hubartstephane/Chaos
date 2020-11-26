#include <chaos/Chaos.h>


namespace chaos
{
	Log::Log()
	{

	}

	Log* Log::GetInstance()
	{
		static Log* result = nullptr;
		if (result == nullptr)
			result = new Log();
		return result;
	}

	void Log::DoFormatAndOuput(LogType type, bool add_line_jump, char const* format, ...)
	{
		va_list va;
		va_start(va, format);

		// format the message
		char buffer[4096];
		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, va); // doesn't count for the zero  
		// output the message
		DoOutput(type, add_line_jump, buffer);

		va_end(va);
	}

	void Log::DoOutput(LogType type, bool add_line_jump, char const* buffer)
	{
		assert(buffer != nullptr);

		std::ostream& output = (type == LogType::Error) ? std::cerr : std::cout;


		output << buffer;
		if (add_line_jump)
			output << "\n";
	}

	void Log::Title(char const* title)
	{
		assert(title != nullptr);

		// get the logger
		Log* log = GetInstance();
		if (log == nullptr)
			return;

		// fill separator buffer
		char line[512] = { 0 };

		size_t l = 12 + strlen(title);
		if (l < sizeof(line) - 1)
			for (size_t i = 0; i < l; ++i)
				line[i] = '=';

		// output
		log->DoOutput(LogType::Message, true, "");
		if (l < sizeof(line) - 1)
			log->DoOutput(LogType::Message, true, line);

		log->DoOutput(LogType::Message, false, "===   ");
		log->DoOutput(LogType::Message, false, title);
		log->DoOutput(LogType::Message, true, "   ===");

		if (l < sizeof(line) - 1)
			log->DoOutput(LogType::Message, true, line);
		log->DoOutput(LogType::Message, true, "");
	}
}; // namespace chaos
