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
		OnKeyEventInputActionEnumerator action_enumerator(in_event_receiver, key_event, input_device, consumption_cache);
		if (in_event_receiver->EnumerateInputActions(action_enumerator, EnumerateInputActionContext::OnEvent))
		{
			// XXX: prevent the key to be handled in poll event has well
			//      this is the right place to do this because we have the proper input_device
			if (WindowApplication* window_application = Application::GetInstance())
				window_application->GetInputConsumptionCache().TryConsumeInput(key_event.key, input_device);
			return true;
		}
		return false;
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
