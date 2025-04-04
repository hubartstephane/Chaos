#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

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

	void Application::LogExecutionInformation()
	{
		// display the arguments
		ApplicationLog::BeginTransaction(LogSeverity::Message);
		ApplicationLog::TransactionConcatLN("==========================================");
		ApplicationLog::TransactionConcatLN("== Command line");
		ApplicationLog::TransactionConcatLN("==========================================");
		for (std::string const& arg : GetArguments())
			ApplicationLog::TransactionConcatLN("%s", arg.c_str());
		ApplicationLog::EndTransaction();

		// display some informations
		ApplicationLog::BeginTransaction(LogSeverity::Message);
		ApplicationLog::TransactionConcatLN("==========================================");
		ApplicationLog::TransactionConcatLN("== Informations");
		ApplicationLog::TransactionConcatLN("==========================================");
		ApplicationLog::TransactionConcatLN("Working directory: %s", boost::filesystem::current_path().string().c_str());
		ApplicationLog::TransactionConcatLN("%s", StringTools::TimeToString(std::chrono::system_clock::now(), TimeToStringFormatType::FULL).c_str());

		ApplicationLog::EndTransaction();
	}

	boost::filesystem::path Application::GetConfigurationPath() const
	{
		return GetResourcesPath() / "config.json";
	}

	boost::filesystem::path Application::GetPersistentDataPath() const
	{
		return GetUserLocalTempPath() / "persistent_data.json";
	}

	bool Application::LoadClasses(JSONReadConfiguration config)
	{
		if (JSONReadConfiguration classes_config = JSONTools::GetAttributeObjectNode(config, "classes"))
		{
			std::string classes_directory;
			if (JSONTools::GetAttribute(classes_config, "classes_directory", classes_directory))
			{
				ClassLoader loader;
				loader.LoadClassesInDirectory(ClassManager::GetDefaultInstance(), classes_directory);
			}
		}
		return true;
	}

	bool Application::Initialize(JSONReadConfiguration config)
	{
		return OnInitialize(config);
	}

	bool Application::OnInitialize(JSONReadConfiguration config)
	{
		// load class
		if (!LoadClasses(config))
		{
			ApplicationLog::Error("LoadClasses(...) failure");
			return false;
		}

		// initialize the managers
		if (!InitializeManagers(config))
		{
			ApplicationLog::Error("InitializeManagers(...) failure");
			return false;
		}
		return true;
	}

	void Application::Finalize()
	{
		FinalizeManagers();
	}

	int Application::Main()
	{
		return 0;
	}

	bool Application::InitializeConfigurationSystem()
	{
		// create the configuration system
		RootObjectConfiguration* root_config = new RootObjectConfiguration;
		if (root_config == nullptr)
		{
			ApplicationLog::Error("InitializeConfigurationSystem(...) failure");
			return false;
		}
		SetObjectConfiguration(root_config);
		root_config->LoadConfigurablePropertiesFromFile(GetConfigurationPath(), GetPersistentDataPath(), false); // don't send notification yet

		// load the properties
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::Initialization, false)) // do not recursively update all child objects
		{
			ApplicationLog::Error("InitializeConfigurationSystem(...) failure");
			return false;
		}
		return true;
	}

	bool Application::InitializeLogging()
	{
		// prepare the logger
		if (Logger* logger = Logger::GetInstance())
			if (LoggerListener* listener = new FileLoggerListener())
				listener->SetLogger(logger);
		// some logs
		LogExecutionInformation();
		HelpText::OutputToLogs();

		return true;
	}

	bool Application::InitializeGlobalVariables(int argc, char** argv)
	{
		GlobalVariableManager::GetInstance()->ParseArguments(argc, argv);

		// display the options
#if _DEBUG
		ApplicationLog::BeginTransaction(LogSeverity::Message);
		ApplicationLog::TransactionConcatLN("==========================================");
		ApplicationLog::TransactionConcatLN("== Existing Options");
		ApplicationLog::TransactionConcatLN("==========================================");
		ApplicationLog::TransactionConcatLN(GlobalVariableManager::GetInstance()->GetOptionString());
		ApplicationLog::EndTransaction();
#endif

		return true;
	}

	int Application::Run(int argc, char ** argv, char ** env)
	{
		int result = -1;
		if (InitializeStandardLibraries())
		{
			// store a copy of the parameters
			StoreParameters(argc, argv, env);
			// create a user temp directory if necessary */
			CreateUserLocalTempDirectory();
			// initialize logging system (need to be done after StoreParameters so that application paths are known for loggers)
			InitializeLogging();
			// initialize global variables using parameters (requires logger)
			InitializeGlobalVariables(argc, argv);

			// initialize, run, and finalize the application
			if (InitializeConfigurationSystem())
			{
				if (Initialize(GetJSONReadConfiguration()))
				{
					result = Main();
					SavePersistentPropertiesToFile(true); // save the persistent data to file
				}
				// finalization (even if initialization failed)
				Finalize();
			}

			FinalizeStandardLibraries();
		}
		return result;
	}

	bool Application::InitializeStandardLibraries()
	{
		return true;
	}

	void Application::FinalizeStandardLibraries()
	{
	}

	bool Application::InitializeManagers(JSONReadConfiguration config)
	{
		return true;
	}

	void Application::FinalizeManagers()
	{
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
		p = p.lexically_normal();
		p.make_preferred();

		application_filename = p.filename();
		application_name = application_filename.replace_extension().string();

		application_path = p.parent_path();
		resources_path = application_path / "resources";
		userlocal_path = WinTools::GetUserLocalPath() / "chaos" / application_name;
		userlocal_temp_path = userlocal_path / "temp";
	}

	char const* Application::GetApplicationName()
	{
		Application const* application = Application::GetInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetName();
	}

	char const * Application::GetApplicationEnvironment(char const * key)
	{
		Application const * application = Application::GetInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetEnvironment(key);
	}

	char const * Application::GetEnvironment(char const * key) const
	{
		assert(key != nullptr);
		for (auto e : environments)
			if (StringTools::Stricmp(e.first, key) == 0)
				return e.second.c_str();
		return nullptr;
	}

	boost::filesystem::path const & Application::CreateUserLocalTempDirectory() const
	{
		boost::filesystem::path const & result = GetUserLocalTempPath();
		if (!boost::filesystem::is_directory(result))
			boost::filesystem::create_directories(result);
		return result;
	}

	bool Application::HasApplicationCommandLineFlag(char const * flag_name)
	{
		Application const * application = Application::GetInstance();
		if (application == nullptr)
			return false;
		return application->HasCommandLineFlag(flag_name);
	}

	bool Application::HasCommandLineFlag(char const * flag_name) const
	{
		assert(flag_name != nullptr);
		for (std::string const & arg : arguments)
			if (StringTools::Stricmp(arg, flag_name) == 0)
				return true;
		return false;
	}

#if _DEBUG
	void Application::SetFileRedirectionDirectories(boost::filesystem::path const & build_path, std::string const & direct_access_paths)
	{
		redirection_build_path = build_path.lexically_normal();

		std::vector<std::string> extra = StringTools::Split(direct_access_paths.c_str(), ';');
		for (std::string & e : extra)
		{
			boost::filesystem::path p = std::move(e);
			if (!p.empty())
				redirection_source_paths.push_back(p.lexically_normal());
		}
	}
#endif // _DEBUG

}; // namespace chaos
