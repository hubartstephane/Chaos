#pragma once

#include <chaos/StandardHeaders.h>

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

		/** adding a reference */
		virtual void AddReference()
		{
			assert(!shared_destroyed);
			++shared_count;
		}

		/** removing a reference */
		virtual void SubReference()
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

		/** adding a weak reference */
		virtual void AddWeakReference()
		{
			++weak_count; // can add a weak reference even if the object is destroyed
		}

		/** removing a weak reference */
		virtual void SubWeakReference()
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
	* ReferencedObjectFriend is a helper class to give access to ReferencedObject protected members
	*/

	class ReferencedObjectFriend
	{
	protected:

		void AddReference(ReferencedObject * target) const
		{
			assert(target != nullptr);
			target->AddReference();
		}

		void SubReference(ReferencedObject * target) const
		{
			assert(target != nullptr);
			target->SubReference();
		}

		void AddWeakReference(ReferencedObject * target) const
		{
			assert(target != nullptr);
			target->AddWeakReference();
		}

		void SubWeakReference(ReferencedObject * target) const
		{
			assert(target != nullptr);
			target->SubWeakReference();
		}

		bool IsSharedDestroyed(ReferencedObject * target) const
		{
			assert(target != nullptr);
			return target->shared_destroyed;
		}
	};

	/**
	* shared_ptr : self describing
	*/

	template<typename T>
	class shared_ptr : public ReferencedObjectFriend
	{

	public:

		using type = T;

		/** default constructor */
		shared_ptr() {}
		/** constructor with capturing the object */
		shared_ptr(type * in_target) :
			target(in_target)
		{
			if (target != nullptr)
				AddReference(target);
		}

		/** copy constructor */
		shared_ptr(shared_ptr<type> const & src) :
			shared_ptr(src.target)
		{
		}

		/** move constructor */
		shared_ptr(shared_ptr<type> && src) :
			target(src.target)
		{
			src.target = nullptr; // necessary to capture the reference, else the move semantic would
		}                       // not be an optimization while requiring reference count update    

		/** destructor */
		~shared_ptr()
		{
			if (target != nullptr)
				SubReference(target);
		}

		/** copy */
		shared_ptr<type> & operator = (type * src)
		{
			DoSetTarget(src);
			return *this;
		}
		/** copy */
		shared_ptr<type> & operator = (shared_ptr<type> const & src)
		{
			DoSetTarget(src.target);
			return *this;
		}
		/** copy */
		shared_ptr<type> & operator = (shared_ptr<type> && src)
		{
			assert(this != &src);
			if (target != src.target)
			{
				if (target != nullptr)
					SubReference(target);
				target = src.target;
			}
			else
			{
				if (src.target != nullptr)
					SubReference(src.target); // no new reference added due to this, src' reference is lost
			}
			src.target = nullptr;
			return *this;
		}

		/** equality */
		bool operator == (type const * src) const
		{
			return (target == src);
		}

		/** equality */
		bool operator != (type const * src) const
		{
			return (target != src);
		}

		/** equality */
		bool operator == (shared_ptr<type> const & src) const
		{
			return (target == src.target);
		}

		/** equality */
		bool operator != (shared_ptr<type> const & src) const
		{
			return (target != src.target);
		}

		/** getters */
		type * Get() const
		{
			return target;
		}

		/** getters */
		type * operator ->()
		{
			assert(target != nullptr);
			return target;
		}
		/** getters */
		type const * operator ->() const
		{
			assert(target != nullptr);
			return target;
		}

		/** getters */
		type & operator * ()
		{
			assert(target != nullptr);
			return *target;
		}
		/** getters */
		type const & operator * () const
		{
			assert(target != nullptr);
			return *target;
		}

	protected:

		/** internal method to change the content of the pointer */
		void DoSetTarget(type * src)
		{
			if (target != src)
			{
				if (target != nullptr)
					SubReference(target);
				target = src;
				if (target != nullptr)
					AddReference(target);
			}
		}

	protected:

		/** the object pointed */
		type * target = nullptr;
	};

	/**
	* weak_ptr : self describing
	*/

	template<typename T>
	class weak_ptr : public ReferencedObjectFriend
	{
	public:

		using type = T;

		/** default constructor */
		weak_ptr() {}
		/** constructor with capturing the object */
		weak_ptr(type * in_target) :
			target(in_target)
		{
			if (target != nullptr)
			{
				if (!IsSharedDestroyed(target))
					AddWeakReference(target);
				else
					target = nullptr; // target is destroyed, simply ignore initialization of pointer. Give opportunity to free memory earlier
			}
		}

		/** copy constructor */
		weak_ptr(weak_ptr<type> const & src) : // forward constructor : give src the opportunity to release memory
			weak_ptr(src.Get())
		{
		}

		/** move constructor */
		weak_ptr(shared_ptr<type> && src) :
			target(src.target)
		{
			src.target = nullptr; // capture the reference
		}

		/** destructor */
		~weak_ptr()
		{
			if (target != nullptr)
				SubWeakReference(target);
		}

		/** copy */
		weak_ptr<type> & operator = (type * src)
		{
			DoSetTarget(src);
			return *this;
		}
		/** copy */
		weak_ptr<type> & operator = (weak_ptr<type> const & src)
		{
			DoSetTarget(src.target);
			return *this;
		}
		/** copy */
		weak_ptr<type> & operator = (weak_ptr<type> && src)
		{
			assert(this != &src);
			if (target != src.target)
			{
				if (target != nullptr)
					SubWeakReference(target);
				target = src.target;
			}
			else
			{
				if (src.target != nullptr)
					SubWeakReference(src.target); // no new reference added due to this, src' reference is lost
			}
			src.target = nullptr;
			return *this;
		}

		/** getter */
		type * Get() const
		{
			if (target != nullptr && IsSharedDestroyed(target)) // take the opportunity to release memory
			{
				SubWeakReference(target);
				target = nullptr;
			}
			return target;
		}

	protected:

		/** internal method to change the content of the pointer */
		void DoSetTarget(type * src)
		{
			if (target != src)
			{
				if (target != nullptr)
					SubWeakReference(target);
				target = src;
				if (target != nullptr)
					AddWeakReference(target);
			}
		}

	protected:

		/** the object pointed */
		mutable type * target = nullptr;
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

