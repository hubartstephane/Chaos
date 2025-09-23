#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnKeyEventInputEventReceiverHierarchyTraverser::OnKeyEventInputEventReceiverHierarchyTraverser(KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		key_event(in_key_event),
		input_device(in_input_device),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnKeyEventInputEventReceiverHierarchyTraverser::Process(InputEventReceiverInterface* in_event_receiver)
	{
		OnKeyEventInputActionEnumerator action_enumerator(key_event, input_device, consumption_cache);
		return in_event_receiver->EnumerateInputActions(action_enumerator, EnumerateInputActionContext::OnEvent);
	}

	bool OnKeyEventInputEventReceiverHierarchyTraverser::Traverse(InputEventReceiverInterface* in_event_receiver)
	{
		return in_event_receiver->InvokeWithUpgradedInputDevice(input_device, [this, in_event_receiver](InputDeviceInterface const* in_input_device)
		{
			OnKeyEventInputEventReceiverHierarchyTraverser other_traverser(key_event, in_input_device, consumption_cache);
			return in_event_receiver->TraverseInputEventReceiverHierarchy(other_traverser);
		});
	}

}; // namespace chaos
