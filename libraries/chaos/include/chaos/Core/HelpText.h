namespace chaos
{
	namespace HelpText
	{
#ifdef CHAOS_FORWARD_DECLARATION

		/** CHAOS_HELP_TEXT : is a macro to show in console a message whenever the console is open */
#if _DEBUG
#define CHAOS_HELP_TEXT(FAMILY, x) static size_t const BOOST_PP_CAT(help_id, __LINE__) = chaos::HelpText::PushMessage(#FAMILY, x);
#else
#define CHAOS_HELP_TEXT(FAMILY, x)
#endif

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** post a message when STD is available */
		CHAOS_API size_t PushMessage(char const* family, char const* message);
		/** flush pending messages (returns number of message flushed). The callback receives (Family, Message). for each Family, it is called first with an empty message */
		CHAOS_API void OutputToLogs();

#else

	CHAOS_DEFINE_LOG(HelpLog, "Help")

#endif

	};

}; // namespace chaos