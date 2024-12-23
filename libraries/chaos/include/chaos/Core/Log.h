namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class LogSeverity;
	class LogLine;
	class LoggerListener;
	class FileLoggerListener;
	class Logger;

	template<char const* &LOG_DOMAIN>
	class LogBase;

	class Log;

	/** CHAOS_DEFINE_LOG: an utility macro to generate some Log class */
#define CHAOS_DEFINE_LOG(LogClass, Domain)\
	static const char* LogClass##_DomainStr = Domain;\
	class LogClass : public LogBase<LogClass##_DomainStr> {};

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* LogSeverity: severity of the message
	*/

	enum class CHAOS_API LogSeverity : int
	{
		Message,
		Warning,
		Error
	};

	CHAOS_DECLARE_ENUM_METHOD(LogSeverity, CHAOS_API);

	/**
	* LogLine : an entry in the log system
	*/

	class CHAOS_API LogLine
	{
	public:

		/** output to string */
		std::string ToString() const;
		/** check whether two entries are comparable */
		bool IsComparable(LogLine const& src) const;

	public:

		/** index of the line for the logger */
		size_t line_index = 0;
		/** the date of post */
		std::chrono::system_clock::time_point time;
		/** the severity of the message */
		LogSeverity severity = LogSeverity::Message;
		/** the domain of the message */
		char const * domain = nullptr;
		/** the content of the message */
		std::string content;
	};

	/**
	* LoggerListener : an object dedicated to wait for entries
	*/

	class CHAOS_API LoggerListener : public Object
	{
		CHAOS_DECLARE_OBJECT_CLASS(LoggerListener, Object);

		friend class Logger;

	public:

		/** destructor */
		virtual ~LoggerListener();

		/** get the log system the listener is attached to */
		Logger* GetLogger() { return logger; }
		/** get the log system the listener is attached to */
		Logger const * GetLogger() const { return logger; }
		/** change the log system */
		void SetLogger(Logger* in_logger);

		/** plug some ImGui items inside ImGui menu */
		virtual void DrawImGuiMenu() {}

	protected:

		/** called whenever the listener is attached to the log object */
		virtual void OnAttachedToLogger(Logger* in_logger) {}
		/** called whenever the listener is detached from the log object */
		virtual void OnDetachedFromLogger(Logger* in_logger) {}
		/** called whenever a new line is emitted from the log object */
		virtual void OnNewLine(LogLine const& line) {}

	protected:

		/** the log system the listener belongs to */
		Logger* logger = nullptr;
	};

	/**
	* FileLoggerListener : an object dedicated to wait for entries and write them into a file
	*/

	class CHAOS_API FileLoggerListener : public LoggerListener
	{
		CHAOS_DECLARE_OBJECT_CLASS(FileLoggerListener, LoggerListener);

	public:

		/** get the log file path */
		boost::filesystem::path GetOutputPath() const;
		/** force flush to file */
		void Flush();

	protected:

		/** override */
		virtual void OnAttachedToLogger(Logger* in_logger) override;
		/** override */
		virtual void OnDetachedFromLogger(Logger* in_logger) override;
		/** override */
		virtual void OnNewLine(LogLine const& line) override;
		/** override */
		virtual void DrawImGuiMenu() override;

	protected:

		/** an additionnal output */
		std::ofstream output_file;
		/** last line index handled */
		std::optional<size_t> last_line_index_handled;
	};

	/**
	* Logger : deserve to output some logs
	*/

	class CHAOS_API Logger : public Object
	{
		CHAOS_DECLARE_OBJECT_CLASS(Logger, Object);

		friend class LoggerListener;

	public:

		/** destructor */
		virtual ~Logger();

		/** get the Log instance */
		static Logger* GetInstance();

		/** output a message */
		template<typename ...PARAMS>
		void Message(std::string_view domain, PARAMS && ...params)
		{
			Output(domain, LogSeverity::Message, std::forward<PARAMS>(params)...);
		}
		/** output a warning */
		template<typename ...PARAMS>
		void Warning(std::string_view domain, PARAMS && ...params)
		{
			Output(domain, LogSeverity::Warning, std::forward<PARAMS>(params)...);
		}
		/** output an error */
		template<typename ...PARAMS>
		void Error(std::string_view domain, PARAMS && ...params)
		{
			Output(domain, LogSeverity::Error, std::forward<PARAMS>(params)...);
		}

		/** generic log function (non static function) */
		template<typename ...PARAMS>
		void Output(std::string_view domain, LogSeverity severity, PARAMS && ...params)
		{
			DoFormatAndOutput(domain, severity, std::forward<PARAMS>(params)...);
		}

		/** start a new line of log, but wait until transaction ends to emit the line */
		void BeginTransaction(std::string_view domain, LogSeverity severity);
		/** add some text to the current transaction */
		template<typename ...PARAMS>
		void TransactionConcat(PARAMS && ...params)
		{
			DoFormatAndConcatToTransaction(std::forward<PARAMS>(params)...);
		}
		template<typename ...PARAMS>
		void TransactionConcatLN(PARAMS && ...params)
		{
			DoFormatAndConcatToTransaction(std::forward<PARAMS>(params)...);
			transaction_content.push_back('\n');
		}
		/** end the transaction */
		void EndTransaction();
		/** whether a transaction is started */
		bool IsTransactionInProgress() const;

		/** get the entries */
		std::vector<LogLine> const& GetLines() const
		{
			return GetInstance()->lines;
		}

		/** get the number of listeners */
		size_t GetListenerCount() const { return listeners.size(); }
		/** get the nth listener */
		LoggerListener* GetListener(size_t index) { return listeners[index].get(); }
		/** get the nth listener */
		LoggerListener const* GetListener(size_t index) const { return listeners[index].get(); }

		/** get the number of domains */
		size_t GetDomainCount() const { return domains.size(); }
		/** get a given domain */
		char const * GetDomain(size_t index) const { return domains[index]->c_str(); }

	protected:

		/** add a listener to the log */
		void AddListener(LoggerListener* listener);
		/** remove a listener from the log */
		void RemoveListener(LoggerListener* listener);

		/** internal method to format then display a log */
		void DoFormatAndOutput(std::string_view domain, LogSeverity severity, char const* format, ...);
		/** format a string and concat to the transaction in progress */
		void DoFormatAndConcatToTransaction(char const* format, ...);

		/** internal method to display a log */
		virtual void DoOutput(char const * domain, LogSeverity severity, std::string_view buffer);

		/** register a string for domain and gets its internal pointer */
		char const* RegisterDomain(std::string_view domain);

	protected:

		/** the line displayed */
		std::vector<LogLine> lines;
		/** domains. store domains only once */
		std::vector<std::string *> domains;
		/** listeners */
		std::vector<shared_ptr<LoggerListener>> listeners;

		/** whether a transaction is being started */
		int transaction_count = 0;
		/** the transaction information */
		LogSeverity transaction_severity = LogSeverity::Message;
		/** the transaction domain */
		char const* transaction_domain = nullptr;
		/** the transaction information */
		std::string transaction_content;
		/** the index to give to the next line */
		size_t next_line_index = 0;
	};

	/**
	* LogBase: Base class for an utility class that gets the singleton logger and push data into it
	*/

	template<char const*& LOG_DOMAIN>
	class LogBase
	{
	public:

		/** output a message */
		template<typename ...PARAMS>
		static void Message(PARAMS && ...params)
		{
			Output(LogSeverity::Message, std::forward<PARAMS>(params)...);
		}
		/** output a warning */
		template<typename ...PARAMS>
		static void Warning(PARAMS && ...params)
		{
			Output(LogSeverity::Warning, std::forward<PARAMS>(params)...);
		}
		/** output an error */
		template<typename ...PARAMS>
		static void Error(PARAMS && ...params)
		{
			Output(LogSeverity::Error, std::forward<PARAMS>(params)...);
		}

		/** generic log function */
		template<typename ...PARAMS>
		static void Output(LogSeverity severity, PARAMS && ...params)
		{
			if (Logger* logger = Logger::GetInstance())
				logger->Output(GetDomain(), severity, std::forward<PARAMS>(params)...);
		}

		/** start a new line of log, but wait until transaction ends to emit the line */
		static void BeginTransaction(LogSeverity severity)
		{
			if (Logger* logger = Logger::GetInstance())
				logger->BeginTransaction(GetDomain(), severity);
		}
		/** add some text to the current transaction */
		template<typename ...PARAMS>
		static void TransactionConcat(PARAMS && ...params)
		{
			if (Logger* logger = Logger::GetInstance())
				logger->TransactionConcat(std::forward<PARAMS>(params)...);
		}
		/** add some text to the current transaction */
		template<typename ...PARAMS>
		static void TransactionConcatLN(PARAMS && ...params)
		{
			if (Logger* logger = Logger::GetInstance())
				logger->TransactionConcatLN(std::forward<PARAMS>(params)...);
		}
		/** end the transaction */
		static void EndTransaction()
		{
			if (Logger* logger = Logger::GetInstance())
				logger->EndTransaction();
		}

		/** whether a transaction is started (static method) */
		static bool IsTransactionInProgress()
		{
			if (Logger* logger = Logger::GetInstance())
				return logger->IsTransactionInProgress();
			return false;
		}

		/** the domain the logs belong to */
		static char const* GetDomain()
		{
			return LOG_DOMAIN;
		}
	};

	/**
	* Log: the default log definition
	*/

CHAOS_DEFINE_LOG(Log, "Default")

#endif

}; // namespace chaos