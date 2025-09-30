#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnKeyEventInputReceiverTraverser::OnKeyEventInputReceiverTraverser(KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		key_event(in_key_event),
		input_device(in_input_device),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnKeyEventInputReceiverTraverser::Process(InputReceiverInterface* in_input_receiver)
	{
		OnKeyEventInputActionEnumerator action_enumerator(in_input_receiver, key_event, input_device, consumption_cache);
		if (in_input_receiver->EnumerateInputActions(action_enumerator, EnumerateInputActionContext::OnEvent))
		{
			// XXX: prevent the key to be handled in poll event has well
			//      this is the right place to do this because we have the proper input_device
			if (WindowApplication* window_application = Application::GetInstance())
				window_application->GetInputConsumptionCache().TryConsumeInput(key_event.key, input_device);
			return true;
		}
		return false;
	}

	bool OnKeyEventInputReceiverTraverser::Traverse(InputReceiverInterface* in_input_receiver)
	{
		return in_input_receiver->InvokeWithUpgradedInputDevice(input_device, [this, in_input_receiver](InputDeviceInterface const* in_input_device)
		{
			OnKeyEventInputReceiverTraverser other_traverser(key_event, in_input_device, consumption_cache);
			return in_input_receiver->TraverseInputReceiver(other_traverser);
		});
	}

}; // namespace chaos
