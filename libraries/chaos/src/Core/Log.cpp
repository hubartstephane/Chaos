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
		return std::format("{} {} {} {}",
			StringTools::TimeToString(time, TimeToStringFormatType::FULL),
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
	// LoggerListener implementation
	// ================================================================

	LoggerListener::~LoggerListener()
	{
		SetLogger(nullptr); // remove listener from log
	}

	void LoggerListener::SetLogger(Logger* in_logger)
	{
		if (in_logger != logger)
		{
			if (logger != nullptr)
			{
				logger->RemoveListener(this);
			}
			if (in_logger != nullptr)
			{
				in_logger->AddListener(this);
			}
		}
	}

	// ================================================================
	// FileLoggerListener implementation
	// ================================================================

	boost::filesystem::path FileLoggerListener::GetOutputPath() const
	{
		if (Application* application = Application::GetInstance())
		{
			return application->GetUserLocalTempPath() / "logs.txt";
		}
		return {};
	}

	void FileLoggerListener::Flush()
	{
		if (output_file.is_open())
		{
			output_file.flush();
		}
	}

	void FileLoggerListener::OnAttachedToLogger(Logger* in_logger)
	{
		boost::filesystem::path log_path = GetOutputPath();
		if (!log_path.empty())
		{
			output_file.open(log_path.c_str(), std::ofstream::binary | std::ofstream::trunc);
			if (output_file.is_open())
			{
				for (LogLine const& line : in_logger->GetLines())
				{
					output_file << line.ToString() << "\n";
				}
			}
		}
	}
	
	void FileLoggerListener::OnDetachedFromLogger(Logger* in_logger)
	{
		output_file.close();
	}
	
	void FileLoggerListener::OnNewLine(LogLine const& line)
	{
		if (output_file.is_open())
		{
			output_file << line.ToString() << "\n";
		}
	}

	void FileLoggerListener::DrawImGui()
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

	Logger::~Logger()
	{
		// remove all listeners
		while (listeners.size() > 0)
		{
			listeners[0]->SetLogger(nullptr);
		}
	}

	Logger* Logger::GetInstance()
	{
		// XXX : use a share pointer so that we are sure it is being destroyed at the end of the application (and so the output_file is being flushed)
		static shared_ptr<Logger> result; 
		if (result == nullptr)
			result = new Logger();
		return result.get();
	}

	void Logger::AddListener(LoggerListener* listener)
	{
		assert(listener != nullptr);
		assert(listener->logger == nullptr);
		listener->logger = this;
		listeners.push_back(listener);
		listener->OnAttachedToLogger(this);
	}

	void Logger::RemoveListener(LoggerListener* listener)
	{
		assert(listener != nullptr);
		assert(listener->logger == this);

		shared_ptr<LoggerListener> prevent_destruction = listener;
		listener->logger = nullptr;
		listeners.erase(std::find(listeners.begin(), listeners.end(), listener));
		listener->OnDetachedFromLogger(this);
	}

	void Logger::BeginTransaction(LogType type)
	{
		assert(!IsTransactionInProgress());
		if (transaction_count++ == 0)
		{
			transaction_type = type;
			transaction_content = {};
		}
	}

	void Logger::EndTransaction()
	{
		assert(IsTransactionInProgress());
		if (--transaction_count == 0)
		{
			DoOutput(transaction_type, transaction_content);
			transaction_type = LogType::Message;
			transaction_content = {};
		}
	}

	bool Logger::IsTransactionInProgress() const
	{
		return (transaction_count > 0);
	}

	void Logger::DoFormatAndOutput(LogType type, char const* format, ...)
	{
		va_list va;
		va_start(va, format);
		// format the message
		char buffer[4096];
		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, va); // doesn't count for the zero  
		// output the message
		DoOutput(type, buffer);
		va_end(va);
	}

	void Logger::DoFormatAndConcatToTransaction(char const* format, ...)
	{
		va_list va;
		va_start(va, format);
		// format the message
		char buffer[4096];
		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, va); // doesn't count for the zero  
		// output the message
		transaction_content.append(buffer);
		va_end(va);
	}

	void Logger::DoOutput(LogType type, std::string_view buffer)
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


#if 0
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
		log->DoOutput(LogType::Message, "");
		if (l < sizeof(line) - 1)
			log->DoOutput(LogType::Message, line);

		log->DoOutput(LogType::Message, "===   ");
		log->DoOutput(LogType::Message, title);
		log->DoOutput(LogType::Message, "   ===");

		if (l < sizeof(line) - 1)
			log->DoOutput(LogType::Message, line);
		log->DoOutput(LogType::Message, "");
	}

#endif



}; // namespace chaos
