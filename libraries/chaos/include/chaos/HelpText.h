#pragma once

#include <chaos/StandardHeaders.h>

/** CHAOS_HELP_TEXT : is a macro to show in console a message whenever the console is open */
#if _DEBUG
#define CHAOS_HELP_TEXT(x) static size_t const BOOST_PP_CAT(help_id, __LINE__) = chaos::HelpText::PushMessage(x);
#else
#define CHAOS_HELP_TEXT(x)
#endif

namespace chaos
{
	namespace HelpText
	{
		/** post a message when STD is available */
		size_t PushMessage(char const* message);
		/** flush pending messages (returns number of message flushed) */
		size_t FlushMessages(std::function<void(char const*)> function);
	};



}; // namespace chaos

