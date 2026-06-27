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
	bool IsFullyInitialized() const { return info != nullptr; }
	/** gets the parent class */
	MyClassBase const* GetParentClass() const { return parent_class; }
	/** gets the class manager */
	MyClassManager* GetClassManager() const { return manager; } // no need to have a manager const

	/** check whether we inherit from another class */
	bool InheritsFrom(MyClassBase const * base, bool accept_same = true) const;
	/** check whether we inherit from another class */
	template<typename CPP_TYPE>
	bool InheritsFrom(bool accept_same = true) const;

protected:

	/** change the shortname of the class (just used at construction) */
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

bool MyClassBase::InheritsFrom(MyClassBase const* base, bool accept_same) const
{
	if (base == this)
		return accept_same;

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
bool MyClassBase::InheritsFrom(bool accept_same) const
{
	return InheritsFrom(MyClass<CPP_TYPE>::GetCPPClassInstance(), accept_same);
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
	friend class MyCPPClassManager;

	template<typename CPP_TYPE>
	friend class MyCppClassRegisterResult;

public:

	using Super = MyClassSuper_t<CPP_TYPE>;

	virtual CPP_TYPE* CreateInstance() const
	{
		assert(this->IsFullyInitialized());
		CPP_TYPE* result = AllocateInstance();
		if (result != nullptr)
			InitializeInstance(result);
		return result;
	}

	static MyClass const * GetCPPClassInstance()
	{
		return GetMutableCPPClassInstance();
	}

	virtual MyClass<CPP_TYPE> * CreateChildClass(MyClassManager * in_manager, char const * in_name) const;

	template<typename FUNC>
	requires std::invocable<FUNC, CPP_TYPE *>
	decltype(auto) CreateSubclass(FUNC && func) const;


protected:

	static MyClass* GetMutableCPPClassInstance()
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
#if 0
		if (this->parent_class != nullptr)
			this->parent_class->InitializeInstance(instance);
		DoInitializeInstance(instance);
#endif
	}

	virtual void DoInitializeInstance(CPP_TYPE* instance) const
	{
	}
};

//-----------------------------------------------------------

template<typename PARENT_CLASS, typename FUNC>
class MySubClass : public PARENT_CLASS
{
public:

	MySubClass(FUNC const & in_func):
		func(in_func)
	{
		int i = 0;
		++i;
	}

	MySubClass(FUNC && in_func) :
		func(std::move(in_func))
	{
		int i = 0;
		++i;
	}


protected:

	FUNC func;
};

//-----------------------------------------------------------

template<typename CPP_TYPE>
template<typename FUNC>
requires std::invocable<FUNC, CPP_TYPE*>
decltype(auto) MyClass<CPP_TYPE>::CreateSubclass(FUNC&& func) const
{
	class R : public MyClass<CPP_TYPE>
	{
	
	};

	return new R;
}




//-----------------------------------------------------------

class MyClassFindResult
{
	friend class MyClassManager;

	using iterator_type = std::vector<shared_ptr<MyClassBase>>::iterator;

public:

	template<typename CPP_TYPE>
	operator MyClass<CPP_TYPE> * () const;

	operator MyClassBase * () const;


	/** check whether the result is empty */
	bool operator == (nullptr_t) const;
	/** check whether the result is empty */
	bool operator != (nullptr_t) const;

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

bool MyClassFindResult::operator == (nullptr_t) const
{
	return (result == nullptr) && (class_manager == nullptr); // no cached result, nothing more starting point for searching
}

bool MyClassFindResult::operator != (nullptr_t) const
{
	return !operator == (nullptr);
}

//-----------------------------------------------------------

class MyClassManager : public Object
{
	friend class MyClassFindResult;
	friend class MyClassLoader;

	template<typename CPP_TYPE>
	friend class MyClass;

public:

	/** constructor */
	MyClassManager(MyClassManager* in_parent_manager = nullptr);

	/** destructor */
	~MyClassManager();

	/** gets the parent manager */
	MyClassManager* GetParentManager() { return parent_manager.get(); }
	/** gets the parent manager */
	MyClassManager const* GetParentManager() const { return parent_manager.get(); }

	/** search whether manager is an ancestor of another manager */
	bool IsAncestorManagerFor(MyClassManager const* child_manager, bool accept_same = true) const;

