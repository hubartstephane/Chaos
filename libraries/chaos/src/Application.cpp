#include <chaos/Application.h>
#include <chaos/LuaTools.h>
#include <chaos/FileTools.h>
#include <chaos/Buffer.h>

namespace chaos
{
  Application * Application::singleton_instance = nullptr;

  Application::~Application()
  {
    if (configuration_state != nullptr)
    {
      lua_close(configuration_state);
      configuration_state = nullptr;
    }

    if (file_path_manager != nullptr)
    {
      delete(file_path_manager);
      file_path_manager = nullptr;
    }
  }

  bool Application::DoInitialize(int argc, char ** argv, char ** env)
  {
    assert(file_path_manager == nullptr);

    StoreParameters(argc, argv, env);

    file_path_manager = CreateFilePathManager();

    LoadConfigurationFile();
    return true;
  }

  bool Application::LoadConfigurationFile()
  {
#if 0
    Buffer<char> buffer = FileTools::LoadFile(application_path / "config.lua", true);
    if (buffer != nullptr)
    {
      configuration_state = LuaTools::CreateStandardLuaState();
      if (configuration_state != nullptr)
      {
        InitializeLuaState(configuration_state);
        LuaTools::ExecBuffer(configuration_state, buffer, false);
      }
    }
#endif
    return true;
  }

  FilePathManager * Application::CreateFilePathManager()
  {
    return new FilePathManager();
  }

  void Application::InitializeLuaState(lua_State * state)
  {
    assert(state != nullptr);

 //   FilePathManager::InitializeLuaState(state);
 //   OOLUA::set_global(state, "path", file_path_manager);
  }

  void Application::StoreParameters(int argc, char ** argv, char ** env)
  {
    assert(argc > 0); // should at least contains the fullpath of the application

    if (argv != nullptr)
      for (int i = 0 ; i < argc ; ++i)
        arguments.push_back(argv[i]);

    if (env != nullptr)
    {
      for (int i = 0 ; env[i] != nullptr ; ++i)
      {
        char const * separator = strchr(env[i], '=');
        if (separator != nullptr)
        {
          std::string key   = std::string(env[i], separator - env[i]);
          std::string value = std::string(separator + 1);                    
          environments.push_back(std::make_pair(key, value));
        }
      }
    }

    boost::filesystem::path p = argv[0];

    application_path     = p.parent_path();
    application_filename = p.filename(); 
  }

  char const * Application::GetEnvironment(char const * key) const
  {
    assert(key != nullptr);
    for (auto e : environments)
    if (_stricmp(e.first.c_str(), key) == 0)
      return e.second.c_str();
    return nullptr;
  }

}; // namespace chaos
