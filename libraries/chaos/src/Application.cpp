#include <chaos/Application.h>
#include <chaos/LuaTools.h>
#include <chaos/FileTools.h>
#include <chaos/Buffer.h>
#include <chaos/WinTools.h>

namespace chaos
{
  Application * Application::singleton_instance = nullptr;

  void Application::DoFinalize()
  {

  }

  void Application::Finalize()
  {
    if (singleton_instance != nullptr)
    {
      singleton_instance->DoFinalize();
      delete(singleton_instance);
      singleton_instance = nullptr;
      FinalizeStandardLibraries();
    }
  }

  void Application::FreeImageOutputMessageFunc(FREE_IMAGE_FORMAT fif, const char *msg)
  {

  }

  void Application::InitializeStandardLibraries()
  {
    FreeImage_Initialise(); // glew will be initialized 
    FreeImage_SetOutputMessage(&FreeImageOutputMessageFunc);
    glfwInit();
  }

  void Application::FinalizeStandardLibraries()
  {
    glfwTerminate();
    FreeImage_DeInitialise();
  }

  bool Application::DoInitialize(int argc, char ** argv, char ** env)
  {
    return true;
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
    p.normalize();
    p.make_preferred();

    application_path     = p.parent_path();
    application_filename = p.filename(); 
    resources_path       = application_path / "resources";
    userlocal_path       = WinTools::GetUserLocalPath() / application_filename;
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
