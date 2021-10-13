#include <chaos/Chaos.h>

#include "Ludum49Game.h"

class ClassManager;

// ===========================================================

template<typename T>
class Singleton
{
public:

	static T* GetInstance()
	{
		static T single_instance;
		return &single_instance;
	}

	static T const* GetConstInstance()
	{
		return GetInstance();
	}
};

// ===========================================================

class MyClass
{
public:

	/** the parent of the class */
	Class const* parent = nullptr;
	/** get class size */
	size_t class_size = 0;
	/** the optional name of the class */
	std::string class_name;
	/** whether the class has been fully declared */
	bool declared = false;
	/** create an instance of the object delegate */
	std::function<Object* ()> create_instance_func;
	/** delegate to create a temporary instance of the object on the stack and call a functor on it */
	std::function<void(std::function<void(Object*)>)> create_instance_on_stack_func;
	/** additionnal initialization for JSONSerializable objects */
	nlohmann::json json_data;
};

// ===========================================================


class ClassRegistration
{
	friend class ClassManager;

protected:

	/** constructor (may only be called from ClassManager) */
	ClassRegistration(ClassManager * in_manager, MyClass * in_class_ptr):
		manager(in_manager),
		class_ptr(in_class_ptr)
	{
		assert(in_manager != nullptr);
	}

public:

	template<typename... PARAMS>
	ClassRegistration operator()(char const * extra_name, PARAMS... params)
	{
		assert(extra_name != nullptr);

		//manager->RegisterAdditionnalName(class_ptr, extra_name);
		if constexpr (sizeof...(params) > 0)
			return operator()(params...);
		return *this;
	}

	/** convert the registration to the class */
	operator MyClass * ()
	{
		return class_ptr;
	}

protected:

	/** the manager */
	ClassManager* manager = nullptr;
	/** the class */
	MyClass* class_ptr = nullptr;

};

//chaos::SubclassOf<A> sub_class;


class ClassManager : public Singleton<ClassManager>
{
	friend class ClassRegistration;

public:

	/** register a class into the manager */
	template<typename CLASS, typename PARENT_CLASS = chaos::EmptyClass>
	ClassRegistration DeclareClass(char const* classname)
	{
		static MyClass cls;




		return { this, &cls };
	}

protected:

	void RegisterAdditionnalName(MyClass* class_ptr, char const* extra_name)
	{
		assert(extra_name != nullptr);




	}

	MyClass* FindClass(char const* name)
	{
		return nullptr;
	}

public:

	

protected:

	/** the declared classes */	
	std::vector<MyClass*> classes;

	/** sorted for searched by name */
	std::multimap<std::string, MyClass*> sorted_classes;
};

class ClassManagerLoader
{

};





//#define CHAOS_REGISTER_CLASS1(classname) inline chaos::Class const * classname##_class = chaos::Class::DeclareClass<classname>(#classname);
//#define CHAOS_REGISTER_CLASS2(classname, parent_classname) inline chaos::Class const * classname##_class = chaos::Class::DeclareClass<classname, parent_classname>(#classname);
//

#define REGISTER_CLASS(CLASS, ...) MyClass * cls = ClassManager::GetInstance()->DeclareClass<CLASS, __VA_ARGS__>(#CLASS)


class A {};
class B {};


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{



	auto ppp = std::make_pair("0", 1);

	std::multimap<std::string, int> mm;
	mm.emplace("0", 1);
	mm.emplace("0", 2);
	mm.emplace("0", 3);
	mm.emplace("1", 10);
	mm.emplace("1", 20);
	mm.emplace("1", 30);
	mm.emplace("2", 100);
	mm.emplace("2", 200);
	mm.emplace("2", 300);
/*
	mm.insert(0, 2);
	mm.insert(0, 3);
	mm.insert(1, 10);
	mm.insert(1, 20);
	mm.insert(1, 30);
	mm.insert(2, 100);
	mm.insert(2, 200);
	mm.insert(2, 300);
	*/
	auto it1 = mm.lower_bound("1");
	auto it2 = mm.upper_bound("1");

	for (auto it = it1; it != it2; ++it)
	{
		int value = it->second;

		value = value;
		

	}




	ClassManager manager;

	MyClass* mc0 = manager.DeclareClass<A>("A");
	MyClass* mc1 = manager.DeclareClass<A>("A")("myA");
	MyClass* mc2 = manager.DeclareClass<A>("A")("myA", "myB");

	char const* filename = __FILE__;
	int l = __LINE__;

	{REGISTER_CLASS(A, B); }
	{REGISTER_CLASS(A, B)("myA"); }
	{REGISTER_CLASS(A, B)("myA", "myB"); }


	WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	WindowHints hints;

	return RunGame<LudumGame>(argc, argv, env, params, hints);
}
