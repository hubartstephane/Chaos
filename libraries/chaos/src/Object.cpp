#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/SmartPointers.h>
#include <chaos/Object.h>

namespace chaos
{
	Object::Object() :
		shared_count(0)
	{
	}

	Object::~Object()
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

	void Object::AddReference()
	{
		++shared_count;
	}

	void Object::SubReference()
	{
		assert(shared_count > 0);
		if (--shared_count <= 0)
			OnLastReferenceLost();
	}

	void Object::OnLastReferenceLost()
	{
		delete(this); 		
	}

}; // namespace chaos


void intrusive_ptr_add_ref(chaos::Object* obj) // to work with boost::intrusive_ptr<>
{
	obj->AddReference();
}

/** utility method for shared_ptr */
void intrusive_ptr_release(chaos::Object* obj) // to work with boost::intrusive_ptr<>
{
	obj->SubReference();
}

