#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	/**
	* ReferencedObject is a base class that have a reference count
	*/

	class ReferencedObject
	{
	public:

		/** default constructor */
		ReferencedObject() : reference_count(0) {}

		/** copy constructor */
		ReferencedObject(ReferencedObject const & src) : reference_count(0) {}

		/** destructor */
		virtual ~ReferencedObject() { assert(reference_count == 0); }

		/** copy operator (do not copy the reference count) */
		inline ReferencedObject & operator = (ReferencedObject const & src) { return *this; }

		/** Increment the reference count */
		virtual void AddReference() { ++reference_count; }

		/** Decrement the reference count */
		virtual void SubReference()
		{
			if (--reference_count == 0)
				OnLastReferenceLost();
		}

		/** utility method for boost::intrusive_ptr */
		friend inline void intrusive_ptr_add_ref(ReferencedObject * obj)
		{
			obj->AddReference();
		}

		/** utility method for boost::intrusive_ptr */
		friend inline void intrusive_ptr_release(ReferencedObject * obj)
		{
			obj->SubReference();
		}

	protected:

		/** called whenever there are no more reference on the object */
		virtual void OnLastReferenceLost() { delete(this); }

		/** the reference count of the object */
		boost::atomic<int> reference_count;
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

