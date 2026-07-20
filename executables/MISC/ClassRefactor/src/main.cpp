#include "chaos/Chaos.h"
#include <Windows.h>
#include <iostream>
#include <streambuf>

using namespace chaos;

//-----------------------------------------------------------

class DebugOutputBuf : public std::streambuf
{
protected:


	virtual std::streamsize xsputn(const char* s, std::streamsize n) override
	{
		OutputDebugStringA(std::string(s, n).c_str());
		return n;
	}
	 
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
	friend class MyNativeClassManager;
	friend class MyClassLoader;

	template<typename CPP_TYPE>
	friend class MyCClassRegisterResult;

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

	/** change the shortname of the class (just used at construction) */
	void SetShortName(std::string in_shorname);

	/** check whether we inherit from another class */
	bool InheritsFrom(MyClassBase const * base, bool accept_same = true) const;
	/** check whether we inherit from another class */
	template<typename CPP_TYPE>
	bool InheritsFrom(bool accept_same = true) const;

protected:

	/** initialize newly created instance (at this point type of 'instance' is unknown) */
	void InitializeInstance(void * instance) const
	{
		if (parent_class != nullptr)
			parent_class->InitializeInstance(instance);
		if (initialize_instance_func)
			initialize_instance_func(instance);
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
	MyClassBase const * parent_class = nullptr;
	/** the manager for this class */
	MyClassManager* manager = nullptr;
	/** function to initialize newly created instance */
	std::function<void(void*)> initialize_instance_func;
};

void MyClassBase::SetShortName(std::string in_shorname)
{ 
	assert(short_name.empty()); // this should only be called/set a single time
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
	return InheritsFrom(MyClass<CPP_TYPE>::GetNativeClassInstance(), accept_same);
}

//-----------------------------------------------------------

template<typename CPP_TYPE>
struct MyClassSuperClass
{
	using type = MyClassBase;
};

template<typename CPP_TYPE>
requires
(
	HasSuperType<CPP_TYPE>
)
struct MyClassSuperClass<CPP_TYPE>
{
	using type = MyClass<SuperClass_t<CPP_TYPE>>;
};

template<typename CPP_TYPE>
using MyClassSuperClass_t = typename MyClassSuperClass<CPP_TYPE>::type;








//-----------------------------------------------------------

template<typename CPP_TYPE>
class MyClass : public MyClassSuperClass_t<CPP_TYPE>
{
	friend class MyClassManager;
	friend class MyNativeClassManager;

	template<typename CPP_TYPE>
	friend class MyCppClassRegisterResult;

public:

	using Super = MyClassSuperClass_t<CPP_TYPE>;

	virtual CPP_TYPE* CreateInstance(void * inplace_buffer = nullptr) const
	{
		assert(this->IsFullyInitialized());
		CPP_TYPE* result = AllocateInstance(inplace_buffer);
		if (result != nullptr)
			this->InitializeInstance(result);
		return result;
	}

	static MyClass const * GetNativeClassInstance()
	{
		return GetMutableNativeClassInstance();
	}

	virtual MyCppClassRegisterResult<CPP_TYPE> CreateSubclass(MyClassManager* in_manager, std::string in_name, std::function<void(CPP_TYPE *)> in_func = {}) const;

protected:

	MyClass() = default;

	static MyClass* GetMutableNativeClassInstance()
	{
		static shared_ptr<MyClass<CPP_TYPE>> result = new MyClass<CPP_TYPE>;
		return result.get();
	}

	virtual CPP_TYPE* AllocateInstance(void* inplace_buffer) const
	{
		if (inplace_buffer == nullptr)
			return new CPP_TYPE;
		return new (inplace_buffer) CPP_TYPE;
	}

