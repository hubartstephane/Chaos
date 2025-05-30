namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Application;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(ApplicationLog, "Application")

	/**
	* Application : used to store generic application data
	*/
	class CHAOS_API Application : public Object, public ConfigurationUserInterface
	{

	public:

		/** constructor */
		Application();
		/** destructor */
		virtual ~Application();

		/** the user main method */
		int Run(int argc, char** argv, char** env);

		/** getter of the singleton instance */
		static AutoCastable<Application> GetInstance() { return singleton_instance; }
		/** getter of the singleton instance */
		static AutoConstCastable<Application> GetConstInstance() { return singleton_instance; }

		/** get the name of the application */
		char const* GetName() const { return application_name.c_str(); }
		/** get the name of the application */
		static char const* GetApplicationName();

		/** get an environment value */
		char const* GetEnvironment(char const* key) const;
		/** get an environment value */
		static char const* GetApplicationEnvironment(char const* key);

		/** get the application arguments */
		std::vector<std::string> const& GetArguments() const { return arguments; }
		/** get the application path */
		boost::filesystem::path const& GetApplicationPath() const { return application_path; }
		/** get the resources path */
		boost::filesystem::path const& GetResourcesPath() const { return resources_path; }
		/** get the application name */
		boost::filesystem::path const& GetApplicationFilename() const { return application_filename; }
		/** get the application local path for execution data */
		boost::filesystem::path const& GetUserLocalPath() const { return userlocal_path; }
		/** get the application local temp path for execution data */
		boost::filesystem::path const& GetUserLocalTempPath() const { return userlocal_temp_path; }

		/** get the configuration file path */
		boost::filesystem::path GetConfigurationPath() const;

		/** get the persistent data file path */
		boost::filesystem::path GetPersistentDataPath() const;

		/** create the use local temp directory */
		boost::filesystem::path const& CreateUserLocalTempDirectory() const;

		/** check whether -flag_name is in command line */
		bool HasCommandLineFlag(char const* flag_name) const;
		/** check whether -flag_name is in command line */
		static bool HasApplicationCommandLineFlag(char const* flag_name);

#if _DEBUG
		/** set redirection file directories */
		void SetFileRedirectionDirectories( boost::filesystem::path const & build_path, std::string const & direct_access_paths);
		/** get the redirected source directories */
		std::vector<boost::filesystem::path> const& GetRedirectionSourcePaths() const { return redirection_source_paths; };
		/** get the redirected build directory */
		boost::filesystem::path const& GetRedirectionBuildPath() const { return redirection_build_path; };
#endif

	protected:

		/** the method that should be override */
		virtual int Main();
		/** store the application parameters */
		virtual void StoreParameters(int argc, char** argv, char** env);
		/** Initialize the application with the main data */
		virtual bool Initialize(JSONReadConfiguration config);
		/** Initialization custom point */
		virtual bool OnInitialize(JSONReadConfiguration config);
		/** Finalization method */
		virtual void Finalize();
		/** standard library initialization */
		virtual bool InitializeStandardLibraries();
		/** standard library finalization */
		virtual void FinalizeStandardLibraries();

		/** initialize the managers */
		virtual bool InitializeManagers(JSONReadConfiguration config);
		/** finalize the managers */
		virtual void FinalizeManagers();

		/** initialization the configuration system */
		bool InitializeConfigurationSystem();
		/** initialize the logging system */
		bool InitializeLogging();
		/** initialize the global variable system */
		bool InitializeGlobalVariables(int argc, char** argv);

		/** load the extra classes */
		virtual bool LoadClasses(JSONReadConfiguration config);
		/** log some application information */
		virtual void LogExecutionInformation();

	protected:

		/** the single application instance */
		static Application* singleton_instance;

		/** the name of the application */
		std::string application_name;
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
		/** path of the application to store user temp data */
		boost::filesystem::path userlocal_temp_path;

		/** redirection source directories */
#if _DEBUG
		boost::filesystem::path redirection_build_path;
		std::vector<boost::filesystem::path> redirection_source_paths;
#endif
	};

	template<typename APPLICATION_TYPE, typename ...PARAMS>
	int RunApplication(int argc, char** argv, char** env, PARAMS && ...params)
	{
		static_assert(std::is_base_of_v<Application, APPLICATION_TYPE>);

		shared_ptr<APPLICATION_TYPE> application = new APPLICATION_TYPE(std::forward<PARAMS>(params)...);
		if (application != nullptr)
		{

#if (_DEBUG && defined DEATH_PROJECT_BUILD_PATH && defined DEATH_PROJECT_DIRECT_RESOURCE_PATH) // File Redirection

			// XXX: under normal circonstances, you should not use DEATH_PROJECT_BUILD_PATH, DEATH_PROJECT_DIRECT_RESOURCE_PATH in libraries
			//      here, this is an exception because this function is a template and so is not compiled in libraries but by caller code instead
			//
			// XXX: premake defines {...} function produces errors whenever the string contains some special characters like ';'
			//      that's why they are encoded in Base64

			// build directory
			Buffer<char> decoded_build_path = MyBase64().Decode(DEATH_PROJECT_BUILD_PATH);
			boost::filesystem::path build_path = std::string(decoded_build_path.data, decoded_build_path.bufsize);
			// source directories
			Buffer<char> decoded_direct_access_paths = MyBase64().Decode(DEATH_PROJECT_DIRECT_RESOURCE_PATH);
			std::string direct_access_paths = std::string(decoded_direct_access_paths.data, decoded_direct_access_paths.bufsize);
			// prepare the application for direct access
			application->SetFileRedirectionDirectories(build_path, direct_access_paths);

#endif
			return application->Run(argc, argv, env);
		}
		return -1; // error
	}

#endif

}; // namespace chaos