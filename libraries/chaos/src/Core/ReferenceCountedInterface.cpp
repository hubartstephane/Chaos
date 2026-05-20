#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ReferenceCountedInterface::ReferenceCountedInterface() :
		shared_count(0)
	{
	}

	ReferenceCountedInterface::~ReferenceCountedInterface()
	{
		// reset or destroy weak structure
		if (weak_ptr_data != nullptr)
		{
			if (weak_ptr_data->weak_count == 0)
				delete weak_ptr_data;
			else
				weak_ptr_data->object_ptr = nullptr;
		}
	}

	void ReferenceCountedInterface::AddReference()
	{
		++shared_count;
	}

	void ReferenceCountedInterface::SubReference()
	{
		assert(shared_count > 0);
		if (--shared_count <= 0)
			OnLastReferenceLost();
	}

	void ReferenceCountedInterface::OnLastReferenceLost()
	{
		delete(this);
	}

}; // namespace chaos

// outside of chaos scope
void intrusive_ptr_add_ref(chaos::ReferenceCountedInterface* obj)
{
	obj->AddReference();
}

// outside of chaos scope
void intrusive_ptr_release(chaos::ReferenceCountedInterface* obj)
{
	obj->SubReference();
}
