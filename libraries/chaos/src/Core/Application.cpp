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
		// display the options
#if _DEBUG
		Log::BeginTransaction(LogType::Message);
		Log::TransactionConcatLN("==========================================");
		Log::TransactionConcatLN("== Existing Options");
		Log::TransactionConcatLN("==========================================");
		Log::TransactionConcatLN(GlobalVariableManager::GetInstance()->GetOptionString());
		Log::EndTransaction();
#endif
		// display the arguments
		Log::BeginTransaction(LogType::Message);
		Log::TransactionConcatLN("==========================================");
		Log::TransactionConcatLN("== Command line");
		Log::TransactionConcatLN("==========================================");
		for (std::string const& arg : GetArguments())
			Log::TransactionConcatLN("%s", arg.c_str());
		Log::EndTransaction();

		// display some informations
		Log::BeginTransaction(LogType::Message);
		Log::TransactionConcatLN("==========================================");
		Log::TransactionConcatLN("== Informations");
		Log::TransactionConcatLN("==========================================");
		Log::TransactionConcatLN("Working directory: %s", boost::filesystem::current_path().string().c_str());
		Log::TransactionConcatLN("%s", StringTools::TimeToString(std::chrono::system_clock::now(), TimeToStringFormatType::FULL).c_str());

		Log::EndTransaction();
	}

	bool Application::OnConfigurationChanged(JSONReadConfiguration config)
	{
		return ConfigurableInterface::OnConfigurationChanged(config);
	}

	bool Application::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		return true;
	}

	boost::filesystem::path Application::GetConfigurationPath() const
	{
		return GetResourcesPath() / "config.json";
	}

	boost::filesystem::path Application::GetPersistentDataPath() const
	{
		return GetUserLocalTempPath() / "persistent_data.json";
	}

	bool Application::LoadClasses()
	{
		if (JSONReadConfiguration classes_config = JSONTools::GetAttributeObjectNode(GetJSONReadConfiguration(), "classes"))
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

	bool Application::Initialize()
	{
		// prepare the logger
		if (Logger* logger = Logger::GetInstance())
			if (LoggerListener* listener = new FileLoggerListener())
				listener->SetLogger(logger);

		HelpText::OutputToLogs();

		// show console
		if (false)
			WinTools::AllocConsoleAndRedirectStdOutput();

		// some log
		LogExecutionInformation();

		// load the configuration
		if (!InitializeConfiguration())
		{
			Log::Error("InitializeConfiguration(...) failure");
			return false;
		}

		// load the properties
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::INITIALIZATION, false))
		{
			Log::Error("ReadConfigurableProperties(...) failure");
			return false;
		}

		// load class
		if (!LoadClasses())
		{
			Log::Error("LoadClasses(...) failure");
			return false;
		}

		// initialize the managers
		if (!InitializeManagers())
		{
			Log::Error("InitializeManagers(...) failure");
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

	bool Application::InitializeConfiguration()
	{
		if (RootObjectConfiguration* root_config = new RootObjectConfiguration)
		{
			SetObjectConfiguration(root_config);
			root_config->LoadConfigurablePropertiesFromFile(GetConfigurationPath(), GetPersistentDataPath(), false); // do not send notification yet
			return true;
		}
		return false;

	}

	int Application::Run(int argc, char ** argv, char ** env)
	{
		bool result = false;
		if (InitializeStandardLibraries())
		{
			// parse the parameters
			GlobalVariableManager::GetInstance()->ParseArguments(argc, argv);

			// store a copy of the parameters
			StoreParameters(argc, argv, env);
			// create a user temp directory if necessary */
			CreateUserLocalTempDirectory();
			// initialize, run, and finalize the application
			if (Initialize())
			{
				result = Main();
				SavePersistentPropertiesToFile(true); // save the persistent data to file
			}
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

	void Application::FinalizeStandardLibraries()
	{
	}

	bool Application::InitializeManagers()
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
		userlocal_path = WinTools::GetUserLocalPath() / application_name;
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

	InputMode Application::GetApplicationInputMode()
	{
		Application const * application = Application::GetConstInstance();
		if (application != nullptr)
			return application->GetInputMode();
		return InputMode::KEYBOARD;
	}

	void Application::SetApplicationInputMode(InputMode new_mode)
	{
		Application * application = Application::GetInstance();
		if (application != nullptr)
			application->SetInputMode(new_mode);
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
