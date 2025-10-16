#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputReceiverTraverser::Traverse(InputReceiverInterface * in_input_receiver, InputDeviceInterface const * in_input_device)
	{
		return in_input_receiver->InvokeWithUpgradedInputDevice(in_input_device, [this, in_input_receiver](InputDeviceInterface const* in_input_device)
		{
			return in_input_receiver->TraverseInputReceiver(*this, in_input_device);
		});
	}

	bool InputReceiverTraverser::Process(InputReceiverInterface * in_input_receiver, InputDeviceInterface const* in_input_device)
	{
		return false; // don't stop
	}

}; // namespace chaos
