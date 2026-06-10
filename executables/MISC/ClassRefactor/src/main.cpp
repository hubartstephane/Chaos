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

template<typename CPP_TYPE>
class MyClass;

class MyClassManager;

//-----------------------------------------------------------



//-----------------------------------------------------------

class MyClassBase : public ReferenceCountedInterface
{
	friend class MyClassManager;
	friend class MyCPPClassManager;
	friend class MyClassLoader;

	template<typename CPP_TYPE>
	friend class MyCppClassRegisterResult;

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
	bool IsFullyInitialized() const { return manager != nullptr; }
	/** gets the parent class */
	MyClassBase const* GetParentClass() const { return parent_class; }
	/** gets the class manager */
	MyClassManager* GetClassManager() const { return manager; } // no need to have a manager const

	/** check whether we inherit from another class */
	bool InheritsFrom(MyClassBase const * base, bool accept_equal = true) const;
	/** check whether we inherit from another class */
	template<typename CPP_TYPE>
	bool InheritsFrom(bool accept_equal = true) const;

protected:

	void SetShortName(std::string in_shorname);

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

void MyClassBase::SetShortName(std::string in_shorname)
{ 
	assert(short_name.empty());
	short_name = std::move(in_shorname); 
}

//-----------------------------------------------------------

bool MyClassBase::InheritsFrom(MyClassBase const* base, bool accept_equal) const
{
	if (base == this)
		return accept_equal;

	MyClassBase const* cls = parent_class;
	while (cls != nullptr)
	{
		if (cls == base)
			return true;
		cls = cls->parent_class;
	}
	return false;
}

template<typename CPP_TYPE>
bool MyClassBase::InheritsFrom(bool accept_equal) const
{
	return InheritsFrom(MyClass<CPP_TYPE>::GetCPPClassInstance(), accept_equal);
}

//-----------------------------------------------------------

template<typename CPP_TYPE>
struct MyClassSuper
{
	using type = MyClassBase;
};

template<typename CPP_TYPE>
requires
(
	HasSuperType<CPP_TYPE>
)
struct MyClassSuper<CPP_TYPE>
{
	using type = MyClass<SuperClass_t<CPP_TYPE>>;
};

template<typename CPP_TYPE>
using MyClassSuper_t = typename MyClassSuper<CPP_TYPE>::type;

//-----------------------------------------------------------


template<typename CPP_TYPE>
class MyClass : public MyClassSuper_t<CPP_TYPE>
{
	friend class MyClassManager;

public:

	using Super = MyClassSuper_t<CPP_TYPE>;

	CPP_TYPE* CreateInstance() const
	{
		assert(this->IsFullyInitialized());
		CPP_TYPE* result = AllocateInstance();
		if (result != nullptr)
			InitializeInstance(result);
		return result;
	}

	static MyClass * GetCPPClassInstance()
	{
		static shared_ptr<MyClass<CPP_TYPE>> result = new MyClass<CPP_TYPE>;
		return result.get();
	}

protected:

	MyClass() = default;

	virtual CPP_TYPE* AllocateInstance() const
	{
		return new CPP_TYPE;
	}

	void InitializeInstance(CPP_TYPE* instance) const
	{
		if (this->parent_class != nullptr)
			this->parent_class->InitializeInstance(instance);
		DoInitializeInstance(instance);
	}

	virtual void DoInitializeInstance(CPP_TYPE* instance) const
	{
	}
};

//-----------------------------------------------------------

//-----------------------------------------------------------

class MyClassFindResult
{
	friend class MyClassManager;

	using iterator_type = std::vector<shared_ptr<MyClassBase>>::iterator;

public:

	template<typename OBJECT_TYPE>
	operator MyClass<OBJECT_TYPE> * () const;

	operator MyClassBase * () const;

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
	friend class MyClassFindResult;
	friend class MyClassLoader;

public:

	/** constructor */
	MyClassManager(MyClassManager* in_parent_manager = nullptr);

	/** destructor */
	~MyClassManager();

	/** gets the parent manager */
	MyClassManager* GetParentManager() { return parent_manager.get(); }
	/** gets the parent manager */
	MyClassManager const* GetParentManager() const { return parent_manager.get(); }

	MyClassFindResult FindClass(char const* name, FindClassFlags flags = FindClassFlags::All);

protected:

