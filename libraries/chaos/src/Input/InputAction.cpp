#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	void InputAction::Process() const
	{
		if (enabled && action_function.IsValid())
			action_function();
	}

}; // namespace chaos