	virtual MyClass<CPP_TYPE>* CreateChildClass(MyClassManager* in_manager, std::string in_name) const;
};

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



// --------------------------------------------------------------------------------------------------------------

template<typename CPP_TYPE>
MyClass<CPP_TYPE>* MyClass<CPP_TYPE>::CreateChildClass(MyClassManager* in_manager, std::string in_name) const
{
	if (in_manager != nullptr)
	{
		// search if target manager is a child manager
		// (bad idea to have an ancestor manager having a derived class from one's child manager)
		if (this->manager != nullptr)
		{
			if (in_manager->IsAncestorManagerFor(this->manager, false))
			{
				ClassLog::Error("Can't create a child class in an ancestor ClassManager");
				return nullptr;
			}
		}
		// search if there already is a class with same name
		if (in_manager->FindClass(in_name.c_str(), FindClassFlags::Name | FindClassFlags::ParentManager) != nullptr) // ignore short name
		{
			ClassLog::Error("Can't load class [%s]: already existing in manager", in_name);
			return nullptr;
		}
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

	MyCppClassRegisterResult(MyClass<CPP_TYPE>* in_target_class = nullptr):
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

	MyClass<CPP_TYPE> * operator -> () const
	{
		return target_class;
	}

protected:

	MyClass<CPP_TYPE> * target_class = nullptr;
};


//-----------------------------------------------------------

class MyNativeClassManager : public MyClassManager, public Singleton<MyNativeClassManager>
{
public:

	template<typename CPP_TYPE>
	MyCppClassRegisterResult<CPP_TYPE> RegisterNativeClass(char const * name)
	{
		MyClass<CPP_TYPE>* result = MyClass<CPP_TYPE>::GetMutableNativeClassInstance();
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
			result->parent_class = MyClass<SuperClass_t<CPP_TYPE>>::GetNativeClassInstance();

		classes.push_back(result);
		return { result };
	}
};


//-----------------------------------------------------------

template<typename CPP_TYPE>
MyCppClassRegisterResult<CPP_TYPE> MyClass<CPP_TYPE>::CreateSubclass(MyClassManager* in_manager, std::string in_name, std::function<void(CPP_TYPE*)> in_func) const
{
	MyClass<CPP_TYPE>* result = CreateChildClass(in_manager, std::move(in_name));
	if (result == nullptr)
		return {};

	if (in_func)
	{
		result->initialize_instance_func = [func = std::move(in_func)](void* instance)
		{
			func((CPP_TYPE*)instance);
		};
	}
	return { result };
}











//-----------------------------------------------------------

class CHAOS_API MyClassLoader
{
public:

	/** load one class by its path */
	virtual MyClass<Object>* LoadClass(MyClassManager* manager, FilePathParam const& path) const;
	/** load all classes in one directory */
	virtual bool LoadClassesInDirectory(MyClassManager* manager, FilePathParam const& path) const;

protected:

	/* an utility function to load a JSON file a find the approriate classname */
	bool DoLoadClassHelper(FilePathParam const& path, LightweightFunction<bool(std::string, std::string, std::string, nlohmann::json)> func) const;

	/** create a child class */
	MyClass<Object>* DoCreateChildClass(MyClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json, char const * parent_class_name) const;
	/** create a child class */
	MyClass<Object>* DoCreateChildClassHelper(MyClass<Object>* parent_class, MyClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json) const;
};




bool MyClassLoader::DoLoadClassHelper(FilePathParam const& path, LightweightFunction<bool(std::string, std::string, std::string, nlohmann::json)> func) const
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
			return false;
		}
		// create the class
		if (!class_name.empty())
		{
			std::string short_name;
			JSONTools::GetAttribute(&json, "short_name", short_name);
			return func(std::move(class_name), std::move(short_name), std::move(parent_class_name), std::move(json));
		}
	}
	return false;
}

MyClass<Object>* MyClassLoader::LoadClass(MyClassManager* manager, FilePathParam const& path) const
{
	assert(manager != nullptr);

	MyClass<Object>* result = nullptr;
	DoLoadClassHelper(path, [this, manager, &result](std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json)
	{
		result = DoCreateChildClass(manager, std::move(class_name), std::move(short_name), std::move(json), parent_class_name.c_str());
		return (result != nullptr);
	});
	return result;
}