	/* the parent class manager */
	shared_ptr<MyClassManager> parent_manager;
	/** the classes owned by this manager */
	std::vector<shared_ptr<MyClassBase>> classes;
};


MyClassManager::MyClassManager(MyClassManager* in_parent_manager) :
	parent_manager(in_parent_manager)
{
}

MyClassManager::~MyClassManager()
{
	for (auto & cls : classes)
		cls->manager = nullptr;
}

MyClassFindResult MyClassManager::FindClass(char const* name, FindClassFlags flags)
{
	assert(name != nullptr);
	assert(HasAnyFlags(flags, FindClassFlags::Name | FindClassFlags::Shortname)); // at least one search criteria

	// early exit
	if (StringTools::IsEmpty(name))
		return { nullptr, classes.end(), ClassMatchType::Name, flags }; // empty ClassFindResult result

	// search in manager chain
	MyClassManager* manager = this;
	while (manager != nullptr)
	{
		ClassMatchType match_type = ClassMatchType::Name;
		auto it = std::ranges::find_if(manager->classes, [name, &match_type, flags](shared_ptr<MyClassBase> const & cls)
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
			return { manager, it, match_type, flags };
		if (!HasAnyFlags(flags, FindClassFlags::ParentManager))
			break;
		manager = manager->parent_manager.get();
	}
	// no class, no possible alias
	return { nullptr, classes.end(), ClassMatchType::Name, flags }; // empty ClassFindResult result
}







//-----------------------------------------------------------


template<typename OBJECT_TYPE>
MyClassFindResult::operator MyClass<OBJECT_TYPE> * () const
{
	// check for cached result or stop if no class_manager
	if (result != nullptr || class_manager == nullptr)
		return (MyClass<OBJECT_TYPE>*)result;

	// check for the very first entry (string comparaison not necessary)
	if ((*iterator)->InheritsFrom<OBJECT_TYPE>(true))
	{
		result = iterator->get();
		return (MyClass<OBJECT_TYPE>*)result;
	}
	++iterator; // no need to test again for this iterator
	if (iterator != class_manager->classes.end())
	{
		class_manager = nullptr; // so we have early result next time this function is called
		return nullptr;
	}

	// we know that the iterator points on a valid entry. get the string that made this entry a good one
	std::string const& searched_name = (match_type == ClassMatchType::Name) ?
		iterator->get()->GetClassName() :
		iterator->get()->GetShortName();

	// search in manager chain
	while (class_manager != nullptr)
	{
		while (iterator != class_manager->classes.end())
		{
			if (HasAnyFlags(find_flags, FindClassFlags::Name))
			{
				if (StringTools::Stricmp((*iterator)->GetClassName(), searched_name) == 0)
				{
					if ((*iterator)->InheritsFrom<OBJECT_TYPE>(true))
					{
						result = iterator->get(); // cache the result for next call
						return (MyClass<OBJECT_TYPE>*)result;
					}
					++iterator;
					continue; // the class does not match no matter what. no need to check for the short name
				}
			}
			if (HasAnyFlags(find_flags, FindClassFlags::Shortname))
			{
				if (StringTools::Stricmp((*iterator)->GetShortName(), searched_name) == 0)
				{
					if ((*iterator)->InheritsFrom<OBJECT_TYPE>(true))
					{
						result = iterator->get(); // cache the result for next call
						return (MyClass<OBJECT_TYPE>*)result;
					}
				}
			}
			++iterator;
		}
		if (!HasAnyFlags(find_flags, FindClassFlags::ParentManager))
			break;

		class_manager = class_manager->GetParentManager();
		if (class_manager == nullptr)
			break;

		iterator = class_manager->classes.begin();
	}
	class_manager = nullptr; // so we have early result next time this function is called
	return nullptr;
};

MyClassFindResult::operator MyClassBase* () const
{



	return nullptr;
}

//-----------------------------------------------------------

template<typename CPP_TYPE>
class MyCppClassRegisterResult
{
public:

	/** set the short name for the class */
	MyCppClassRegisterResult& ShortName(std::string in_short_name)
	{
		MyClass<CPP_TYPE>::GetCPPClassInstance()->SetShortName(std::move(in_short_name));
		return *this;
	}

	/** convert the registration to the class */
	operator MyClass<CPP_TYPE> * () const
	{
		return MyClass<CPP_TYPE>::GetCPPClassInstance();
	}
};


//-----------------------------------------------------------

class MyCPPClassManager : public MyClassManager, public Singleton<MyCPPClassManager>
{
public:

	template<typename CPP_TYPE>
	MyCppClassRegisterResult<CPP_TYPE> RegisterCPPClass(char const * name)
	{
		MyClass<CPP_TYPE>* result = MyClass<CPP_TYPE>::GetCPPClassInstance();
		if (result->IsFullyInitialized()) // class has already be registered ?
		{
			assert(StringTools::Stricmp(result->name, name) == 0); // ensure class is registered with same name
			return {};
		}

		result->name = name;
		result->class_size = sizeof(CPP_TYPE);
		result->info = &typeid(CPP_TYPE);
		result->manager = this;
		if constexpr (HasSuperType<CPP_TYPE>)
			result->parent_class = MyClass<SuperClass_t<CPP_TYPE>>::GetCPPClassInstance();

		classes.push_back(result);
		return {};
	}
};





//-----------------------------------------------------------

class CHAOS_API MyClassLoader
{
public:

	/** load one class by its path */
	virtual MyClassBase* LoadClass(MyClassManager* manager, FilePathParam const& path) const;
	/** load all classes in one directory */
	virtual bool LoadClassesInDirectory(MyClassManager* manager, FilePathParam const& path) const;

protected:

	/** internal method to declare a class without finding yet its parent (used for directory iteration) */
	MyClassBase* DoCreateSpecialClass(MyClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json) const; // XXX : no const return value here !! (for Finalization of special class)
	/** finalization of a special class (called from ClassLoader) : find parent */
	bool DoSetSpecialClassParent(MyClassManager* manager, MyClassBase* cls, std::string const& parent_class_name) const;
	/** internal method called from ClassLoader to abord a failed loaded class */
	void DoDeleteSpecialClass(MyClassManager* manager, MyClassBase* cls) const;
	/** internal method to complete the load */
	bool DoCompleteSpecialClassMissingData(MyClassBase* cls) const;
};












//-----------------------------------------------------------


// an utility function to load a JSON file a find the approriate classname
static MyClassBase* DoLoadClassHelper(FilePathParam const& path, LightweightFunction<MyClassBase* (std::string, std::string, std::string, nlohmann::json)> func)
{
	nlohmann::json json;
	if (JSONTools::LoadJSONFile(path, json))
	{
		// get or build a class name
		std::string class_name;
		if (!JSONTools::GetAttribute(&json, "class_name", class_name))
			class_name = PathTools::PathToName(path.GetResolvedPath());
		// parent class is MANDATORY for Special objects
		std::string parent_class_name;
		if (!JSONTools::GetAttribute(&json, "parent_class", parent_class_name))
		{
			ClassLog::Error("DoLoadClassHelper : special class [%s] require a parent class", class_name.c_str());
			return nullptr;
		}
		// create the class
		if (!class_name.empty())
		{
			std::string short_name;
			JSONTools::GetAttribute(&json, "short_name", short_name);
			return func(std::move(class_name), std::move(short_name), std::move(parent_class_name), std::move(json));
		}
	}
	return nullptr;
}

MyClassBase* MyClassLoader::LoadClass(MyClassManager* manager, FilePathParam const& path) const
{
	assert(manager != nullptr);

	return DoLoadClassHelper(path, [this, manager](std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json) -> MyClassBase*
	{
		if (MyClassBase* result = DoCreateSpecialClass(manager, std::move(class_name), std::move(short_name), std::move(json)))
		{
			if (DoSetSpecialClassParent(manager, result, parent_class_name))
			{
				DoCompleteSpecialClassMissingData(result);
				return result;
			}
			DoDeleteSpecialClass(manager, result);
		}
		return nullptr;
	});
}

bool MyClassLoader::LoadClassesInDirectory(MyClassManager* manager, FilePathParam const& path) const
{
	assert(manager != nullptr);

	using ClassRegistrationType = std::pair<MyClassBase*, std::string>; // <class, parent_class_name>

	std::vector<ClassRegistrationType> loaded_classes;

	// Step 1: load all classes (no full initialization, ignore parent). Register them (without inheritance data in classes list)
	FileTools::WithDirectoryContent(path, [this, manager, &loaded_classes](boost::filesystem::path const& p)
	{
		DoLoadClassHelper(p, [this, manager, &loaded_classes](std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json)
		{
			MyClassBase* result = DoCreateSpecialClass(manager, std::move(class_name), std::move(short_name), std::move(json));
			if (result != nullptr)
				loaded_classes.push_back({ result, std::move(parent_class_name) });
			return result;
		});
		return false; // don't stop
	});

	// Step 2: set parents
	for (ClassRegistrationType& class_registration : loaded_classes)
		DoSetSpecialClassParent(manager, class_registration.first, class_registration.second);

	//  Step 3: complete inheritance chain
	std::vector<MyClassBase*> to_remove;

	for (ClassRegistrationType& class_registration : loaded_classes)
	{
		if (!DoCompleteSpecialClassMissingData(class_registration.first)) // if it fais, this means that parent chain is broken
		{
			ClassLog::Error("MyClassLoader::LoadClassesInDirectory : special class [%s] has a broken inheritance chain.", class_registration.first->GetClassName().c_str());
			to_remove.push_back(class_registration.first);
		}
	}

	// Step 4: remove classes that have to
	for (MyClassBase* cls : to_remove)
		DoDeleteSpecialClass(manager, cls);
	return true;
}










bool MyClassLoader::DoCompleteSpecialClassMissingData(MyClassBase* cls) const
{
	// the class is already fully initialized
	if (cls->declared)
		return true;
	// cannot complete the initialization
	if (cls->parent == nullptr)
		return false;
	// recursively initialize children
	if (!DoCompleteSpecialClassMissingData(const_cast<Class*>(cls->parent))) // forced to remove constness here
		return false;
	// get missing data
	cls->declared = true;
	cls->class_size = cls->parent->class_size;
	cls->info = cls->parent->info;
	return true;
}

Class* MyClassLoader::DoCreateSpecialClass(MyClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json) const
{
	// check parameter and not already registered
	assert(!StringTools::IsEmpty(class_name));

	if (manager->FindClass(class_name.c_str(), FindClassFlags::Name) != nullptr)
	{
		ClassLog::Error("ClassLoader::DoCreateSpecialClass(...): class already existing [%s]", class_name.c_str());
		return nullptr;
	}

	if (Class* result = new ClassWithJSONInitialization(std::move(class_name), std::move(json)))
	{
		if (!StringTools::IsEmpty(short_name))
			result->SetShortName(std::move(short_name));
		manager->InsertClass(result);
		return result;
	}
	return nullptr;
}




bool MyClassLoader::DoSetSpecialClassParent(MyClassManager* manager, MyClassBase* cls, std::string const& parent_class_name) const
{
	// parent class is MANDATORY for Special objects
	cls->parent_class = manager->FindClass(parent_class_name.c_str());
	if (cls->parent_class == nullptr)
	{
		ClassLog::Error("MyClassLoader::DoSetSpecialClassParent : special class [%s] has unknown parent class [%s]", cls->name.c_str(), parent_class_name.c_str());
		return false;
	}
	return true;
}

void MyClassLoader::DoDeleteSpecialClass(MyClassManager* manager, MyClassBase* cls) const
{
	// remove & delete the class
	assert(cls != nullptr);
	manager->classes.erase(std::ranges::find(manager->classes, cls));
	delete(cls);
}






//-----------------------------------------------------------




//-----------------------------------------------------------

#define MY_CHAOS_DECLARE_OBJECT_CLASS(CLASS, PARENT_CLASS)\
public:\
using Super = PARENT_CLASS;\
static MyClass<CLASS> const * GetStaticClass(){ return CLASS##_class;}\
virtual MyClass<CLASS> const * GetClass() const { return CLASS##_class; }\
static inline MyClass<CLASS> const * CLASS##_class = MyDeclareCPPClass<CLASS>(#CLASS)

class A
{
protected:

	/** declare the class in the default C++ ClassManager (must be declared before the CHAOS_DECLARE_OBJECT_CLASS usage) */
	template<typename CPP_TYPE>
	static MyCppClassRegisterResult<CPP_TYPE> MyDeclareCPPClass(char const* name);

public:

	A() { std::cout << "A::A()" << std::endl; }

	virtual ~A() = default;

	static MyClass<A> const* GetStaticClass() { return A_class; }
	virtual MyClass<A> const* GetClass() const { return A_class; }
	static inline MyClass<A> const* A_class = MyDeclareCPPClass<A>("A");
};

template<typename CPP_TYPE>
MyCppClassRegisterResult<CPP_TYPE> A::MyDeclareCPPClass(char const* name)
{
	return MyCPPClassManager::GetInstance()->RegisterCPPClass<CPP_TYPE>(name);
}






class B : public A
{

public:

	MY_CHAOS_DECLARE_OBJECT_CLASS(B, A).ShortName("bbb");

	B() { std::cout << "B::B()" << std::endl; }
};



class BidonC : public MyClass<B>
{
};

int main(int argc, char ** argv, char ** env)
{
	DebugOutputBuf debugBuf;
	std::cout.rdbuf(&debugBuf);


	//MyClass<A> const * ca = MyClass<A>::GetCPPClassInstance();
	//MyClass<B> const * cb = MyClass<B>::GetCPPClassInstance();


	MyClass<A> const * ca = MyCPPClassManager::GetInstance()->FindClass("A");
	MyClass<B> const * cb = MyCPPClassManager::GetInstance()->FindClass("bbb");

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