#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputDeviceInterface const* InputReceiverTraverser::GetInputDevice() const
	{
		return input_device;
	}

	bool InputReceiverTraverser::Traverse(InputReceiverInterface * in_input_receiver)
	{
		return in_input_receiver->InvokeWithUpgradedInputDevice(input_device, [this, in_input_receiver](InputDeviceInterface const* in_input_device)
		{
			auto input_device_guard = ScopedValueGuard(input_device, in_input_device);
			return in_input_receiver->TraverseInputReceiver(*this);
		});	
	}

	bool InputReceiverTraverser::Process(InputReceiverInterface * in_input_receiver)
	{
		return false; // don't stop
	}

}; // namespace chaos