MyClass<Object>* MyClassLoader::DoCreateChildClass(MyClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json, char const * parent_class_name) const
{
	// check parameter and not already registered
	assert(!StringTools::IsEmpty(class_name));

	if (manager->FindClass(class_name.c_str(), FindClassFlags::Name) != nullptr)
	{
		ClassLog::Error("ClassLoader::DoCreateChildClass(...): class already existing [%s]", class_name.c_str());
		return nullptr;
	}

	MyClass<Object> * parent_class = manager->FindClass(parent_class_name);
	if (parent_class == nullptr)
	{
		ClassLog::Error("ClassLoader::DoCreateChildClass(...): can't find parent class [%s] derived from Object", parent_class_name);
		return nullptr;
	}

	return DoCreateChildClassHelper
	(
		parent_class,
		manager,
		std::move(class_name),
		std::move(short_name),
		std::move(json)
	);
}

MyClass<Object>* MyClassLoader::DoCreateChildClassHelper(MyClass<Object>* parent_class, MyClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json) const
{
	MyClass<Object> * result = parent_class->CreateSubclass(manager, std::move(class_name), [json = std::move(json)](Object * object)
	{
		if (JSONSerializableInterface* serializable = auto_cast(object))
			serializable->SerializeFromJSON(&json);
	});

	if (result == nullptr)
		return nullptr;

	if (!StringTools::IsEmpty(short_name))
		result->SetShortName(std::move(short_name));
	manager->InsertClass(result);

	return result;
}












struct LoadingClassInfo
{
	/** the name of the class to create */
	std::string class_name;
	/** the shortname of the class to create */
	std::string short_name;
	/** the name of the parent_class to create */
	std::string parent_class_name;
	/** the json data to be used to initialize new instances */
	nlohmann::json json;

	/** the parent class_info */
	LoadingClassInfo* parent_class_info = nullptr;
	/** the parent class */
	MyClass<Object> * parent_class = nullptr;
	/** the effective class created */
	MyClass<Object> * loaded_class = nullptr;

	/** the level in hierarchy of class. Higher values must be treated first */
	int  level = 0;
	/** whether this info is valid, or is to be rejected */
	bool is_valid = true;
	/** used to detect circular classes hierarchy */
	bool traversal_in_progress = false;
};

void SearchCircularReferenceMarkInvalid(LoadingClassInfo* cls, bool & error_circular_reference)
{
	if (cls == nullptr)
		return;
	else if (!cls->is_valid)
		return;
	else if (cls->traversal_in_progress)
	{
		cls->is_valid = false;
		error_circular_reference = true;
	}
	else
	{
		cls->traversal_in_progress = true;
		SearchCircularReferenceMarkInvalid(cls->parent_class_info, error_circular_reference);
		cls->traversal_in_progress = false;
	}
}

