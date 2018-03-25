#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FileManager.h>

namespace chaos
{
/**
 * Application : used to store generic application data
 */
class Application
{
public:

  /** constructor */
  Application();
  /** destructor */
  virtual ~Application();

  /** the user main method */
  bool Run(int argc, char ** argv, char ** env);

  /** getter of the singleton instance */
  static inline Application * GetInstance() { return singleton_instance; }
  /** getter of the singleton instance */
  static inline Application const * GetConstInstance() { return singleton_instance; }

  /** get an environment value */
  char const * GetEnvironment(char const * key) const;

  /** get the application arguments */
  inline std::vector<std::string> const & GetArguments() const { return arguments; }
  /** get the application path */
  inline boost::filesystem::path const & GetApplicationPath() const { return application_path; }
  /** get the resources path */
  inline boost::filesystem::path const & GetResourcesPath() const { return resources_path; }
  /** get the application name */
  inline boost::filesystem::path const & GetApplicationFilename() const { return application_filename; }
  /** get the application local path for execution data */
  inline boost::filesystem::path const & GetUserLocalPath() const { return userlocal_path; }

  /** gets the file manager */
  FileManager * GetFileManager() { return file_manager.get(); }
  /** gets the file manager */
  FileManager const * GetFileManager() const { return file_manager.get(); }

  /** get the configuration */
  nlohmann::json const & GetConfiguration() const { return configuration;}

protected:

  /** the method that should be override */
  virtual bool Main();
  /** store the application parameters */
  virtual void StoreParameters(int argc, char ** argv, char ** env);
  /** Initialize the application with the main data */
  virtual bool Initialize();
  /** Finalization method */
  virtual bool Finalize();
  /** standard library initialization */
  virtual bool InitializeStandardLibraries();
  /** standard library finalization */
  virtual bool FinalizeStandardLibraries();

  /** initialize the managers */
  virtual bool InitializeManagers();
  /** finalize the managers */
  virtual bool FinalizeManagers();

  /** loading the configuration file */
  void LoadConfigurationFile();

protected:

    /** the single application instance */
  static Application * singleton_instance;

  /** the application parameters */
  std::vector<std::string> arguments;
  /** the application environments */
  std::vector<std::pair<std::string, std::string>> environments;
  /** the filename of the application */
  boost::filesystem::path application_filename;
  /** the path of the application */
  boost::filesystem::path application_path;
  /** the path of the resources */
  boost::filesystem::path resources_path;
  /** path of the application to store user data */
  boost::filesystem::path userlocal_path;

  /** the JSON configuration file if existing */
  nlohmann::json configuration;
  /** the path where the configuration file is */
  boost::filesystem::path configuration_path;

  /** the file manager */
  boost::intrusive_ptr<FileManager> file_manager;
};

template<typename APPLICATION_TYPE>
bool RunApplication(int argc, char ** argv, char ** env)
{
  bool result = false;

  APPLICATION_TYPE * application = new APPLICATION_TYPE();
  if (application != nullptr)
  {
    result = application->Run(argc, argv, env);
    delete(application);
  }
  return result;
}

}; // namespace chaos
