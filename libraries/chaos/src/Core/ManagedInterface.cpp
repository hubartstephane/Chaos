#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ManagedInterface::ManagedInterface() :
		manager_count(0)
	{
	}

	ManagedInterface::~ManagedInterface()
	{
		assert(manager_count == 0);
	}

	void ManagedInterface::OnRemoveFromManager()
	{
		assert(0);
	}

	void ManagedInterface::SubReference()
	{
		assert(shared_count > 0);
		if (--shared_count == 0)
		{
			if (manager_count == 0)
				OnLastReferenceLost();
			else
				OnRemoveFromManager();
		}
	}

	void ManagedInterface::AddOwnerReference()
	{
		++manager_count;
	}

	void ManagedInterface::SubOwnerReference()
	{
		assert(manager_count > 0);
		if (--manager_count == 0)
		{
			if (shared_count == 0)
				OnLastReferenceLost();
		}
	}

}; // namespace chaos