bool MyClassLoader::LoadClassesInDirectory(MyClassManager* manager, FilePathParam const& path) const
{
	assert(manager != nullptr);

	std::vector<LoadingClassInfo> loaded_classes_info;

	FileTools::WithDirectoryContent(path, [this, &loaded_classes_info](boost::filesystem::path const& p)
	{
		DoLoadClassHelper(p, [this, &loaded_classes_info](std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json)
		{
			LoadingClassInfo class_info;
			class_info.class_name = std::move(class_name);
			class_info.short_name = std::move(short_name);
			class_info.parent_class_name = std::move(parent_class_name);
			class_info.json = std::move(json);
			loaded_classes_info.push_back(std::move(class_info));
			return true;
		});
		return false; // do not stop iterating in directory
	});

	if (loaded_classes_info.size() == 0)
		return true;

	// generate indices
	std::vector<size_t> indices;
	indices.reserve(loaded_classes_info.size());
	for (size_t i = 0 ; i < loaded_classes_info.size() ; ++i)
		indices.push_back(i);

	auto RemoveInvalidClasses = [&indices, &loaded_classes_info](LightweightFunction<void(LoadingClassInfo const & class_info)> log_func)
	{
		std::erase_if(indices, [&loaded_classes_info, &log_func](size_t index)
		{
			if (!loaded_classes_info[index].is_valid)
			{
				log_func(loaded_classes_info[index]);
				return true;
			}
			return false;
		});
	};

	// search classes already existing in manager
	bool error_already_existing = false;
	for (size_t i = 0 ; i < indices.size() ; ++i)
	{
		LoadingClassInfo& class_info = loaded_classes_info[indices[i]];

		if (manager->FindClass(class_info.class_name.c_str(), FindClassFlags::Name) != nullptr) // ignore short name and parent manager
		{
			class_info.is_valid = false;
			error_already_existing = true;
		}	
	}

	if (error_already_existing)
	{
		RemoveInvalidClasses([](LoadingClassInfo const & class_info)
		{
			ClassLog::Error("Loading class [%s] failure: already exists in manager", class_info.class_name.c_str());
		});
	}

	// link child-parent alltogether
	for (size_t index1 : indices)
	{
		LoadingClassInfo& class_info1 = loaded_classes_info[index1];
		for (size_t index2 : indices)
		{
			if (index1 == index2)
				continue;
			LoadingClassInfo& class_info2 = loaded_classes_info[index2];
			if (class_info1.parent_class_name == class_info2.class_name)
			{
				class_info1.parent_class_info = &class_info2;
				break;
			}
		}
	}

	// search real base class from manager
	bool error_base_class_not_found = false;
	for (size_t index : indices)
	{
		LoadingClassInfo& class_info = loaded_classes_info[index];

		if (class_info.parent_class_info != nullptr) // we need to find an already exising base class ?
			continue;
		if (MyClass<Object> * parent_class = manager->FindClass(class_info.parent_class_name.c_str()))
		{
			class_info.parent_class = parent_class;
		}
		else
		{
			class_info.is_valid = false;
			error_base_class_not_found = true;
		}
	}

	if (error_base_class_not_found)
	{
		RemoveInvalidClasses([](LoadingClassInfo const & class_info)
		{
			ClassLog::Error("Loading class [%s] failure: parent [%s] not found", class_info.class_name.c_str(), class_info.parent_class_name.c_str());
		});
	}

	// eliminate circular referenced classes
	bool error_circular_reference = false;
	for (size_t index : indices)
	{
		LoadingClassInfo& class_info = loaded_classes_info[index];
		SearchCircularReferenceMarkInvalid(&class_info, error_circular_reference);
	}

	if (error_circular_reference)
	{
		RemoveInvalidClasses([](LoadingClassInfo const & class_info)
		{
			ClassLog::Error("Loading class [%s] failure: circular reference", class_info.class_name.c_str());
		});
	}

	// propagate invalidity
	bool error_propagation = false;

	do
	{
		error_propagation = false;

		for (size_t index : indices)
		{
			LoadingClassInfo& class_info = loaded_classes_info[index];
			if (class_info.parent_class_info != nullptr && !class_info.parent_class_info->is_valid)
			{
				class_info.is_valid = false;
				error_propagation = true;
			}
		}
		if (error_propagation)
		{
			RemoveInvalidClasses([](LoadingClassInfo const & class_info)
			{
				ClassLog::Error("Loading class [%s] failure: invalid parent [%s]", class_info.class_name.c_str(), class_info.parent_class_name.c_str());
			});
		}
	}
	while (error_propagation);

	// 'sort' classes
	// in a class is a 'parent/root' it has higher level value
	// because the node is traversed more times
	// 
	//  B inherits of A
	//
	//     A     level = 2 (because there are two traversals of this class. one starting from A, one starting from B
	//     |
	//     v
	//     B     level = 1 (a single traversal for this class. the one starting from B)
	//
	// at the end, we just have to handle classes in order of decreasing values
	auto IncrementClassDataLevel = [](LoadingClassInfo* class_info)
	{
		do
		{
			++class_info->level;
			class_info = class_info->parent_class_info;
		} 
		while (class_info != nullptr);
	};

	for (size_t index : indices)
		IncrementClassDataLevel(&loaded_classes_info[index]);

	// sort thoses indices according the level
	std::ranges::sort(indices, [&loaded_classes_info](size_t index1, size_t index2)
	{
		return loaded_classes_info[index1].level > loaded_classes_info[index2].level;
	});

	// handle class_info according to the decreasing level
	for (size_t index : indices)
	{
		LoadingClassInfo& class_info = loaded_classes_info[index];

		if (class_info.parent_class != nullptr)
		{
			class_info.loaded_class = DoCreateChildClassHelper(
				class_info.parent_class,
				manager,
				std::move(class_info.class_name),
				std::move(class_info.short_name),
				std::move(class_info.json)
			);
		}
		else if (class_info.parent_class_info != nullptr)
		{
			assert(class_info.parent_class_info->loaded_class != nullptr);

			class_info.loaded_class = DoCreateChildClassHelper(
				class_info.parent_class_info->loaded_class,
				manager,
				std::move(class_info.class_name),
				std::move(class_info.short_name),
				std::move(class_info.json)
			);
		}	
	}
	return true;
}








