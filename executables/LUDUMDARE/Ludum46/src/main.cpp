#include <chaos/Chaos.h>

#include "Ludum46Game.h"

class toto : public chaos::Object, public chaos::JSONSerializable
{
public:

	CHAOS_DECLARE_OBJECT_CLASS2(toto, chaos::Object)

	virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override
	{
		chaos::JSONTools::GetAttribute(json_entry, "myvalue", myvalue);

		return true;
	}

	int myvalue = 12345;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
