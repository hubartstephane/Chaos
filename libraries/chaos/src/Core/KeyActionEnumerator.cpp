#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool KeyActionEnumerator::operator () (KeyRequest const & in_request, char const * in_title, KeyActionFunction in_action_func)
	{
		return operator ()(in_request, in_title, true, in_action_func);
	}

	bool KeyActionEnumerator::operator () (KeyRequest const & in_request, char const * in_title, bool in_enabled, KeyActionFunction in_action_func)
	{
		return false;
	}

}; // namespace chaos