//-----------------------------------------------------------

class A;





//-----------------------------------------------------------


template<typename CPP_TYPE>
MyCppClassRegisterResult<CPP_TYPE> MyDeclareNativeClass(char const* name)
{
	return MyNativeClassManager::GetInstance()->RegisterNativeClass<CPP_TYPE>(name);
}

#define MY_CHAOS_DECLARE_OBJECT_CLASS(CLASS, PARENT_CLASS)\
public:\
using Super = PARENT_CLASS;\
static MyClass<CLASS> const * GetStaticClass(){ return CLASS##_class;}\
virtual MyClass<CLASS> const * GetClass() const { return CLASS##_class; }\
static inline MyClass<CLASS> const * CLASS##_class = MyDeclareNativeClass<CLASS>(#CLASS)

class A
{
protected:

	/** declare the class in the default C++ ClassManager (must be declared before the CHAOS_DECLARE_OBJECT_CLASS usage) */
	//template<typename CPP_TYPE>
	//static MyCppClassRegisterResult<CPP_TYPE> MyDeclareNativeClass(char const* name);

public:

	A() { std::cout << "A::A()" << std::endl; }

	virtual ~A() = default;

	static MyClass<A> const* GetStaticClass() { return A_class; }
	virtual MyClass<A> const* GetClass() const { return A_class; }
	static inline MyClass<A> const* A_class = MyDeclareNativeClass<A>("A");
};








class B : public A
{

public:

	MY_CHAOS_DECLARE_OBJECT_CLASS(B, A).ShortName("bbb");

	B() { std::cout << "B::B()" << std::endl; }
};







class BaseClass : public chaos::Object, public chaos::JSONSerializableInterface
{
public:

	MY_CHAOS_DECLARE_OBJECT_CLASS(BaseClass, chaos::Object).ShortName("BC"); // with a short name

	BaseClass()
	{

	}

	virtual bool SerializeIntoJSON(nlohmann::json* json) const override
	{
		if (!chaos::PrepareSaveObjectIntoJSON(json))
			return false;
		chaos::JSONTools::SetAttribute(json, "value", value);
		return true;
	}

	virtual bool SerializeFromJSON(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::JSONSerializableInterface::SerializeFromJSON(config))
			return false;
		chaos::JSONTools::GetAttribute(config, "value", value);
		return true;
	}

	int value = 13;

};














class MyApplication : public chaos::Application
{
	CHAOS_DECLARE_OBJECT_CLASS(MyApplication, chaos::Application);

protected:

	virtual int Main() override
	{
		CheckSubclass();
		CheckLoadClassesInDirectory();
		return 0;
	}

	bool CheckSubclass()
	{
		MyClass<B> const* bbb = MyClass<B>::GetNativeClassInstance();

		MyClass<A> const* subclass = bbb->CreateSubclass(nullptr, "", [](A* a)
		{
			std::cout << "CheckSubclass creation OK" << std::endl;
		});

		subclass->CreateInstance();

		return true;
	}

	bool CheckLoadClassesInDirectory()
	{
		MyClassManager new_manager(MyNativeClassManager::GetInstance());

		MyClassLoader loader;
		loader.LoadClassesInDirectory(&new_manager, GetResourcesPath() / "refactored_classes");

		return true;
	}
};



// ------------------------------------------------------------

int main(int argc, char ** argv, char ** env)
{
	DebugOutputBuf debugBuf;
	std::cout.rdbuf(&debugBuf);

	chaos::ApplicationData application_data;
	return chaos::RunApplication<MyApplication>(argc, argv, env, &application_data);
}