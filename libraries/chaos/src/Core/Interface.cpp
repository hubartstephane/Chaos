#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	Interface::Interface() :
		shared_ref_count(0)
	{
	}

	Interface::Interface(Interface const&):
		Interface()
	{
		// do not copy reference count related members 
	}
	
	Interface::Interface(Interface&&):
		Interface()
	{
		// do not copy reference count related members 
	}

	Interface::~Interface()
	{
		assert(shared_ref_count == 0);
		// reset or destroy weak pointer control structure
		if (weak_ptr_control_block != nullptr)
			weak_ptr_control_block->OnTargetDestroyed();
	}

	Interface& Interface::operator = (Interface&&)
	{
		return *this; // do not copy reference count related members 
	}
	Interface& Interface::operator = (Interface const&)
	{
		return *this; // do not copy reference count related members 
	}

	void Interface::AddReference()
	{
		++shared_ref_count;
	}

	void Interface::SubReference()
	{
		assert(shared_ref_count > 0);
		if (--shared_ref_count == 0)
			OnLastReferenceLost();
	}

	void Interface::OnLastReferenceLost()
	{
		delete(this);
	}

	void IntrusiveAddSharedReference(Interface* obj)
	{
		obj->AddReference();
	}

	void IntrusiveSubSharedReference(Interface* obj)
	{
		obj->SubReference();
	}

}; // namespace chaos