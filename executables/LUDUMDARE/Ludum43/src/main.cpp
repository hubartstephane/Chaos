#include <chaos/StandardHeaders.h> 

#include <death/Death.h> 

#include "Ludum43Game.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

// ===========================================================================================

template<typename T>
class shared_ptr
{
	using type = T;
		
public:

	/** default constructor */
	shared_ptr() {}
	/** constructor with capturing the object */
	shared_ptr(type * in_target) :
		target(in_target)
	{
		if (target != nullptr)
			target->AddReference();
	}

	/** copy constructor */
	shared_ptr(shared_ptr<type> const & src):
		target(src.target)
	{
		if (target != nullptr)
			target->AddReference();
	}

	/** move constructor */
	shared_ptr(shared_ptr<type> && src):
		target(src.target)
	{
	}

	/** destructor */
	~shared_ptr()
	{
		if (target != nullptr)
			target->SubReference();
	}

	/** copy */
	shared_ptr<type> & operator = (type * src)
	{
		DoSetTarget(src);
		return *this;
	}

	shared_ptr<type> & operator = (shared_ptr<type> const & src)
	{
		DoSetTarget(src.target);
		return *this;
	}

	shared_ptr<type> & operator = (shared_ptr<type> && src)
	{
		DoSetTarget(src.target);
		return *this;
	}

	/** tests */
	bool operator == (type const * src) const
	{
		return (target == src);
	}
	bool operator != (type const * src) const
	{
		return (target != src);
	}

	bool operator == (shared_ptr<type> const & src) const
	{
		return (target == src.target);
	}
	bool operator != (shared_ptr<type> const & src) const
	{
		return (target != src.target);
	}

	/** getters */
	type * operator ->() { return target; }
	/** getters */
	type const * operator ->() const { return target; }

	/** getters */
	type * operator * () { return target; }
	/** getters */
	type const * operator * () const { return target; }

protected:

	/** internal method to change the content of the pointer */
	void DoSetTarget(type * src)
	{
		if (src == target)
			return;
		if (target != nullptr)
			target->SubReference();
		target = src;
		if (src != nullptr)
			src->AddReference();
	}

protected:

	/** the object pointed */
	type * target = nullptr;
};

// ===========================================================================================

template<typename T>
class weak_ptr
{
public:

};

// ===========================================================================================

class ReferencedObject
{

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
				free(this);	
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


int _tmain(int argc, char ** argv, char ** env)
{
	{
		AA * aa = new AA;
		AA * cc = new AA;

		shared_ptr<AA> ptr = aa;
		shared_ptr<AA> ptr2(ptr);

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


