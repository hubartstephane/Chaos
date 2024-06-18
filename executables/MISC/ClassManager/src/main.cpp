#include "chaos/Chaos.h"

class BaseClass : public chaos::Object, public chaos::JSONSerializableInterface
{
public:

	CHAOS_DECLARE_OBJECT_CLASS(BaseClass, chaos::Object);

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
		chaos::SubClassOf<BaseClass> c = chaos::ClassManager::GetDefaultInstance()->FindClass("ChildClass");


		BaseClass* object = c.CreateInstance();

		return 0;
	}
};




int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}