#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool DelegateInputReceiverTraverser::Process(InputReceiverInterface * in_input_receiver)
	{
		return process_function(in_input_receiver);
	}

}; // namespace chaos
