#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ================================================================
	// LogLine implementation
	// ================================================================

	static std::vector<std::pair<LogType, char const*>> const log_type_encoding =
	{
		{ LogType::Message, "MESSAGE" },
		{ LogType::Warning, "WARNING" },
		{ LogType::Error,   "ERROR" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(LogType, log_type_encoding);

	std::string LogLine::ToString() const
	{
		return std::format("{0} {1} {2}",
			EnumToString(type),
			(domain != nullptr) ? domain : "",
			content.c_str());
	}

	bool LogLine::IsComparable(LogLine const& src) const
	{
		// compare members to members, by comparaison speed (ignore time)
		return
			(type == src.type) &&
			(domain == src.domain) && // raw pointer comparaison
			(content == src.content);
	}

	// ================================================================
	// LogListener implementation
	// ================================================================

	LogListener::~LogListener()
	{
		SetLog(nullptr); // remove listener from log
	}

	void LogListener::SetLog(Log* in_log)
	{
		if (in_log != log)
		{
			if (log != nullptr)
			{
				log->RemoveListener(this);
			}
			if (in_log != nullptr)
			{
				in_log->AddListener(this);
			}
		}
	}

	// ================================================================
	// FileLogListener implementation
	// ================================================================

	boost::filesystem::path FileLogListener::GetOutputPath() const
	{
		if (Application* application = Application::GetInstance())
		{
			return application->GetUserLocalTempPath() / "logs.txt";
		}
		return {};
	}

	void FileLogListener::Flush() const
	{
		if (output_file.is_open())
		{
			output_file.flush();
		}
	}

	void FileLogListener::OnAttachedToLog(Log* in_log)
	{
		boost::filesystem::path log_path = GetOutputPath();
		if (!log_path.empty())
		{
			output_file.open(log_path.c_str(), std::ofstream::binary | std::ofstream::trunc);
			if (output_file.is_open())
			{
				for (LogLine const& line : in_log->GetLines())
				{
					output_file << line.ToString() << "\n";
				}
			}
		}
	}
	
	void FileLogListener::OnDetachedFromLog(Log* in_log)
	{
		output_file.close();
	}
	
	void FileLogListener::OnNewLine(LogLine const& line)
	{
		if (output_file.is_open())
		{
			output_file << line.ToString() << "\n";
		}
	}

	void FileLogListener::DrawImGui()
	{
		if (ImGui::Button("Show output file"))
		{
			boost::filesystem::path log_path = GetOutputPath();
			if (!log_path.empty())
			{
				Flush();
				chaos::WinTools::ShowFile(log_path);
			}
		}
	}

	// ================================================================
	// Log implementation
	// ================================================================

	Log::~Log()
	{
		// remove all listeners
		while (listeners.size() > 0)
		{
			listeners[0]->SetLog(nullptr);
		}
	}

	Log* Log::GetInstance()
	{
		// XXX : use a share pointer so that we are sure it is being destroyed at the end of the application (and so the output_file is being flushed)
		static shared_ptr<Log> result; 
		if (result == nullptr)
			result = new Log();
		return result.get();
	}

	void Log::AddListener(LogListener* listener)
	{
		assert(listener != nullptr);
		assert(listener->log == nullptr);
		listener->log = this;
		listeners.push_back(listener);
		listener->OnAttachedToLog(this);
	}

	void Log::RemoveListener(LogListener* listener)
	{
		assert(listener != nullptr);
		assert(listener->log == this);

		shared_ptr<LogListener> prevent_destruction = listener;
		listener->log = nullptr;
		listeners.erase(std::find(listeners.begin(), listeners.end(), listener));
		listener->OnDetachedFromLog(this);
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

	void Log::DoOutput(LogType type, bool add_line_jump, std::string_view buffer)
	{


		// register the new line
		LogLine new_line;
		new_line.type = type;
		new_line.content = buffer;
		new_line.time = std::chrono::system_clock::now();
		lines.push_back(new_line);

		// notify all listener for this new line
		for (auto& listener : listeners)
			listener->OnNewLine(new_line);


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
