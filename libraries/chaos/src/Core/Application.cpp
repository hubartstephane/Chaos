#include <chaos/Chaos.h>

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
		Log::Output(LogType::Message, false, "==========================================\n");
		Log::Output(LogType::Message, false, "== Options\n");
		Log::Output(LogType::Message, false, "==========================================\n\n");
		Log::Output(LogType::Message, false, ApplicationArgumentManager::GetInstance()->GetOptionString());
#endif
		// display the arguments
		Log::Output(LogType::Message, false, "==========================================\n");
		Log::Output(LogType::Message, false, "== Command line\n");
		Log::Output(LogType::Message, false, "==========================================\n\n");
		for (std::string const& arg : GetArguments())
			Log::Output(LogType::Message, false, "%s ", arg.c_str());
		Log::Output(LogType::Message, false, "\n");
		Log::Output(LogType::Message, false, "==========================================\n");
		Log::Output(LogType::Message, false, "== Informations\n");
		Log::Output(LogType::Message, false, "==========================================\n\n");
		// the current directory
		Log::Message("Working directory: %s", boost::filesystem::current_path().string().c_str());
		// the date
		std::time_t t = std::time(0);
		struct tm tm;
		localtime_s(&tm, &t);

		Log::Message(
			"Time: %04d-%02d-%02d %02dh%02dm%02ds",
			tm.tm_year + 1900,
			tm.tm_mon + 1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec);
	}

	bool Application::ReloadConfigurationFile(nlohmann::json & result) const
	{
		return JSONTools::LoadJSONFile(configuration_path, result, LoadFileFlag::RECURSIVE);
	}

	bool Application::LoadConfigurationFile()
	{
		boost::filesystem::path path = GetResourcesPath() / "config.json";

		if (JSONTools::LoadJSONFile(path, configuration, LoadFileFlag::RECURSIVE))
		{
			configuration_path = std::move(path);
			return true;
		}
		return false;
	}

	bool Application::LoadClasses()
	{
		nlohmann::json const * classes_json = JSONTools::GetStructure(configuration, "classes");
		if (classes_json != nullptr && classes_json->is_object())
		{
			std::string classes_directory;
			if (JSONTools::GetAttribute(*classes_json, "classes_directory", classes_directory))
			{
				ClassLoader loader;
				loader.LoadClassesInDirectory(classes_directory);
			}
		}
		return true;
	}

	CHAOS_APPLICATION_ARG(bool, ShowConsole);
	CHAOS_APPLICATION_ARG(bool, HideConsole);
	CHAOS_APPLICATION_ARG(bool, DumpConfigFile);
	CHAOS_APPLICATION_ARG(bool, ShowDirectories);
	CHAOS_APPLICATION_ARG(bool, ShowUserTempDirectory);
	CHAOS_APPLICATION_ARG(bool, ShowInstalledResourcesDirectory);

	bool Application::Initialize()
	{
		// show console
		bool will_show_console = show_console;
		if (Arguments::ShowConsole)
			will_show_console = true;
		else if (Arguments::HideConsole)
			will_show_console = false;

		if (will_show_console)
			WinTools::AllocConsoleAndRedirectStdOutput();

		// some log
		LogExecutionInformation();

		// load class
		if (!LoadClasses())
			return false;

		// initialize the managers
		if (!InitializeManagers())
			return false;
		// open user temp directory and dump the config file
		boost::filesystem::path user_temp = CreateUserLocalTempDirectory(); // XXX : this directory is necessary for some per application data
#if _DEBUG
		// display the directories to help debugging
		bool dump_config = Arguments::DumpConfigFile;
		if (dump_config)
			JSONTools::DumpConfigFile(configuration);
		if (dump_config || Arguments::ShowDirectories || Arguments::ShowUserTempDirectory)
			WinTools::ShowFile(user_temp);
		if (Arguments::ShowDirectories || Arguments::ShowInstalledResourcesDirectory)
			WinTools::ShowFile(GetResourcesPath());
#endif
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
			// parse the parameters
			ApplicationArgumentManager::GetInstance()->ParseArguments(argc, argv);

			// store a copy of the parameters
			StoreParameters(argc, argv, env);
			// load the configuration file (ignore return value because there is no obligation to use a configuration file)
			LoadConfigurationFile();
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

		return true;
	}

	bool Application::FinalizeManagers()
	{
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

	boost::filesystem::path const & Application::ShowUserLocalTempDirectory() const
	{
		boost::filesystem::path const & result = CreateUserLocalTempDirectory();
		WinTools::ShowFile(result);
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
