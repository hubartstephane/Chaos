#include <chaos/Application.h>
#include <chaos/LuaTools.h>
#include <chaos/FileTools.h>
#include <chaos/Buffer.h>
#include <chaos/WinTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{
  Application * Application::singleton_instance = nullptr;

  Application::Application()
  {
    assert(singleton_instance == nullptr);
    singleton_instance = this; // well this should be a singleton
  }

  Application::~Application()
  {
    assert(singleton_instance == this);
    singleton_instance = nullptr;
  }

  nlohmann::json Application::LoadConfigurationFile()
  {
    boost::filesystem::path configuration_path = GetResourcesPath() / "config.json";

    return JSONTools::LoadJSONFileRecursive(configuration_path);
  }

  bool Application::Initialize()
  {
    chaos::WinTools::AllocConsoleAndRedirectStdOutput();
    if (!InitializeManagers())
      return false;
    return true;
  }

  bool Application::Finalize()
  {
    FinalizeManagers();
    return true;
  }

  bool Application::Main()
  {
    return true;
  }

  bool Application::Run(int argc, char ** argv, char ** env)
  {
    bool result = false;
    if (InitializeStandardLibraries())
    {
      // store a copy of the parameters
      StoreParameters(argc, argv, env);

      // load the configuration file
      configuration = LoadConfigurationFile();

      // initialize, run, and finalize the application
      if (Initialize())
        result = Main();
      // finalization (even if initialization failed)
      Finalize();
      FinalizeStandardLibraries();
    }
    return result;
  }

  bool Application::InitializeStandardLibraries()
  {
    return true;
  }

  bool Application::FinalizeStandardLibraries()
  {
    return true;
  }

  bool Application::InitializeManagers()
  {
    // generate the file manager
    file_manager = new FileManager;
    if (file_manager == nullptr)
      return false;

    return true;
  }

  bool Application::FinalizeManagers()
  {
    file_manager = nullptr;
    return true;
  }

  void Application::StoreParameters(int argc, char ** argv, char ** env)
  {
    assert(argc > 0); // should at least contains the fullpath of the application

    if (argv != nullptr)
      for (int i = 0; i < argc; ++i)
        arguments.push_back(argv[i]);

    if (env != nullptr)
    {
      for (int i = 0; env[i] != nullptr; ++i)
      {
        char const * separator = strchr(env[i], '=');
        if (separator != nullptr)
        {
          std::string key = std::string(env[i], separator - env[i]);
          std::string value = std::string(separator + 1);
          environments.push_back(std::make_pair(key, value));
        }
      }
    }

    boost::filesystem::path p = argv[0];
    p.normalize();
    p.make_preferred();

    application_path = p.parent_path();
    application_filename = p.filename();
    resources_path = application_path / "resources";
    userlocal_path = WinTools::GetUserLocalPath() / application_filename;
    userlocal_path.replace_extension();
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
