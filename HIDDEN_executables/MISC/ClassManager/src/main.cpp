#include "chaos/Chaos.h"

class BaseClass : public chaos::Object, public chaos::JSONSerializableInterface
{
public:

	CHAOS_DECLARE_OBJECT_CLASS(BaseClass, chaos::Object)("BC"); // with a short name

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

//
//   BaseClass           ???     Unknown (does not exist)
//       ^                ^               ^
//       |                |               |
//   ChildClass        NoParent     UnknownParent
//       ^                                ^
//       |                                |
//   ChildClass2                  BrokenInheritance1
//                                        ^
//                                        |
//                                BrokenInheritance2
//

class MyApplication : public chaos::Application
{
protected:

	virtual int Main() override
	{
		chaos::SubClassOf<BaseClass> c1 = chaos::ClassManager::GetDefaultInstance()->FindClass("ChildClass");
		BaseClass* object1 = c1.CreateInstance();

		assert(object1 != nullptr);
		assert(object1->value == 666);

		chaos::SubClassOf<BaseClass> c2 = chaos::ClassManager::GetDefaultInstance()->FindClass("BC");
		BaseClass* object2 = c2.CreateInstance();

		assert(object2 != nullptr);
		assert(object2->value == 13);

		chaos::SubClassOf<BaseClass> c3 = chaos::ClassManager::GetDefaultInstance()->FindClass("ChildClass2");
		BaseClass* object3 = c3.CreateInstance();

		assert(object3 != nullptr);
		assert(object3->value == 777);

		return 0;
	}
};



int main(int argc, char ** argv, char ** env)
{
	//Cls c = Cls([]() {return new chaos::Object; });




	return chaos::RunApplication<MyApplication>(argc, argv, env);
}