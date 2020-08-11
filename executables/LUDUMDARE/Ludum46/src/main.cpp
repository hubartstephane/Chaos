#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 

#include <chaos/Object.h> 
#include <chaos/JSONSerializable.h> 

#include "Ludum46Game.h"

class toto : public chaos::Object, public chaos::JSONSerializable
{
public:

	CHAOS_OBJECT_DECLARE_CLASS2(toto, chaos::Object);

	virtual bool SerializeFromJSON(nlohmann::json const& json) override;

	virtual bool SerializeIntoJSON(nlohmann::json & json) const override;

	int myvalue = 777;

};


bool toto::SerializeFromJSON(nlohmann::json const& json)
{
	if (!chaos::JSONSerializable::SerializeFromJSON(json))
		return false;

	chaos::JSONTools::GetAttribute(json, "myvalue", myvalue);

	return true;
}

bool toto::SerializeIntoJSON(nlohmann::json& json) const
{
	if (!chaos::JSONSerializable::SerializeIntoJSON(json))
		return false;

	chaos::JSONTools::SetAttribute(json, "myvalue", myvalue);

	return true;
}


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
