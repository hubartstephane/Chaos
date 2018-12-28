#include <chaos/StandardHeaders.h> 

#include <death/Death.h> 

#include "Ludum43Game.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>










// ===========================================================================================

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

protected:

	/** adding a reference */
	void AddReference()
	{
		assert(!shared_destroyed);
		++shared_count;
	}

	/** removing a reference */
	void SubReference()
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
	void AddWeakReference()
	{
		++weak_count; // can add a weak reference even if the object is destroyed
	}

	/** removing a weak reference */
	void SubWeakReference()
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

class ReferencedObjectFriend
{
protected:

	void AddReference(ReferencedObject * target)
	{
		assert(target != nullptr);
		target->AddReference();
	}

	void SubReference(ReferencedObject * target)
	{
		assert(target != nullptr);
		target->SubReference();
	}

	void AddWeakReference(ReferencedObject * target)
	{
		assert(target != nullptr);
		target->AddWeakReference();
	}

	void SubWeakReference(ReferencedObject * target)
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



// ===========================================================================================




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
	shared_ptr(shared_ptr<type> const & src):
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
	type * Get() 
	{ 
		return target; 
	}

	/** getters */
	type const * Get() const 
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

// ===========================================================================================

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
	type * Get()
	{
		if (target != nullptr && IsSharedDestroyed(target)) // take the opportunity to release memory
		{
			SubWeakReference(target);
			target = nullptr;
		}
		return target;
	}
	/** getter */
	type const * Get() const
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




class AA : public ReferencedObject
{
public:

	~AA()
	{
		int i = 0;
		++i;
	}

	void ff()
	{
		int i = 0;
		++i;
	}

};




class C1
{
public:

	C1(int x) 
	{
		x = x;
	}

	C1(char const * s) : C1(666) {}

};

class C2 : public C1
{
	using C1::C1;  // forwarding constructor

};

class C3 : public C2
{

	using C2::C2;  // forwarding constructor

};





int _tmain(int argc, char ** argv, char ** env)
{
	{
		C2 c1(3);

		C2 c2("3");

	}

	{


		AA * aa = new AA;
		AA * cc = new AA;

		weak_ptr<AA> w1 = aa;
		weak_ptr<AA> w2 = aa;

		{
			shared_ptr<AA> ptr = aa;
			shared_ptr<AA> ptr2(ptr);

			aa->ff();
		}






#if 0
		ReferencedObject * bb = *ptr;

		bb = bb;


		auto x1 = (ptr == nullptr);
		auto x2 = (ptr != nullptr);
		auto x3 = (ptr == ptr2);
		auto x4 = (ptr != ptr2);
		auto x5 = (ptr == cc);
		auto x6 = (ptr != cc);
#endif


		aa = aa;
	}





	return 0;


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