	/** search for a class with miscelleous criteria. return intermediate for last moment class requirement resolution */
	MyClassFindResult FindClass(char const* name, FindClassFlags flags = FindClassFlags::All);

protected:

	/** insert a new class in this manager */
	void InsertClass(MyClassBase* cls);

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

bool MyClassManager::IsAncestorManagerFor(MyClassManager const* child_manager, bool accept_same) const
{
	assert(child_manager != nullptr);

	if (child_manager == this)
		return accept_same;

	MyClassManager const * target_manager = child_manager->parent_manager.get();
	while (target_manager != nullptr)
	{
		if (target_manager == this)
			return true;
		target_manager = target_manager->parent_manager.get();
	}
	return false;
}

void MyClassManager::InsertClass(MyClassBase* cls)
{
	assert(cls != nullptr);
	assert(cls->manager == nullptr);

	classes.push_back(cls);
	cls->manager = this;
}


template<typename CPP_TYPE>
MyClass<CPP_TYPE>* MyClass<CPP_TYPE>::CreateChildClass(MyClassManager* in_manager, char const* in_name) const
{
	if (in_manager != nullptr)
	{
		// search if target manager is a child manager
		// (bad idea to have an ancestor manager having a derived class from one's child manager)
		if (this->manager != nullptr)
			if (in_manager->IsAncestorManagerFor(this->manager, false))
				return nullptr;
		// search if there already is a class with same name in target manager (do not search in parent managers)
		if (in_manager->FindClass(in_name, FindClassFlags::Name) != nullptr)
			return nullptr;
	}

	MyClass<CPP_TYPE>* result = new MyClass<CPP_TYPE>();
	if (result == nullptr)
		return nullptr;

	result->name = in_name;
	result->class_size = this->class_size;
	result->info = this->info;
	result->parent_class = this;
	result->manager = in_manager;

	if (in_manager != nullptr)
		in_manager->InsertClass(result);
	return result;
};

//-----------------------------------------------------------


template<typename CPP_TYPE>
MyClassFindResult::operator MyClass<CPP_TYPE> * () const
{
	// check for cached result (class_manager == nullptr -> can use result as a cached result (even if null))
	if (result != nullptr || class_manager == nullptr)
		return (MyClass<CPP_TYPE>*)result;

	// check for the very first entry (string comparaison already done)
	if ((*iterator)->InheritsFrom<CPP_TYPE>(true))
	{
		result = iterator->get();
		class_manager = nullptr;
		return (MyClass<CPP_TYPE>*)result;
	}

	// we know that the iterator points on a valid entry. get the string that made this entry a good one
	std::string const& searched_name = (match_type == ClassMatchType::Name) ?
		iterator->get()->GetClassName() :
		iterator->get()->GetShortName();

	++iterator; // no need to test again for this iterator

	if (iterator == class_manager->classes.end())
	{
		if (class_manager->GetParentManager() == nullptr || !HasAnyFlags(find_flags, FindClassFlags::ParentManager))
		{
			result = nullptr;
			class_manager = nullptr;
			return nullptr;
		}
		class_manager = class_manager->GetParentManager();
		iterator      = class_manager->classes.begin();
	}

	// search in manager chain
	while (class_manager != nullptr)
	{
		while (iterator != class_manager->classes.end())
		{
			if (HasAnyFlags(find_flags, FindClassFlags::Name))
			{
				if (StringTools::Stricmp((*iterator)->GetClassName(), searched_name) == 0)
				{
					if ((*iterator)->InheritsFrom<CPP_TYPE>(true))
					{
						result = iterator->get(); // cache the result for next call
						return (MyClass<CPP_TYPE>*)result;
					}
					++iterator;
					continue; // the class does not match no matter what. no need to check for the short name
				}
			}
			if (HasAnyFlags(find_flags, FindClassFlags::Shortname))
			{
				if (StringTools::Stricmp((*iterator)->GetShortName(), searched_name) == 0)
				{
					if ((*iterator)->InheritsFrom<CPP_TYPE>(true))
					{
						result = iterator->get(); // cache the result for next call
						return (MyClass<CPP_TYPE>*)result;
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
	if (result != nullptr || class_manager == nullptr)
		return result;

	if (iterator != class_manager->classes.end())
		return iterator->get();

	return nullptr;
}

//-----------------------------------------------------------

template<typename CPP_TYPE>
class MyCppClassRegisterResult
{
public:

	MyCppClassRegisterResult(MyClass<CPP_TYPE>* in_target_class):
		target_class(in_target_class){}

	/** set the short name for the class */
	MyCppClassRegisterResult& ShortName(std::string in_short_name)
	{
		target_class->SetShortName(std::move(in_short_name));
		return *this;
	}

	/** convert the registration to the class */
	operator MyClass<CPP_TYPE> * () const
	{
		return target_class;
	}

protected:

	MyClass<CPP_TYPE> * target_class = nullptr;
};


//-----------------------------------------------------------

class MyCPPClassManager : public MyClassManager, public Singleton<MyCPPClassManager>
{
public:

	template<typename CPP_TYPE>
	MyCppClassRegisterResult<CPP_TYPE> RegisterCPPClass(char const * name)
	{
		MyClass<CPP_TYPE>* result = MyClass<CPP_TYPE>::GetMutableCPPClassInstance();
		if (result->IsFullyInitialized()) // class has already be registered ?
		{
			assert(StringTools::Stricmp(result->name, name) == 0); // ensure class is registered with same name
			return { result };
		}

		result->name = name;
		result->class_size = sizeof(CPP_TYPE);
		result->info = &typeid(CPP_TYPE);
		result->manager = this;
		if constexpr (HasSuperType<CPP_TYPE>)
			result->parent_class = MyClass<SuperClass_t<CPP_TYPE>>::GetCPPClassInstance();

		classes.push_back(result);
		return { result };
	}
};












#if 0

//-----------------------------------------------------------

class CHAOS_API MyClassWithJSONInitialization : public Class
{
	friend class ClassLoader;

protected:

	/** constructor */
	ClassWithJSONInitialization(std::string in_name, nlohmann::json in_json);

	/** override */
	virtual void OnObjectInstanceInitialized(Object* object) const override;

protected:

	/** the data to apply at object instance creation */
	nlohmann::json json;
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

MyClassBase* MyClassLoader::DoCreateSpecialClass(MyClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json) const
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

#endif





//-----------------------------------------------------------

class A;



class MyClassABase : public MyClassBase
{
	using InitializationFunction = std::function<void(A *)>;

public:

	MyClassABase()
	{
		int i = 0;
		++i;
	}

	virtual A * CreateInstance() const = 0;





protected:

	//virtual void DoInitializeInstance(A * instance) const override
	//{
	//}



protected:


	InitializationFunction initialization_function;
};

template<>
struct MyClassSuper<A>
{
	using type = MyClassABase;
};



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




// ------------------------------------------------------------

class ClassManagerBase;
class CustomClassManager;
class NativeClassManager;

class ClassManagerBase
{
public:

	CustomClassManager* CreateChildManager() const;
};

class CustomClassManager : public ClassManagerBase
{
public:

	


};

class NativeClassManager : public ClassManagerBase, public Singleton<NativeClassManager>
{
	// no real class insertion method




};

CustomClassManager * ClassManagerBase::CreateChildManager() const
{
	return nullptr;
}

class Truc 
{
public:


	Truc()
	{
		int i = 0;
		++i;
	}

};

// ------------------------------------------------------------

int main(int argc, char ** argv, char ** env)
{

	MyClass<A> const * aaa = MyClass<A>::GetCPPClassInstance();
	

	auto XXX = [](A * a)
	{
		a = a;
		
		
	};

	MyClass<A> const * subclass = aaa->CreateSubclass(XXX)->CreateSubclass([](A * a)
	{
		a = a;
		
	});

	A * res = subclass->CreateInstance();








	MyClass<Truc> const * truc_class = MyCPPClassManager::GetInstance()->RegisterCPPClass<Truc>("truc");

	Truc * truc = truc_class->CreateInstance();


	std::function<void(A*)> myfa = [](A*)
		{
			int i = 0;
			++i;
			return 0;
		};

	std::function<void(B*)> myfb = myfa;


	A * a = nullptr;
	B * b = nullptr;

	//myf(a);
	myfb(b);
		














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