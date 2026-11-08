#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ShareableInterface::ShareableInterface() :
		shared_ref_count(0)
	{
	}

	ShareableInterface::~ShareableInterface()
	{
		assert(shared_ref_count == 0);
		// reset or destroy weak structure
		if (weak_ptr_data != nullptr)
		{
			if (weak_ptr_data->weak_ref_count == 0)
				delete weak_ptr_data;
			else
				weak_ptr_data->object_ptr = nullptr;
		}
	}

	void ShareableInterface::AddReference()
	{
		++shared_ref_count;
	}

	void ShareableInterface::SubReference()
	{
		assert(shared_ref_count > 0);
		if (--shared_ref_count == 0)
			OnLastReferenceLost();
	}

	void ShareableInterface::OnLastReferenceLost()
	{
		delete(this);
	}

}; // namespace chaos

// outside of chaos scope
void intrusive_ptr_add_ref(chaos::ShareableInterface* obj)
{
	obj->AddReference();
}

// outside of chaos scope
void intrusive_ptr_release(chaos::ShareableInterface* obj)
{
	obj->SubReference();
}
