#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	/** 
	 * some tags for pointer's policies 
	 */

	class SharedPointerPolicy	
	{
	public:

		template<typename T>
		static auto Get(T const * smart_ptr)
		{
			return smart_ptr->target;
		}
	};

	class WeakPointerPolicy 
	{
	public:

		template<typename T>
		static auto Get(T const * smart_ptr)
		{
			return smart_ptr->target;
		}	
	};

	/**
	 * SmartPointerBase : implementation of both shared_ptr and weak_ptr
	 */

	template<typename T, typename POLICY>
	class SmartPointerBase
	{
		friend class SharedPointerPolicy;
		friend class WeakPointerPolicy;

	public:

		/** the type of object pointed */
		using type = T;
		
		/** default constructor */
		SmartPointerBase() {}
		/** constructor with capturing the object */
		SmartPointerBase(type * in_target) :
			target(in_target)
		{
			if (target != nullptr)
				intrusive_ptr_add_ref(target, POLICY());
		}
		/** copy constructor */
		SmartPointerBase(SmartPointerBase<T, POLICY> const & src) :
			SmartPointerBase(src.get())
		{
		}
		/** conversion constructor */
#if 0
		template<typename U>
		SmartPointerBase(SmartPointerBase<U, POLICY> const & src) : // shuxxx to test
			target(src.target)
		{
			if (target != nullptr)
				intrusive_ptr_add_ref(target, POLICY()); 
		}
#endif
		/** move constructor */
		SmartPointerBase(SmartPointerBase<T, POLICY> && src) :  // shuxxx to test
			target(src.get())
		{
			src.target = nullptr; // necessary to capture the reference, else the move semantic would
		}
		/** move constructor + conversion */
#if 0
		template<typename U>
		SmartPointerBase(SmartPointerBase<U, POLICY> && src) :  // shuxxx to test
			target(src.target)
		{
			src.target = nullptr; // necessary to capture the reference, else the move semantic would
		}                       // not be an optimization while requiring reference count update    
#endif		

		/** destructor */
		~SmartPointerBase()
		{
			if (target != nullptr)
				intrusive_ptr_release(target, POLICY());
		}

		/** copy */
		SmartPointerBase & operator = (type * src)
		{
			DoSetTarget(src);
			return *this;
		}
		/** copy */
		SmartPointerBase & operator = (SmartPointerBase<T, POLICY> const & src)
		{
			DoSetTarget(src.get());
			return *this;
		}
		/** copy + conversion */
#if 0
		template<typename U>
		SmartPointerBase & operator = (SmartPointerBase<U, POLICY> const & src)
		{
			DoSetTarget(src.target);
			return *this;
		}
#endif
		/** move */
		SmartPointerBase & operator = (SmartPointerBase<T, POLICY> && src) // shuxxx to test
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
		/** move */
#if 0
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
#endif

		/** getters */
		type * get() const
		{
			return POLICY::Get(this);
		}

		/** getters */
		type * operator ->() const
		{
			type * result = get();
			assert(result != nullptr);
			return result;
		}

		/** getters */
		type & operator * () const
		{
			type * result = get();
			assert(result != nullptr);
			return *result;
		}

		/** swap pointers */
		void swap(SmartPointerBase<T, POLICY> & src)
		{
			if (&src != this)
			{
				type * tmp = get();
				target = src.get();
				src.target = tmp;
			}
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
		mutable type * target = nullptr;
	};


	/**
	* SmartPointerBase / raw pointer comparaisons
	*/

	template<typename T, typename POLICY, typename U>
	bool operator == (SmartPointerBase<T, POLICY> const & src1, U * src2)
	{
		return (src1.get() == src2);
	}

	template<typename T, typename POLICY, typename U>
	bool operator != (SmartPointerBase<T, POLICY> const & src1, U * src2)
	{
		return (src1.get() != src2);
	}

	template<typename T, typename POLICY, typename U>
	bool operator == (U * src1, SmartPointerBase<T, POLICY> const & src2)
	{
		return (src1 == src2.get());
	}

	template<typename T, typename POLICY, typename U>
	bool operator != (U * src1, SmartPointerBase<T, POLICY> const & src2)
	{
		return (src1 != src2.get());
	}

	/**
	* SmartPointerBase / nullptr comparaisons
	*/

	template<typename T, typename POLICY>
	bool operator == (SmartPointerBase<T, POLICY> const & src1, nullptr_t src2)
	{
		return (src1.get() == src2);
	}

	template<typename T, typename POLICY>
	bool operator != (SmartPointerBase<T, POLICY> const & src1, nullptr_t src2)
	{
		return (src1.get() != src2);
	}

	template<typename T, typename POLICY>
	bool operator == (nullptr_t src1, SmartPointerBase<T, POLICY> const & src2)
	{
		return (src1 == src2.get());
	}

	template<typename T, typename POLICY>
	bool operator != (nullptr_t src1, SmartPointerBase<T, POLICY> const & src2)
	{
		return (src1 != src2.get());
	}

	/**
	* SmartPointerBase / SmartPointerBase comparaisons
	*/

	template<typename T, typename POLICY, typename U>
	bool operator == (SmartPointerBase<T, POLICY> const & src1, SmartPointerBase<U, POLICY> const & src2)
	{
		return (src1.get() == src2.get());
	}

	template<typename T, typename POLICY, typename U>
	bool operator != (SmartPointerBase<T, POLICY> const & src1, SmartPointerBase<U, POLICY> const & src2)
	{
		return (src1.get() != src2.get());
	}

	/**
	* swap
	*/

	template<typename T, typename POLICY> 
	void swap(SmartPointerBase<T, POLICY> & src1, SmartPointerBase<T, POLICY> & src2)
	{
		src1.swap(src2);
	}

	/**
	* weak_ptr & shared_ptr definition
	*/

	template<typename T>
	using weak_ptr = SmartPointerBase<T, WeakPointerPolicy>;
	template<typename T>
	using shared_ptr = SmartPointerBase<T, SharedPointerPolicy>;
	//using shared_ptr = boost::intrusive_ptr<T>;

}; // namespace chaos

