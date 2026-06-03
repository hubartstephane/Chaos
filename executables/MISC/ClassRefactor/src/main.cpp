#include "chaos/Chaos.h"
#include <Windows.h>
#include <iostream>
#include <streambuf>

using namespace chaos;

//-----------------------------------------------------------

class DebugOutputBuf : public std::streambuf
{
protected:
	virtual int overflow(int c) override
	{
		if (c != EOF)
		{
			char ch = static_cast<char>(c);
			OutputDebugStringA(std::string(1, ch).c_str());
		}
		return c;
	}
};

//-----------------------------------------------------------

template<typename CLASS_TYPE>
class MyClass;

class MyClassManager;

//-----------------------------------------------------------



//-----------------------------------------------------------

class MyClassBase
{
	friend class MyRootClassManager;

public:

	/** destructor */
	virtual ~MyClassBase() = default;

	/** gets the class name */
	std::string const& GetClassName() const { return name; }
	/** gets the short name */
	std::string const& GetShortName() const { return short_name; }
	/** gets the class size */
	size_t GetClassSize() const { return class_size; }
	/** returns whether the class has been registered */
	bool IsFullyInitialized() const { return info != nullptr; }
	/** gets the parent class */
	MyClassBase const* GetParentClass() const { return parent_class; }
	/** gets the class manager */
	MyClassManager* GetClassManager() const { return manager; } // no need to have a manager const

	template<typename CLASS_TYPE>
	InheritanceType InheritsFrom(bool accept_equal = true) const
	{

#if 0

		// class not registered, cannot known result
		if (!IsFullyInitialized())
			return InheritanceType::Unknown;
		// other class not fully initialized
		if (!MyClass<CLASS_TYPE>::GetCPPClassInstance()->IsFullyInitialized())
			return InheritanceType::Unknown;

#endif


		// returns no if classes are same and we don't accept that as a valid result
		if (this == MyClass<CLASS_TYPE>::GetCPPClassInstance())
		{
			if (!accept_equal)
				return InheritanceType::No;
			else
				return InheritanceType::Yes;
		}
		// check with casting
		if (dynamic_cast<MyClass<CLASS_TYPE> const *>(this) != nullptr)
			return InheritanceType::Yes;
		return InheritanceType::No;
	}

protected:

	/** the optional name of the class */
	std::string name;
	/** the optional short name of the class */
	std::string short_name;
	/** get class size */
	size_t class_size = 0;
	/** the type_info for the class */
	std::type_info const * info = nullptr;
	/** the parent of the class */
	MyClassBase const* parent_class = nullptr;
	/** the manager for this class */
	MyClassManager* manager = nullptr;
};

//-----------------------------------------------------------

template<typename CLASS_TYPE>
struct MyClassParentClass
{
	using type = MyClassBase;
};

template<typename CLASS_TYPE>
requires
(
	HasSuperType<CLASS_TYPE>
)
struct MyClassParentClass<CLASS_TYPE>
{
	using type = MyClass<SuperClass_t<CLASS_TYPE>>;
};

template<typename CLASS_TYPE>
using MyClassParentClass_t = typename MyClassParentClass<CLASS_TYPE>::type;

//-----------------------------------------------------------


template<typename CLASS_TYPE>
class MyClass : public MyClassParentClass_t<CLASS_TYPE>
{
	friend class MyClassManager;

public:

	using Super = MyClassParentClass_t<CLASS_TYPE>;

	CLASS_TYPE* CreateInstance() const
	{
		assert(IsFullyInitialized());
		CLASS_TYPE * result = AllocateInstance();
		if (result != nullptr)
			InitializeInstance(result);
		return result;
	}

	static MyClass * GetCPPClassInstance()
	{
		static MyClass<CLASS_TYPE> result;
		return &result;
	}

protected:

	virtual CLASS_TYPE* AllocateInstance() const
	{
		return new CLASS_TYPE;
	}

	void InitializeInstance(CLASS_TYPE * instance) const
	{
		if (this->parent_class != nullptr)
			this->parent_class->InitializeInstance(instance);
		DoInitializeInstance(instance);
	}

	virtual void DoInitializeInstance(CLASS_TYPE* instance) const
	{
	}
};

//-----------------------------------------------------------

//-----------------------------------------------------------

class MyClassFindResult
{
	friend class MyClassManager;

	using iterator_type = std::vector<MyClassBase*>::iterator;

public:

