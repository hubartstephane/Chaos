#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	enum class LogType;
	class Log;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{

	/**
	* LogType : severity of the message
	*/

	enum class LogType : int
	{
		Message,
		Warning,
		Error
	};

	/** 
	* Log : deserve to output some logs
	*/

	class Log : public Object
	{
	public:

		/** get the Log instance */
		static Log* GetInstance();

		/** output a message */
		template<typename ...PARAMS>
		static void Message(PARAMS... params) { Output(LogType::Message, true, params...); }
		/** output a warning */
		template<typename ...PARAMS>
		static void Warning(PARAMS... params) { Output(LogType::Warning, true, params...); }
		/** output an error */
		template<typename ...PARAMS>
		static void Error(PARAMS... params) { Output(LogType::Error, true, params...); }

		/** generic log function */
		template<typename ...PARAMS>
		static void Output(LogType type, bool add_line_jump, PARAMS... params)
		{ 
			if (Log* log = GetInstance())
				log->DoFormatAndOuput(type, add_line_jump, params...);
		}

		/** display a box with a text */
		static void Title(char const * title);

	protected:

		/** internal method to format then display a log */
		void DoFormatAndOuput(LogType type, bool add_line_jump, char const* format, ...);
		/** internal method to display a log */
		void DoOutput(LogType type, bool add_line_jump, char const* buffer);

	protected:

		/** an additionnal output */
		std::ofstream output_file;
		/** indicates whether we tryed once to open output_file (when opening fails, do not retry) */
		bool open_output_file = true;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


