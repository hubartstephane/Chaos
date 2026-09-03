#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	DelegateInputReceiverTraverser::DelegateInputReceiverTraverser(InputDeviceInterface const* in_input_device, DelegateInputReceiverTraverserFunction in_process_function):
		InputReceiverTraverser(in_input_device),
		process_function(in_process_function)
	{
		assert(in_process_function.IsValid());
	}

	bool DelegateInputReceiverTraverser::Process(InputReceiverInterface * in_input_receiver)
	{
		return process_function(in_input_receiver, input_device);
	}

}; // namespace chaos
