#include <chaos/StandardHeaders.h> 
#include <chaos/Metaprogramming.h> 
#include <death/Death.h> 

#include "Ludum46Game.h"

class ClassRegistryEntry
{
public:

	void* CreateInstance()
	{
		if (create_func)
			return create_func();
		return nullptr;
	}

public:

	/** the name of the class */
	std::string classname;
	/** the allocation function */
	std::function<void*()> create_func;
};

std::vector<ClassRegistryEntry*> & GetClassRegistryEntries()
{
	static std::vector<ClassRegistryEntry*> result;
	return result;
}

template<typename T>
ClassRegistryEntry * GetClassRegistryEntry(T const* fake_ptr)
{
	static ClassRegistryEntry entry;
	return &entry;
}

ClassRegistryEntry* GetClassRegistryEntry(char const* classname)
{
	assert(classname != nullptr && strlen(classname) > 0);
	for (ClassRegistryEntry * entry : GetClassRegistryEntries())
		if (chaos::StringTools::Strcmp(classname, entry->classname) == 0)
			return entry;
	return nullptr;
}

template<typename T>
ClassRegistryEntry * RegisterClassRegistryEntry(T const* fake_ptr, char const * classname)
{
	// not already registered
	assert(classname != nullptr && strlen(classname) > 0);
	assert(GetClassRegistryEntry(classname) == nullptr);

	ClassRegistryEntry * result = GetClassRegistryEntry(fake_ptr);
	result->classname = classname;
	result->create_func = []() { return new T; };

	GetClassRegistryEntries().push_back(result);
	return result;
}

#define REGISTER_CLASS(classname)\
inline ClassRegistryEntry * classname##_factory_entry = RegisterClassRegistryEntry((classname const *)nullptr, #classname);









class A
{
public:


	A() 
	{
	
	}

};

REGISTER_CLASS(A);






#if 0

 template<typename T>
 bool StoreToJSON(nlohmann::json & json_entry, T const* object)
 {
	 char const* classname = GetClassName(object);

	 if (!json_entry.is_object())
		 json_entry = nlohmann::json::object();

	 chaos::JSONTools::SetAttribute(json_entry, "__classname__", classname);
	 return true;
 }

 template<typename T>
 bool ReadFromJSON(nlohmann::json const & json_entry, T * & object)
 {
	 if (!json_entry.is_object())
		 return false;

	 if (object != nullptr)
		 return DoReadFromJSON(json_entry, object);

	 std::string classname;
	 if (chaos::JSONTools::GetAttribute(json_entry, "__classname__", classname))
	 {
		 object = CreateClass<T>(classname);
		 if (object )


	 }
	 else
	 {
		 return DoReadFromJSON(json_entry, object);
	 }
	 return true;
 }


#endif



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	GetClassRegistryEntry("A")->CreateInstance();

	GetClassRegistryEntry((A*)nullptr)->CreateInstance();



	return 0;

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
