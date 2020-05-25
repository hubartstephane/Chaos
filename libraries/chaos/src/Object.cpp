#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/SmartPointers.h>
#include <chaos/Object.h>

namespace chaos
{
	Object::Object() :
		shared_count(0),
		weak_count(0),
		shared_destroyed(false)
	{
	}

	Class const* Object::GetClass() const
	{
		return nullptr;
	}

	void Object::AddReference(SharedPointerPolicy policy)
	{
		assert(!shared_destroyed);
		++shared_count;
	}

	void Object::AddReference(WeakPointerPolicy policy)
	{
		++weak_count; // can add a weak reference even if the object is destroyed
	}

	void Object::SubReference(SharedPointerPolicy policy)
	{
		assert(!shared_destroyed);
		assert(shared_count > 0);
		if (--shared_count <= 0)
		{
			shared_destroyed = true;
			this->~Object(); // destroy the object content, but only release memory if weak_count is 0 too

			if (weak_count <= 0)
				OnLastReferenceLost();
		}
	}

	void Object::SubReference(WeakPointerPolicy policy)
	{
		if (--weak_count <= 0)
			if (shared_count == 0) // no more weak reference nor shared reference, release memory
				OnLastReferenceLost();
	}

	void Object::OnLastReferenceLost()
	{
		// XXX : this is different from : delete(this)
		//       delete(this)          : destruction + deallocation
		//       operator delete(this) : deallocation
		operator delete(this); 		
	}

}; // namespace chaos
