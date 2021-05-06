#ifdef CHAOS_FORWARD_DECLARATION

/** CHAOS_HELP_TEXT : is a macro to show in console a message whenever the console is open */
#if _DEBUG
#define CHAOS_HELP_TEXT(FAMILY, x) static size_t const BOOST_PP_CAT(help_id, __LINE__) = chaos::HelpText::PushMessage(#FAMILY, x);
#else
#define CHAOS_HELP_TEXT(FAMILY, x)
#endif

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{
	namespace HelpText
	{
		/** post a message when STD is available */
		size_t PushMessage(char const * family, char const* message);
		/** flush pending messages (returns number of message flushed). The callback receives (Family, Message). for each Family, it is called first with an empty message */
		size_t FlushMessages(std::function<void(char const*, char const*)> function);
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



