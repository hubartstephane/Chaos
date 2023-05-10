namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class LogType;
	class LogLine;
	class LogListener;
	class FileLogListener;
	class Log;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* LogType : severity of the message
	*/

	enum class CHAOS_API LogType : int
	{
		Message,
		Warning,
		Error
	};

	CHAOS_DECLARE_ENUM_METHOD(LogType);

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

		/** the date of post */
		std::chrono::system_clock::time_point time;
		/** the type of the message */
		LogType type = LogType::Message;
		/** the domain of the message */
		char const * domain = nullptr;
		/** the content of the message */
		std::string content;
	};

	/**
	* LogListener : an object dedicated to wait for entries
	*/

	class CHAOS_API LogListener : public Object
	{
		CHAOS_DECLARE_OBJECT_CLASS(LogListener, Object);

		friend class Log;

	public:

		/** destructor */
		virtual ~LogListener();

		/** get the log system the listener is attached to */
		Log* GetLog() { return log; }
		/** get the log system the listener is attached to */
		Log const * GetLog() const { return log; }
		/** change the log system */
		void SetLog(Log* in_log);

	protected:

		/** called whenever the listener is attached to the log object */
		virtual void OnAttachedToLog(Log* log) {}
		/** called whenever the listener is detached from the log object */
		virtual void OnDetachedFromLog(Log* log) {}
		/** called whenever a new line is emitted from the log object */
		virtual void OnNewLine(LogLine const& line) {}

	protected:

		/** the log system the listener belongs to */
		Log* log = nullptr;
	};

	/**
	* FileLogListener : an object dedicated to wait for entries and write them into a file
	*/

	class CHAOS_API FileLogListener : public LogListener, public ImGuiDrawableInterface
	{
		CHAOS_DECLARE_OBJECT_CLASS(LogListener, Object);

	public:

		/** get the log file path */
		boost::filesystem::path GetOutputPath() const;
		/** force flush to file */
		void Flush() const;

	protected:

		/** override */
		virtual void OnAttachedToLog(Log* in_log) override;
		/** override */
		virtual void OnDetachedFromLog(Log* in_log) override;
		/** override */
		virtual void OnNewLine(LogLine const& line) override;

		/** override */
		virtual void DrawImGui() override;

	protected:

		/** an additionnal output (mutable so we can flush) */
		mutable std::ofstream output_file;
	};

	/**
	* Log : deserve to output some logs
	*/

	class CHAOS_API Log : public Object
	{
		CHAOS_DECLARE_OBJECT_CLASS(Log, Object);

		friend class LogListener;

	public:

		/** destructor */
		virtual ~Log();

		/** get the Log instance */
		static Log* GetInstance();

		/** output a message */
		template<typename ...PARAMS>
		static void Message(PARAMS && ...params) { Output(LogType::Message, true, std::forward<PARAMS>(params)...); }
		/** output a warning */
		template<typename ...PARAMS>
		static void Warning(PARAMS && ...params) { Output(LogType::Warning, true, std::forward<PARAMS>(params)...); }
		/** output an error */
		template<typename ...PARAMS>
		static void Error(PARAMS && ...params) { Output(LogType::Error, true, std::forward<PARAMS>(params)...); }



		/** generic log function */
		template<typename ...PARAMS>
		static void Output(LogType type, bool add_line_jump, PARAMS && ...params)
		{
			if (Log* log = GetInstance())
				log->DoFormatAndOuput(type, add_line_jump, std::forward<PARAMS>(params)...);
		}

		/** display a box with a text */
		static void Title(char const* title);

		/** get the entries */
		std::vector<LogLine> const& GetLines() const
		{
			return GetInstance()->lines;
		}

		/** get the number of listeners */
		size_t GetListenerCount() const { return listeners.size(); }
		/** get the nth listener */
		LogListener* GetListener(size_t index) { return listeners[index].get(); }
		/** get the nth listener */
		LogListener const * GetListener(size_t index) const { return listeners[index].get(); }

	protected:

		/** add a listener to the log */
		void AddListener(LogListener* listener);
		/** remove a listener from the log */
		void RemoveListener(LogListener* listener);

		/** internal method to format then display a log */
		void DoFormatAndOuput(LogType type, bool add_line_jump, char const* format, ...);
		/** internal method to display a log */
		void DoOutput(LogType type, bool add_line_jump, std::string_view buffer);

	protected:

		/** the line displayed */
		std::vector<LogLine> lines;
		/** domains. store domains only once */
		std::vector<std::string> domains;
		/** listeners */
		std::vector<shared_ptr<LogListener>> listeners;
	};

#endif

}; // namespace chaos