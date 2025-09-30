#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputReceiverTraverser::Traverse(InputReceiverInterface * in_input_receiver)
	{
		return in_input_receiver->TraverseInputReceiver(*this);
	}

	bool InputReceiverTraverser::Process(InputReceiverInterface * in_input_receiver)
	{
		return false; // don't stop
	}

}; // namespace chaos
