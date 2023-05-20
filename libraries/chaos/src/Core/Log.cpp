#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ================================================================
	// LogLine implementation
	// ================================================================

	static std::vector<std::pair<LogType, char const*>> const log_type_encoding =
	{
		{ LogType::Message, "Message" },
		{ LogType::Warning, "Warning" },
		{ LogType::Error,   "Error" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(LogType, log_type_encoding);

	std::string LogLine::ToString() const
	{
		return std::format("[{}] [{}] [{}]\n{}",
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
					output_file << line.ToString() << "\n\n";
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
			output_file << line.ToString() << "\n\n";
		}
	}

	void FileLoggerListener::DrawImGui(ImGuiDrawMenuMode menu_mode)
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
		// remove all domains
		for (std::string* d : domains)
			delete(d);
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

	void Logger::BeginTransaction(std::string_view domain, LogType type)
	{
		assert(!IsTransactionInProgress());
		if (transaction_count++ == 0)
		{
			transaction_type = type;
			transaction_content = {};
			transaction_domain = RegisterDomain(domain);
		}
	}

	void Logger::EndTransaction()
	{
		assert(IsTransactionInProgress());
		if (--transaction_count == 0)
		{
			DoOutput(transaction_domain, transaction_type, transaction_content);
			transaction_type = LogType::Message;
			transaction_content = {};
			transaction_domain = nullptr;
		}
	}

	bool Logger::IsTransactionInProgress() const
	{
		return (transaction_count > 0);
	}

	void Logger::DoFormatAndOutput(std::string_view domain, LogType type, char const* format, ...)
	{
		va_list va;
		va_start(va, format);
		// format the message
		char buffer[4096];
		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, va); // doesn't count for the zero  
		// output the message
		DoOutput(RegisterDomain(domain), type, buffer);
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

	void Logger::DoOutput(char const * domain, LogType type, std::string_view buffer)
	{
		// register the new line
		LogLine new_line;
		new_line.type = type;
		new_line.content = buffer;
		new_line.time = std::chrono::system_clock::now();
		new_line.domain = domain;
		lines.push_back(new_line);

		// notify all listener for this new line
		for (auto& listener : listeners)
			listener->OnNewLine(new_line);
	}

	char const* Logger::RegisterDomain(std::string_view domain)
	{
		// XXX: Lines are using raw pointer on std::string buffer for domains (so there is only a single memory allocation)
		//      I was hoping that storing domains inside a std::vector<std::string> would be nice
		//      Due to move semantic at vector reallocation, i was hoping that the raw pointers would be preserved
		//      For unknown reason, this does not work as intended. That's why i am using
		//      std::vector<std:string*>
		//                    instead of
		//      std::vector<std:string>

		// search if domain is already registered
		for (std::string const * d : domains)
			if (*d == domain)
				return d->c_str();
		// create a new string for the incoming domain
		domains.push_back(new std::string(domain));
		return domains[domains.size() - 1]->c_str();
	}

}; // namespace chaos
