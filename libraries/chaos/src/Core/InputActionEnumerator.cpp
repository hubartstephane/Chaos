#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputActionEnumerator::InputActionEnumerator(InputReceiverInterface const* in_input_receiver) :
		input_receiver(in_input_receiver)
	{
	}

	bool InputActionEnumerator::CheckAndProcess(InputRequestBase const & in_request, char const * in_title, InputActionFunction in_action_func)
	{
		return CheckAndProcess(in_request, in_title, true, in_action_func);
	}

	bool InputActionEnumerator::CheckAndProcess(InputRequestBase const & in_request, char const * in_title, bool in_enabled, InputActionFunction in_action_func)
	{
		return false;
	}

}; // namespace chaos
