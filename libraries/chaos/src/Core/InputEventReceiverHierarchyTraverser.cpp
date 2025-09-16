#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputEventReceiverHierarchyTraverser::Traverse(InputEventReceiverInterface * in_event_receiver)
	{
		return in_event_receiver->TraverseInputEventReceiverHierarchy(*this);
	}

	bool InputEventReceiverHierarchyTraverser::Process(InputEventReceiverInterface * in_event_receiver)
	{
		return false; // don't stop
	}

}; // namespace chaos
