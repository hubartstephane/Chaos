#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	DelegateInputReceiverTraverser::DelegateInputReceiverTraverser(DelegateInputReceiverTraverserFunction in_process_function) :
		process_function(in_process_function)
	{
		assert(in_process_function.IsValid());
	}

	bool DelegateInputReceiverTraverser::Process(InputReceiverInterface * in_input_receiver, InputDeviceInterface const* in_input_device)
	{
		return process_function(in_input_receiver, in_input_device);
	}

}; // namespace chaos
