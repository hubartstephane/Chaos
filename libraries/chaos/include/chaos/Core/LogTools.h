#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	enum class LogType;
	class Log;

}; // namespace chaos

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

	class Log
	{
	public:

		/** output a message */
		template<typename ...PARAMS>
		static void Message(PARAMS... params) 
		{
			if (Log* log = GetInstance())
				log->DoFormatAndOuput(LogType::Message, true, params...); 
		}
		/** output a warning */
		template<typename ...PARAMS>
		static void Warning(PARAMS... params) 
		{ 
			if (Log* log = GetInstance())
				log->DoFormatAndOuput(LogType::Warning, true, params...); 
		}
		/** output an error */
		template<typename ...PARAMS>
		static void Error(PARAMS... params) 
		{ 
			if (Log* log = GetInstance())
				log->DoFormatAndOuput(LogType::Error, true, params...); 
		}

		/** display a box with a text */
		static void Title(char const * title);

		/** get the Log instance */
		static Log* GetInstance();

	protected:

		/** constructor */
		Log();

	protected:

		/** internal method to format then display a log */
		void DoFormatAndOuput(LogType type, bool add_line_jump, char const* format, ...);
		/** internal method to display a log */
		void DoOutput(LogType type, bool add_line_jump, char const* buffer);
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