	template<typename CLASS_TYPE>
	operator CLASS_TYPE * () const
	{
		// check for cached result or stop if no class_manager
		if (result != nullptr || class_manager == nullptr)
			return result;

		// check for the very first entry (string comparaison not necessary)
		if ((*iterator)->InheritsFrom<CLASS_TYPE>(true) == InheritanceType::Yes)
		{
			result = *iterator;
			return result;
		}

		// we know that the iterator points on a valid entry. get the string that made this entry a good one
		std::string const& searched_name = (match_type == ClassMatchType::Name) ?
			(*iterator)->GetClassName() :
			(*iterator)->GetShortName();

		// search in manager chain
		while (class_manager != nullptr)
		{
			while (iterator != class_manager->classes.end())
			{
				if (HasAnyFlags(flags, FindClassFlags::Name))
				{
					if (StringTools::Stricmp(iterator->GetClassName(), searched_name) == 0)
					{
						result = dynamic_cast<CLASS_TYPE>(*iterator);
						if (result != nullptr)
						{
							return result;
						}
					}
				}
				if (HasAnyFlags(flags, FindClassFlags::Shortname))
				{
					if (StringTools::Stricmp(iterator->GetShortName(), searched_name) == 0)
					{
						result = dynamic_cast<CLASS_TYPE>(*iterator);
						if (result != nullptr)
							return result;
					}
				}
				++iterator;
			}
			if (!HasAnyFlags(find_flags, FindClassFlags::ParentManager))
				return nullptr;

			class_manager = class_manager->GetParentManager();
			if (class_manager == nullptr)
				return nullptr;
				
			iterator = class_manager->begin();
		}
		return nullptr;
	};

protected:

	/** constructor */
	MyClassFindResult(MyClassManager* in_class_manager, iterator_type in_iterator, ClassMatchType in_match_type, FindClassFlags in_find_flags);

protected:

	/** cache the resolved result */
	mutable MyClassBase* result = nullptr;
	/** the class manager where to search */
	mutable MyClassManager* class_manager = nullptr;
	/** the very first name matching the request. we can use it for further research instead to store the name somehow (that would be costly) */
	mutable iterator_type iterator;
	/** whether the iterator correspond to a matching name or a matching short name */
	ClassMatchType match_type = ClassMatchType::Name;
	/** the flags used during the search */
	FindClassFlags find_flags = FindClassFlags::All;

};

MyClassFindResult::MyClassFindResult(MyClassManager* in_class_manager, iterator_type in_iterator, ClassMatchType in_match_type, FindClassFlags in_find_flags):
	class_manager(in_class_manager),
	iterator(in_iterator),
	match_type(in_match_type),
	find_flags(in_find_flags)
{
}

//-----------------------------------------------------------

class MyClassManager : public Object
{
public:

	/** constructor */
	MyClassManager(MyClassManager* in_parent_manager = nullptr);

	/** gets the parent manager */
	MyClassManager* GetParentManager() { return parent_manager.get(); }
	/** gets the parent manager */
	MyClassManager const* GetParentManager() const { return parent_manager.get(); }

	MyClassFindResult FindClass(char const* name, FindClassFlags flags = FindClassFlags::All);

protected:

	/* the parent class manager */
	shared_ptr<MyClassManager> parent_manager;
	/** the classes owned by this manager */
	std::vector<MyClassBase*> classes;
};


MyClassManager::MyClassManager(MyClassManager* in_parent_manager) :
	parent_manager(in_parent_manager)
{
}




MyClassFindResult MyClassManager::FindClass(char const* name, FindClassFlags flags)
{
	assert(name != nullptr);
	assert(HasAnyFlags(flags, FindClassFlags::Name | FindClassFlags::Shortname)); // at least one search criteria

	// early exit
	if (StringTools::IsEmpty(name))
		return { this, classes.end(), ClassMatchType::Name, flags }; // empty ClassFindResult result

	// search in manager chain
	MyClassManager* manager = this;
	while (manager != nullptr)
	{
		ClassMatchType match_type = ClassMatchType::Name;
		auto it = std::ranges::find_if(manager->classes, [name, &match_type, flags](MyClassBase const* cls)
		{
			if (HasAnyFlags(flags, FindClassFlags::Name))
			{
				if (StringTools::Stricmp(cls->GetClassName(), name) == 0)
				{
					match_type = ClassMatchType::Name;
					return true;
				}
			}
			if (HasAnyFlags(flags, FindClassFlags::Shortname))
			{
				if (StringTools::Stricmp(cls->GetShortName(), name) == 0)
				{
					match_type = ClassMatchType::Shortname;
					return true;
				}
			}
			return false;
		});
		if (it != manager->classes.end())
			return { this, it, match_type, flags };
		if (!HasAnyFlags(flags, FindClassFlags::ParentManager))
			break;
		manager = manager->parent_manager.get();
	}
	// no class, no possible alias
	return { nullptr, classes.end(), ClassMatchType::Name, flags }; // empty ClassFindResult result
}









