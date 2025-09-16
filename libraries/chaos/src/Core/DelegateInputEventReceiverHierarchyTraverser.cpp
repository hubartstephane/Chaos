#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool DelegateInputEventReceiverHierarchyTraverser::Process(InputEventReceiverInterface * in_event_receiver)
	{
		return process_function(in_event_receiver);
	}

}; // namespace chaos
