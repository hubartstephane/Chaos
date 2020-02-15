#include <chaos/Application.h>
#include <chaos/LuaTools.h>
#include <chaos/FileTools.h>
#include <chaos/Buffer.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
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

	bool Application::ReloadConfigurationFile(nlohmann::json & result) const
	{
		return JSONTools::LoadJSONFile(configuration_path, result, true);	
	}

	bool Application::LoadConfigurationFile()
	{		
		FilePathParam path = GetResourcesPath() / "config.json";

		if (JSONTools::LoadJSONFile(path, configuration, true))
		{
			configuration_path = path.GetResolvedPath();		
			return true;
		}
		return false;
	}

	bool Application::Initialize()
	{
		WinTools::AllocConsoleAndRedirectStdOutput();

		if (!InitializeManagers())
			return false;
		// open user temp directory and dump the config file
		boost::filesystem::path user_temp = CreateUserLocalTempDirectory(); // XXX : this directory is necessary for some per application data
#if _DEBUG
		// display the directories to help debugging
		bool dump_config = HasCommandLineFlag("-DumpConfigFile"); // CMDLINE
		if (dump_config)
			JSONTools::DumpConfigFile(configuration);
		if (dump_config || HasCommandLineFlag("-ShowDirectories") || HasCommandLineFlag("-ShowUserTempDirectory")) // CMDLINE
			WinTools::ShowFile(user_temp);
		if (HasCommandLineFlag("-ShowDirectories") || HasCommandLineFlag("-ShowInstalledResourcesDirectory")) // CMDLINE
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

		application_filename = p.filename();
		application_name = application_filename.replace_extension().string();

		application_path = p.parent_path();
		resources_path = application_path / "resources";
		userlocal_path = WinTools::GetUserLocalPath() / application_name;
		userlocal_temp_path = userlocal_path / "temp";
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
		return InputMode::Keyboard;
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
	void Application::SetFileRedirectionDirectories(boost::filesystem::path const & build_path, boost::filesystem::path const & src_path)
	{
		redirection_build_path = build_path;
		redirection_source_path = src_path;		
	}
#endif // _DEBUG

}; // namespace chaos
