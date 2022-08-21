
#include "chaos/Chaos.h"

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		boost::filesystem::path const & rp = GetResourcesPath();

		int i = 1;
		while(true)
		{
			std::string filename = chaos::StringTools::Printf("test%d.lua", i);

			chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(rp / filename, chaos::LoadFileFlag::ASCII);
			if (buffer == nullptr)
				break;

			chaos::Log::Title(filename.c_str());

			lua_State * l = chaos::LuaTools::CreateStandardLuaState();
			if (l != nullptr)
			{
				chaos::LuaTools::ExecBuffer(l, buffer, false, chaos::StringTools::Printf("@%s", filename.c_str()).c_str());
				lua_close(l);
			}
			++i;
		}

		chaos::WinTools::PressToContinue();

		return true;
	}
};


int main(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


