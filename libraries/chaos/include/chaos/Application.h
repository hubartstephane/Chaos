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

  /** destructor */
  virtual ~Application() = default;

  /** a generic function for initializing */
  template<typename T>
  static bool Initialize(int argc, char ** argv, char ** env)
  {
    assert(singleton_instance == nullptr);
    InitializeStandardLibraries();
    singleton_instance = new T;
    singleton_instance->StoreParameters(argc, argv, env);
    return singleton_instance->DoInitialize(argc, argv, env);
  }

  /** a generic function for initializing with default Application as class */
  static bool Initialize(int argc, char ** argv, char ** env)
  {
    return Initialize<Application>(argc, argv, env);
  }

  /** finalization method */
  static void Finalize();

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
  /** get the resources path */
  inline boost::filesystem::path const & GetResourcesPath() const { return resources_path; }
  /** get the application name */
  inline boost::filesystem::path const & GetApplicationFilename() const { return application_filename; }
  /** get the application local path for execution data */
  inline boost::filesystem::path const & GetUserLocalPath() const { return userlocal_path; }

protected:

  /** Initialize the application with the main data */
  virtual bool DoInitialize(int argc, char ** argv, char ** env);
  /** store the application parameters */
  virtual void StoreParameters(int argc, char ** argv, char ** env);
  /** Finalization method */
  virtual void DoFinalize();

  /** standard library initialization */
  static void InitializeStandardLibraries();
  /** standard library finalization */
  static void FinalizeStandardLibraries();

  /** a debugging function to output some message from FreeImage */
  static void FreeImageOutputMessageFunc(FREE_IMAGE_FORMAT fif, const char *msg);

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
};

}; // namespace chaos
