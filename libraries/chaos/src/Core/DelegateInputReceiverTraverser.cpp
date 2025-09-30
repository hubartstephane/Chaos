#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	DelegateInputReceiverTraverser::DelegateInputReceiverTraverser(DelegateInputReceiverTraverserFunction in_process_function) :
		process_function(in_process_function)
	{
		assert(in_process_function);
	}

	bool DelegateInputReceiverTraverser::Process(InputReceiverInterface * in_input_receiver)
	{
		return process_function(in_input_receiver);
	}

}; // namespace chaos
