#pragma once

#include <chaos/StandardHeaders.h>

/** CHAOS_HELP_TEXT : is a macro to show in console a message whenever the console is open */
#if _DEBUG
#define CHAOS_HELP_TEXT(x) static int const BOOST_PP_CAT(help_id, __LINE__) = chaos::HelpText(x);
#else
#define CHAOS_HELP_TEXT(x)
#endif

namespace chaos
{
	/** post a message when STD is available */
	int HelpText(char const* message);

}; // namespace chaos

