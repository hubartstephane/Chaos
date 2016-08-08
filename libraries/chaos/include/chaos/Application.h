#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FilePathManager.h>

namespace chaos
{
/**
 * Application : used to store generic application data
 */
class Application
{
public:

  /** constructor */
  Application() : configuration_state(nullptr), file_path_manager(nullptr){}
  /** destructor */
  virtual ~Application();

  /** a generic function for initializing */
  template<typename T>
  static bool Initialize(int argc, char ** argv, char ** env)
  {
    assert(singleton_instance == nullptr);
    singleton_instance = new T;
    return singleton_instance->DoInitialize(argc, argv, env);
  }

  /** a generic function for initializing with default Application as class */
  static bool Initialize(int argc, char ** argv, char ** env)
  {
    return Initialize<Application>(argc, argv, env);
  }

  /** getter of the singleton instance */
  static inline Application * GetInstance(){ return singleton_instance; }
  /** getter of the singleton instance */
  static inline Application const * GetConstInstance(){ return singleton_instance; }

  /** get an environment value */
  char const * GetEnvironment(char const * key) const;

  /** get the application arguments */
  inline std::vector<std::string> const & GetArguments() const { return arguments; }
  /** get the application path */
  inline boost::filesystem::path const & GetApplicationPath() const { return application_path; }
  /** get the application name */
  inline boost::filesystem::path const & GetApplicationFilename() const { return application_filename; }
  /** get the application local path for execution data */
  inline boost::filesystem::path const & GetApplicationUserLocalPath() const { return application_userlocal_path; }

protected:

  /** Initialize the application with the main data */
  virtual bool DoInitialize(int argc, char ** argv, char ** env);
  /** load the configuration file */
  virtual bool LoadConfigurationFile();
  /** store the application parameters */
  virtual void StoreParameters(int argc, char ** argv, char ** env);
  /** prepare lua state for configuration file */
  virtual void InitializeLuaState(lua_State * state);

  /** initialize some members */
  virtual FilePathManager * CreateFilePathManager();


protected:

    /** the single application instance */
  static Application * singleton_instance;

  /** lua state from configuration file */
  lua_State * configuration_state;
  /** the application parameters */
  std::vector<std::string> arguments;
  /** the application environments */
  std::vector<std::pair<std::string, std::string>> environments;
  /** the filename of the application */
  boost::filesystem::path application_filename;
  /** the path of the application */
  boost::filesystem::path application_path;
  /** path of the application to store user data */
  boost::filesystem::path application_userlocal_path;

  /** the path manager */
  FilePathManager * file_path_manager;
};

}; // namespace chaos
