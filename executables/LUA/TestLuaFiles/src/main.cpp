
#include <chaos/LuaTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>
#include <chaos/LogTools.h>

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  boost::filesystem::path resources_path = chaos::Application::GetInstance()->GetApplicationPath() / "resources";

  int i = 1;
  while(true)
  {
    std::string filename = chaos::StringTools::Printf("test%d.lua", i);

    chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(resources_path / filename, true);
    if (buffer == nullptr)
      break;

    chaos::LogTools::DisplayTitle(filename.c_str());

    lua_State * l = chaos::LuaTools::CreateStandardLuaState();
    if (l != nullptr)
    {
      chaos::LuaTools::ExecBuffer(l, buffer, false, chaos::StringTools::Printf("@%s", filename.c_str()).c_str());
      lua_close(l);
    }
    ++i;
  }

  chaos::WinTools::PressToContinue();

  return 0;
}


