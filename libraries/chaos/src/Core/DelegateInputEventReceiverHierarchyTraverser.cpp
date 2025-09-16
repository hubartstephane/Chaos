#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool DelegateInputEventReceiverHierarchyTraverser::Process(InputEventReceiverInterface * event_receiver)
	{
		return process_function(event_receiver);
	}

}; // namespace chaos
