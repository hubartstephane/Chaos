#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnPollInputReceiverTraverser::OnPollInputReceiverTraverser(InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		input_device(in_input_device),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnPollInputReceiverTraverser::Process(InputReceiverInterface* in_input_receiver)
	{
		OnPollInputActionEnumerator action_enumerator(in_input_receiver, input_device, consumption_cache);
		return in_input_receiver->EnumerateInputActions(action_enumerator, EnumerateInputActionContext::OnPolling);
	}

	bool OnPollInputReceiverTraverser::Traverse(InputReceiverInterface* in_input_receiver)
	{
		return in_input_receiver->InvokeWithUpgradedInputDevice(input_device, [this, in_input_receiver](InputDeviceInterface const* in_input_device)
		{
			OnPollInputReceiverTraverser other_traverser(in_input_device, consumption_cache);
			return in_input_receiver->TraverseInputReceiver(other_traverser);
		});
	}

}; // namespace chaos
