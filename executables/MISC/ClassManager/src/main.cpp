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

		return 0;
	}
};

#if 0
class Cls : public chaos::Class
{
public:

	Cls(std::function<chaos::Object*()> func)
	{
		int i = 0;
		++i;
	}
};
#endif




int main(int argc, char ** argv, char ** env)
{
	//Cls c = Cls([]() {return new chaos::Object; });




	return chaos::RunApplication<MyApplication>(argc, argv, env);
}