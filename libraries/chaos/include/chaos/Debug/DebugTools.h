namespace chaos
{
	namespace DebugTools
	{
#if !defined CHAOS_FORWARD_DECLARATION

		class CallStackEntry;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		class CHAOS_API CallStackEntry
		{
		public:

			/** the symbol considered */
			std::string symbol_name;
			/** the file containing the symbol */
			std::string filename;
			/** the line number on that file */
			int line_number = 0;
		};

		/** call a functor on all elements of the callstack (parameters are symbol name, filename, linenumber) */
		CHAOS_API void ProcessCallStack(std::function<void(char const*, char const*, int)> func);
		/** extract the callstack */
		CHAOS_API std::vector<CallStackEntry> ExtractCallStack();
		/** display the callstack of the current application */
		CHAOS_API void DisplayCallStack(std::ostream & stream);

#endif

	}; // namespace DebugTools

}; // namespace chaos