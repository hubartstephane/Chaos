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
		ReferencedObject();

		/** destructor */
		virtual ~ReferencedObject() = default;

	public:

		/** adding a shared reference */
		virtual void AddReference(SharedPointerPolicy policy);
		/** adding a weak reference */
		virtual void AddReference(WeakPointerPolicy policy);
		/** removing a shared reference */
		virtual void SubReference(SharedPointerPolicy policy);
		/** removing a weak reference */
		virtual void SubReference(WeakPointerPolicy policy);

	protected:

		/** called whenever there are no more reference on the object */
		virtual void OnLastReferenceLost();

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
	public:

		/** forwarding constructor */
		using T::T;

	protected:
		/** disable the reference lost deletion */
		virtual void OnLastReferenceLost() override { }
	};

}; // namespace chaos

	 /**
	 * ReferencedObject : reference count external methods
	 *
	 * XXX : theses functions are out of chaos scope, else while shared_ptr is in chaos, it searches for chaos::intrusive_ptr_add function in prioriy
	 *       and if it was finding ReferencedObject reference functions inside chaos scope, it will fail with IrrklangTools functions
	 */

	 /** utility method for shared_ptr */
template<typename POLICY = chaos::SharedPointerPolicy>
void intrusive_ptr_add_ref(chaos::ReferencedObject * obj, POLICY policy = POLICY()) // to work with boost::intrusive_ptr<>
{
	obj->AddReference(policy);
}

/** utility method for shared_ptr */
template<typename POLICY = chaos::SharedPointerPolicy>
void intrusive_ptr_release(chaos::ReferencedObject * obj, POLICY policy = POLICY()) // to work with boost::intrusive_ptr<>
{
	obj->SubReference(policy);
}