//-----------------------------------------------------------

class MyRootClassManager : public MyClassManager, public Singleton<MyRootClassManager>
{
public:

	template<typename CLASS_TYPE>
	MyClass<CLASS_TYPE> const* RegisterCPPClass(std::string name)
	{
		MyClass<CLASS_TYPE>* result = MyClass<CLASS_TYPE>::GetCPPClassInstance();
		if (result->IsFullyInitialized())
			return result;

		result->name = std::move(name);
		result->class_size = sizeof(CLASS_TYPE);
		result->info = &typeid(CLASS_TYPE);
		result->manager = this;
		if constexpr (HasSuperType<CLASS_TYPE>)
			result->parent_class = MyClass<SuperClass_t<CLASS_TYPE>>::GetCPPClassInstance();

		classes.push_back(result);
		return result;
	}
};



//-----------------------------------------------------------

#define MY_CHAOS_DECLARE_OBJECT_CLASS(CLASS, PARENT_CLASS)\
public:\
using Super = PARENT_CLASS;\
static MyClass<CLASS> const * GetStaticClass(){ return CLASS##_class;}\
virtual MyClass<CLASS> const * GetClass() const { return CLASS##_class; }\
static inline MyClass<CLASS> const * CLASS##_class = MyDeclareObjectClass<CLASS>(#CLASS)

class A
{
protected:

	/** declare the class in the default C++ ClassManager (must be declared before the CHAOS_DECLARE_OBJECT_CLASS usage) */
	template<typename CLASS_TYPE>
	static MyClass<CLASS_TYPE> const* MyDeclareObjectClass(std::string name);

public:

	A() { std::cout << "A::A()" << std::endl; }

	virtual ~A() = default;

	static MyClass<A> const * GetStaticClass() { return A_class; }
	virtual MyClass<A> const* GetClass() const { return A_class; }
	static inline MyClass<A> const* A_class = MyDeclareObjectClass<A>("A");
};

template<typename CLASS_TYPE>
MyClass<CLASS_TYPE> const* A::MyDeclareObjectClass(std::string name)
{
	return MyRootClassManager::GetInstance()->RegisterCPPClass<CLASS_TYPE>(std::move(name));
}

//-----------------------------------------------------------











class B : public A
{

public:

	MY_CHAOS_DECLARE_OBJECT_CLASS(B, A);

	B() { std::cout << "B::B()" << std::endl; }
};



class BidonC : public MyClass<B>
{
};

int main(int argc, char ** argv, char ** env)
{
	DebugOutputBuf debugBuf;
	std::cout.rdbuf(&debugBuf);


	MyClass<A> const * ca = MyClass<A>::GetCPPClassInstance();
	MyClass<B> const * cb = MyClass<B>::GetCPPClassInstance();
	BidonC cc;  
	auto inherit1 = ca->InheritsFrom<A>();
	auto inherit2 = ca->InheritsFrom<B>();
	auto inherit3 = cb->InheritsFrom<A>();
	auto inherit4 = cb->InheritsFrom<B>();


	auto inherit5 = cc.InheritsFrom<A>();
	auto inherit6 = cc.InheritsFrom<B>();



	std::cout << "inherit1 : " << (int)inherit1 << std::endl;
	std::cout << "inherit2 : " << (int)inherit2 << std::endl;
	std::cout << "inherit3 : " << (int)inherit3 << std::endl;
	std::cout << "inherit4 : " << (int)inherit4 << std::endl;
	std::cout << "inherit5 : " << (int)inherit5 << std::endl;
	std::cout << "inherit6 : " << (int)inherit6 << std::endl;

	return 0;
}




#if 0



template<typename CLASS_TYPE>
class MyClass : public MyClassBase
{
public:

	using Super = MyClassBase;

	MyClass(std::string in_name, std::type_info const& in_info = typeid(CLASS_TYPE)) :
		Super(std::move(in_name), in_info) {
	}

	virtual CLASS_TYPE* CreateInstance() const
	{
		return new CLASS_TYPE;
	}
};

template<typename CLASS_TYPE>
	requires
(
	HasInternalSuperType<CLASS_TYPE> || HasExternalSuperType<CLASS_TYPE>
	)
	class MyClass<CLASS_TYPE> : public MyClass<SuperClass_t<CLASS_TYPE>>
{
public:

	using Super = MyClass<SuperClass_t<CLASS_TYPE>>;

	MyClass(std::string in_name, std::type_info const& in_info = typeid(CLASS_TYPE)) :
		Super(std::move(in_name), in_info) {
	}

	virtual CLASS_TYPE* CreateInstance() const override
	{
		return new CLASS_TYPE;
	}
};




#endif
