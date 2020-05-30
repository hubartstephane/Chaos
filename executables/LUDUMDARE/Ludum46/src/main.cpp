#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 

#include "Ludum46Game.h"



#include <chaos/Object.h> 
#include <chaos/JSONSerializable.h> 

class A : public chaos::Object, public chaos::JSONSerializable
{
	CHAOS_OBJECT_DECLARE_CLASS2(A, Object);

public:

	int i = 0;

	virtual bool LoadFromJSON(nlohmann::json const & entry) override
	{
		chaos::JSONTools::GetAttribute(entry, "i", i);

		return true;
	}

	virtual bool SaveIntoJSON(nlohmann::json & entry) const override
	{
		chaos::JSONTools::SetAttribute(entry, "i", i);
		return true;
	}

};




int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	auto a1 = std::is_class_v<int>;

	auto b1 = std::is_class_v<std::uint32_t>;



	nlohmann::json entry;

	A a;
	a.i = 666;
	chaos::SaveIntoJSON(entry, a);

	std::string ss = entry.dump();


	A* a2 = nullptr;
	chaos::LoadFromJSON(entry, a2);


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
