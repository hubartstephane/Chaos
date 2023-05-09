namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class LogType;
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



	class CHAOS_API LogBackend : public Object
	{


	};

	class CHAOS_API LogToConsoleBackend : public LogBackend
	{

	public:



	protected:

		/** an additionnal output */
		std::ofstream output_file;
		/** indicates whether we tryed once to open output_file (when opening fails, do not retry) */
		bool open_output_file = true;
	};

	class CHAOS_API LogToImGuiBackend : public LogBackend
	{

	protected:

		/** the lines that have been displayed */
		std::vector<std::string> lines;
	};




	/**
	* Log : deserve to output some logs
	*/

	class CHAOS_API Log : public Object
	{
	public:

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
		static std::vector<LogLine> const & GetLines()
		{
			return GetInstance()->lines;
		}

		/** get the log file path */
		static boost::filesystem::path GetOutputPath();


	protected:

		/** internal method to format then display a log */
		void DoFormatAndOuput(LogType type, bool add_line_jump, char const* format, ...);
		/** internal method to display a log */
		void DoOutput(LogType type, bool add_line_jump, std::string_view buffer);

	protected:

		/** the line displayed */
		std::vector<LogLine> lines;
		/** domains. store domains only once */
		std::vector<std::string> domains;




		/** an additionnal output */
		std::ofstream output_file;
		/** indicates whether we tryed once to open output_file (when opening fails, do not retry) */
		bool open_output_file = true;
	};

#endif

}; // namespace chaos