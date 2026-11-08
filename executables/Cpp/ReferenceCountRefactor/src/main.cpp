#include "chaos/Chaos.h"

using namespace chaos;

// KeepOrForget

class OwnerPointerPolicy;
class OwnerCountedInterface;

template<typename T>
using owner_ptr = SmartPointer<T, OwnerPointerPolicy>;
















/**
 * OwnerPointerPolicy : policy used for owner_ptr
 */

class CHAOS_API OwnerPointerPolicy
{
public:

	/** the internal data in the smart pointer */
	template<typename T>
	using pointer_type = T;

	/** adding a reference */
	template<typename T>
	static T* AddReference(T* in_target)
	{
		assert(in_target != nullptr);
		in_target->AddOwnerReference();
		return in_target;
	}

	/** removing a reference */
	template<typename T>
	static T* SubReference(pointer_type<T>* in_target)
	{
		assert(in_target != nullptr);
		in_target->SubOwnerReference();
		return nullptr;
	}

	/** get the resource pointed */
	template<typename T>
	static T* Get(T* in_target)
	{
		return in_target;
	}
};


class OwnerCountedInterface : public ShareableInterface
{
	friend class OwnerPointerPolicy;

public:

	OwnerCountedInterface();

protected:

	virtual void SubReference() override;

	virtual void AddOwnerReference();

	virtual void SubOwnerReference();

	virtual void OnLastReferenceLost() override;

protected:

	virtual void OnRemoveFromOwner();

protected:
	
	std::atomic<int> owner_count;
};

OwnerCountedInterface::OwnerCountedInterface() :
	owner_count(0)
{
}

void OwnerCountedInterface::OnRemoveFromOwner()
{
	assert(0);
}

void OwnerCountedInterface::SubReference()
{
	assert(shared_count > 0);
	if (--shared_count == 0)
	{
		if (owner_count == 0)
			OnLastReferenceLost();
		else
			OnRemoveFromOwner();	
	}
}

void OwnerCountedInterface::AddOwnerReference()
{
	++owner_count;
}

void OwnerCountedInterface::SubOwnerReference()
{
	assert(owner_count > 0);
	if (--owner_count == 0)
	{
		if (shared_count == 0)
			OnLastReferenceLost();
	}
}

void OwnerCountedInterface::OnLastReferenceLost()
{
	delete(this);
}








template<typename T>
struct is_owner_ptr
{
	static constexpr bool value = false;
};

template<typename T>
struct is_owner_ptr<owner_ptr<T>>
{
	static constexpr bool value = true;
};


template<typename T>
constexpr bool is_owner_ptr_v = is_owner_ptr<T>::value;



// ---------------------------------------------------

class A;
class B;



// ---------------------------------------------------





class A : public OwnerCountedInterface
{
public:

	virtual void OnRemoveFromOwner() override;

	weak_ptr<B> owner;
};

class B : public chaos::Object
{
public:

	A * CreateObject();

	void RemoveObject(A* a);


	std::vector<owner_ptr<A>> objects;

};

// ---------------------------------------------------

void A::OnRemoveFromOwner()
{
	if (owner != nullptr)
	{
		owner->RemoveObject(this);	
	}
};


// ---------------------------------------------------

A* B::CreateObject()
{
	if (A* result = new A)
	{
		result->owner = this;
		objects.push_back(result);
		return result;
	}
	return nullptr;
}

void B::RemoveObject(A * a)
{
	for (auto it = objects.begin() ; it != objects.end() ; ++it)
	{
		if (*it == a)
		{
			a->owner = nullptr;
			objects.erase(it);
			return;
		}	
	}
}

// ---------------------------------------------------


int main(int argc, char** argv, char** env)
{
	B * b = new B;
	
	shared_ptr<A> a = b->CreateObject();

	

	return 0;
}


