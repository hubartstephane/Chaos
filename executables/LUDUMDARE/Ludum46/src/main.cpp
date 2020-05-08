#include <chaos/StandardHeaders.h> 
#include <chaos/Metaprogramming.h> 
#include <death/Death.h> 

#include "Ludum46Game.h"


class Object
{
public:

	virtual ~Object() = default;
};

class A : public Object
{

};

class B : public Object
{

};

class C : public A
{

};


// ===========================================================




class ObjectFactory
{
public:

	virtual Object* Create() { return nullptr; }

};

template<typename T>
class TypedObjectFactory : public ObjectFactory
{
public:

	virtual Object* Create() override { return new T; }

};

class FactoryGroup
{
public:

	std::map<std::string, ObjectFactory *> factories;

};


// ===========================================================
template<typename T>
bool DeclareClass(T * fake_ptr, char const * name)
{
	// get the group for this class
	FactoryGroup* factory_group = GetFactoryGroup(fake_ptr);
	if (factory_group == nullptr)
		return false;
	// check whether the class is already registered
	auto it = factory_group->factories.find(name);
	if (it != factory_group->factories.end())
		return false;
	// register the class
	factory_group->factories[name] = new TypedObjectFactory<T>();

	return true;
}

// ===========================================================

FactoryGroup * GetFactoryGroup(...)
{
	return nullptr;
}

#define DECLARE_CLASS_GROUP(baseclassname)\
FactoryGroup * GetFactoryGroup(baseclassname *)\
{\
	static FactoryGroup result;\
	return &result;\
}

#define DECLARE_CLASS(classname)\
namespace declared_class\
{\
	inline bool declared_class_##classname = DeclareClass((classname*)nullptr, #classname);\
};

DECLARE_CLASS_GROUP(A)
DECLARE_CLASS(A)
DECLARE_CLASS(B)
DECLARE_CLASS(C)


 template<typename T>
 T* CreateClassInstance(T*& result, char const* classname)
 {
	 result = nullptr;

	 FactoryGroup* factory_group = GetFactoryGroup(result);
	 if (factory_group != nullptr)
	 {
		 auto it = factory_group->factories.find(classname);
		 if (it != factory_group->factories.end())
		 {
			 Object* object = it->second->Create();
			 if (object != nullptr)
			 {
				 result = chaos::auto_cast(object);
				 if (result == nullptr)
					 delete(object);
			 }
		 }
	 }
	 return result;
 }













int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	A* a = nullptr;
	B* b = nullptr;
	C* c = nullptr;

	void* p = nullptr;

	A* a = dynamic_cast<A*>(p);



	auto pA = CreateClassInstance(a, "A");
	auto pB = CreateClassInstance(b, "B");
	auto pC = CreateClassInstance(c, "C");

	return 0;

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
