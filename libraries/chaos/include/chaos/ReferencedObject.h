#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	/** some tags for pointer's policies */
	class SharedPointerPolicy {};
	class WeakPointerPolicy {};

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
		virtual void AddReference(SharedPointerPolicy policy = SharedPointerPolicy())
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
		virtual void SubReference(SharedPointerPolicy policy = SharedPointerPolicy())
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
	 * SmartPointerBase : implementation of both shared_ptr and weak_ptr
	 */

	template<typename T, typename POLICY>
	class SmartPointerBase
	{
	public:

		/** the type of object pointed */
		using type = T;
		
		/** default constructor */
		SmartPointerBase() {}
		/** constructor with capturing the object */
		template<typename U>
		SmartPointerBase(U * in_target) :
			target(in_target)
		{
			if (target != nullptr)
				intrusive_ptr_add_ref(target, POLICY());
		}
		/** copy constructor */
		template<typename U>
		SmartPointerBase(SmartPointerBase<U, POLICY> const & src) :
			SmartPointerBase(src.target)
		{
		}
		/** move constructor */
		template<typename U>
		SmartPointerBase(SmartPointerBase<U, POLICY> && src) :
			target(src.target)
		{
			src.target = nullptr; // necessary to capture the reference, else the move semantic would
		}                       // not be an optimization while requiring reference count update    

		/** destructor */
		~SmartPointerBase()
		{
			if (target != nullptr)
				intrusive_ptr_release(target, POLICY());
		}

		/** copy */
		template<typename U>
		SmartPointerBase & operator = (U * src)
		{
			DoSetTarget(src);
			return *this;
		}
		/** copy */
		template<typename U>
		SmartPointerBase & operator = (SmartPointerBase<U, POLICY> const & src)
		{
			DoSetTarget(src.target);
			return *this;
		}
		/** move */
		template<typename U>
		SmartPointerBase & operator = (SmartPointerBase<U, POLICY> && src)
		{
			assert(this != &src);
			if (target != src.target)
			{
				if (target != nullptr)
					intrusive_ptr_release(target, POLICY());
				target = src.target;
			}
			else
			{
				if (src.target != nullptr)
					intrusive_ptr_release(src.target, POLICY()); // no new reference added due to this, src' reference is lost
			}
			src.target = nullptr;
			return *this;
		}

		/** getters */
		type * get() const
		{
			return target;
		}

		/** getters */
		type * operator ->() const
		{
			assert(target != nullptr);
			return target;
		}

		/** getters */
		type & operator * () const
		{
			assert(target != nullptr);
			return *target;
		}

	protected:

		/** internal method to change the content of the pointer */
		template<typename U>
		void DoSetTarget(U * src)
		{
			if (target != src)
			{
				if (target != nullptr)
					intrusive_ptr_release(target, POLICY());
				target = src;
				if (target != nullptr)
					intrusive_ptr_add_ref(target, POLICY());
			}
		}

	protected:

		/** the object pointed */
		type * target = nullptr;
	};

	/** compare */
	template<class T, class POLICY, class U=T>
	bool operator == (SmartPointerBase<T, POLICY> const & src1, U * src2)
	{
		return (src1.get() == src2);
	}
	/** compare */
	template<class T, class POLICY, class U=T>
	bool operator != (SmartPointerBase<T, POLICY> const & src1, U * src2)
	{
		return (src1.get() != src2);
	}
	/** compare */
	template<class T, class POLICY, class U=T>
	bool operator == (U * src1, SmartPointerBase<T, POLICY> const & src2)
	{
		return (src1 == src2.get());
	}
	/** compare */
	template<class T, class POLICY, class U=T>
	bool operator != (U * src1, SmartPointerBase<T, POLICY> const & src2)
	{
		return (src1 != src2.get());
	}




	/** compare */
	template<class T, class POLICY>
	bool operator == (SmartPointerBase<T, POLICY> const & src1, nullptr_t src2)
	{
		return (src1.get() == src2);
	}
	/** compare */
	template<class T, class POLICY>
	bool operator != (SmartPointerBase<T, POLICY> const & src1, nullptr_t src2)
	{
		return (src1.get() != src2);
	}
	/** compare */
	template<class T, class POLICY>
	bool operator == (nullptr_t src1, SmartPointerBase<T, POLICY> const & src2)
	{
		return (src1 == src2.get());
	}
	/** compare */
	template<class T, class POLICY>
	bool operator != (nullptr_t src1, SmartPointerBase<T, POLICY> const & src2)
	{
		return (src1 != src2.get());
	}





	/** compare */
	template<class T, class POLICY, class U>
	bool operator == (SmartPointerBase<T, POLICY> const & src1, SmartPointerBase<U, POLICY> const & src2)
	{
		return (src1.get() == src2.get());
	}
	/** compare */
	template<class T, class POLICY, class U>
	bool operator != (SmartPointerBase<T, POLICY> const & src1, SmartPointerBase<U, POLICY> const & src2)
	{
		return (src1.get() != src2.get());
	}

	/**
	* weak_ptr & shared_ptr definition
	*/

	template<typename T>
	using weak_ptr = SmartPointerBase<T, WeakPointerPolicy>;
	template<typename T>
	using shared_ptr = SmartPointerBase<T, SharedPointerPolicy>;

#if 0










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
	using shared_ptr = boost::intrusive_ptr<T>;

#if 0
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
		type * get() const
		{
			return target;
		}

		/** getters */
		type * operator ->() const
		{
			assert(target != nullptr);
			return target;
		}

#if 0
		/** getters */
		type const * operator ->() const
		{
			assert(target != nullptr);
			return target;
		}
#endif
		/** getters */
		type & operator * () const
		{
			assert(target != nullptr);
			return *target;
		}

#if 0
		/** getters */
		type const & operator * () const
		{
			assert(target != nullptr);
			return *target;
		}
#endif

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

#endif
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
			weak_ptr(src.get())
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
		type * get() const
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





#endif

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

