#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnPollInputEventReceiverHierarchyTraverser::OnPollInputEventReceiverHierarchyTraverser(InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		input_device(in_input_device),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnPollInputEventReceiverHierarchyTraverser::Process(InputEventReceiverInterface* in_event_receiver)
	{
		OnPollInputActionEnumerator action_enumerator(input_device, consumption_cache);
		return in_event_receiver->EnumerateInputActions(action_enumerator, EnumerateInputActionContext::OnPolling);
	}

	bool OnPollInputEventReceiverHierarchyTraverser::Traverse(InputEventReceiverInterface* in_event_receiver)
	{
		return in_event_receiver->InvokeWithUpgradedInputDevice(input_device, [this, in_event_receiver](InputDeviceInterface const* in_input_device)
		{
			OnPollInputEventReceiverHierarchyTraverser other_traverser(in_input_device, consumption_cache);
			return in_event_receiver->TraverseInputEventReceiverHierarchy(other_traverser);
		});
	}

}; // namespace chaos
