#include "chaos/Chaos.h"


namespace chaos
{
	Log* Log::GetInstance()
	{
		// XXX : use a share pointer so that we are sure it is being destroyed at the end of the application (and so the output_file is being flushed)
		static shared_ptr<Log> result; 
		if (result == nullptr)
			result = new Log();
		return result.get();
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

		// output in standard output
		std::ostream& output = (type == LogType::Error) ? std::cerr : std::cout;
		output << buffer;
		if (add_line_jump)
			output << "\n";
		// generate output in file
		if (open_output_file && !output_file.is_open())
		{
			// even in case of failure do not ever try to open the file
			open_output_file = false; 
			// open the file
			Application* application = Application::GetInstance();
			if (application != nullptr)
			{
				boost::filesystem::path log_path = application->GetUserLocalTempPath() / "logs.txt";

				output_file.open(log_path.c_str(), std::ofstream::binary | std::ofstream::trunc);
			}
		}
		// output in file
		if (output_file.is_open())
		{
			output_file << buffer;
			if (add_line_jump)
				output_file << "\n";
		}
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
