#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/SmartPointers.h>

namespace chaos
{

	/**
	* ReferencedObject is a base class that have a reference count (shared and weak)
	*/

	class ReferencedObject
	{
		friend class ReferencedObjectFriend;

	public:

		/** constructor */
		ReferencedObject() :
			shared_count(0),
			weak_count(0),
			shared_destroyed(false)
		{
		}

		/** destructor */
		virtual ~ReferencedObject() = default;

		/** utility method for shared_ptr */
		template<typename POLICY>
		friend inline void intrusive_ptr_add_ref(ReferencedObject * obj, POLICY policy = SharedPointerPolicy()) // to work with boost::intrusive_ptr<>
		{
			obj->AddReference(policy);
		}

		/** utility method for shared_ptr */
		template<typename POLICY>
		friend inline void intrusive_ptr_release(ReferencedObject * obj, POLICY policy = SharedPointerPolicy()) // to work with boost::intrusive_ptr<>
		{
			obj->SubReference(policy);
		}

	protected:

		/** adding a reference */
		virtual void AddReference(SharedPointerPolicy policy)
		{
			assert(!shared_destroyed);
			++shared_count;
		}

		/** adding a weak reference */
		virtual void AddReference(WeakPointerPolicy policy)
		{
			++weak_count; // can add a weak reference even if the object is destroyed
		}

		/** removing a reference */
		virtual void SubReference(SharedPointerPolicy policy)
		{
			assert(!shared_destroyed);
			if (--shared_count <= 0)
			{
				shared_destroyed = true;
				this->~ReferencedObject(); // destroy the object content, but only release memory if weak_count is 0 too
				if (weak_count <= 0)
					OnLastReferenceLost();
			}
		}

		/** removing a weak reference */
		virtual void SubReference(WeakPointerPolicy policy)
		{
			if (--weak_count <= 0)
				if (shared_destroyed) // no more weak reference nor shared reference, release memory
					OnLastReferenceLost();
		}

		/** called whenever there are no more reference on the object */
		virtual void OnLastReferenceLost()
		{
			free(this);
		}

	protected:

		/** count shared reference */
		boost::atomic<int> shared_count;
		/** count weak reference */
		boost::atomic<int> weak_count;
		/** whenever the content has been destroyed */
		bool shared_destroyed = false;
	};

	/**
	* DisableLastReferenceLost : an utility class to help using referenced object on stack
	*/

	template<typename T>
	class DisableLastReferenceLost : public T
	{
	protected:
		/** disable the reference lost deletion */
		virtual void OnLastReferenceLost() override { }
	};

}; // namespace chaos

