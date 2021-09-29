#include <chaos/Chaos.h>

namespace chaos
{
	//
	// GLFWHints
	//

	void GLFWHints::ApplyHints()
	{
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug_context);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
		glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate); // only usefull in fullscreen mode
		glfwWindowHint(GLFW_OPENGL_PROFILE, opengl_profile);
	}

	bool SaveIntoJSON(nlohmann::json& json, GLFWHints const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "debug_context", src.debug_context);
		JSONTools::SetAttribute(json, "major_version", src.major_version);
		JSONTools::SetAttribute(json, "minor_version", src.minor_version);
		JSONTools::SetAttribute(json, "refresh_rate", src.refresh_rate);
		JSONTools::SetAttribute(json, "opengl_profile", src.opengl_profile);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json, GLFWHints& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "debug_context", dst.debug_context);
		JSONTools::GetAttribute(json, "major_version", dst.major_version);
		JSONTools::GetAttribute(json, "minor_version", dst.minor_version);
		JSONTools::GetAttribute(json, "refresh_rate", dst.refresh_rate);
		JSONTools::GetAttribute(json, "opengl_profile", dst.opengl_profile);
		return true;
	}

	// 
	// WindowApplication
	//

	WindowApplication::WindowApplication(SubClassOf<Window> in_main_window_class, WindowParams const& in_window_params, WindowHints const & in_window_hints) :
		main_window_class(in_main_window_class),
		window_params(in_window_params),
		window_hints(in_window_hints)
	{
		assert(in_main_window_class.IsValid());
	}

	void WindowApplication::FreezeNextFrameTickDuration()
	{
		forced_zero_tick_duration = true;
	}

	bool WindowApplication::MessageLoop()
	{
		double t1 = glfwGetTime();

		while(windows.size() > 0)
		{
			glfwPollEvents();

			double t2 = glfwGetTime();
			float delta_time = (float)(t2 - t1);
			float real_delta_time = delta_time;

			if (forced_zero_tick_duration)
			{
				delta_time = 0.0f;
				forced_zero_tick_duration = false;
			}
			else
			{
				if (forced_tick_duration > 0.0f)
					delta_time = forced_tick_duration;
				else if (max_tick_duration > 0.0f)
					delta_time = std::min(delta_time, max_tick_duration);
			}
			// internal tick 
			WithGLContext<void>(shared_context, [this, delta_time]()
			{
				Tick(delta_time);
			});

			// destroy windows that mean to be
			for (size_t i = windows.size(); i > 0; --i)
			{
				Window* window = windows[i - 1].get();
				if (window != nullptr && window->ShouldClose())
				{
					// destroy the internal GLFW window
					WithGLContext<void>(window->GetGLFWHandler(), [window]() 
					{
						window->Finalize();
						window->DestroyGLFWWindow();
					});	
					// remove the window for the list
					windows[i - 1] = windows[windows.size() - 1];
					windows.pop_back();
					// check whether it was the main window
					if (window == main_window)
						main_window = nullptr;
				}
			}
			// tick the windows
			for (auto& window : windows)
			{
				WithGLContext<void>(window->GetGLFWHandler(), [window, delta_time, real_delta_time]()
				{
					window->MainTick(delta_time, real_delta_time);
				});
			}
			// update time
			t1 = t2;
		}
		return true;
	}

	Window* WindowApplication::CreateTypedWindow(SubClassOf<Window> window_class, WindowParams const& params, WindowHints const& hints)
	{
		return WithGLContext<Window*>(nullptr, [this, window_class, params, hints]() -> Window *
		{
			// create the window class
			Window* result = window_class.CreateInstance();
			if (result == nullptr)
			{
				return nullptr;
			}
			// create the GLFW resource
			if (!result->CreateGLFWWindow(params, hints, shared_context))
			{
				delete(result);
				return nullptr;
			}
			// post initialization method
			OnWindowCreated(result);
			// store the result
			windows.push_back(result);
			return result;
		});
	}

	void WindowApplication::OnWindowCreated(Window* window)
	{

	}

	bool WindowApplication::Main()
	{
		// the glfw configuration
		GLFWHints glfw_hints;
		nlohmann::json const* glfw_configuration = JSONTools::GetStructure(configuration, "glfw");
		if (glfw_configuration != nullptr)
			LoadFromJSON(*glfw_configuration, glfw_hints);
		glfw_hints.ApplyHints();

		// create a hidden window whose purpose is to provide a sharable context for all others		
		window_hints.ApplyHints();
		glfwWindowHint(GLFW_VISIBLE, 0);
		shared_context = glfwCreateWindow(100, 100, "", nullptr, nullptr);
		if (shared_context == nullptr)
			return false;

		if (!WithGLContext<bool>(shared_context, [this]()
		{
			// XXX : seems to be mandatory for some functions like : glGenVertexArrays(...)
				//       see https://www.opengl.org/wiki/OpenGL_Loading_Library
			glewExperimental = GL_TRUE;
			// create the context
			GLenum err = glewInit();
			if (err != GLEW_OK)
			{
				Log::Message("glewInit(...) failure : %s", glewGetErrorString(err));
				return false;
			}
			// set the debug log hander
			GLTools::SetDebugMessageHandler();
			// some generic information
			GLTools::DisplayGenericInformation();

			// initialize the GPU resource Manager (first window/OpenGL context must have been created)
			if (!CreateGPUResourceManager())
				return false;
			return true;
		}))
		{
			return false;
		}

		// the main window params & hints (work on copy)
		WindowParams params = window_params;
		WindowHints  hints = window_hints;

		nlohmann::json const* window_configuration = JSONTools::GetStructure(configuration, "window");
		if (window_configuration != nullptr)
		{
			LoadFromJSON(*window_configuration, params);
			LoadFromJSON(*window_configuration, hints);
		}

		// create the main window
		main_window = CreateTypedWindow(main_window_class, params, hints);
		if (main_window == nullptr)
			return false;
		// initialize the main with any configuration data window (once GPUResourceManager is fully initialized)
		if (!WithGLContext<bool>(main_window->GetGLFWHandler(), [this]()
		{
			return main_window->InitializeFromConfiguration(configuration);
		}))
		{
			return false;
		}

		// a final initialization (after main window is constructed ... and OpenGL context)
		if (!WithGLContext<bool>(shared_context, [this]()
		{
			return PreMessageLoop();
		}))
		{
			return false;
		}		
		// the main loop
		MessageLoop();

		return true;
	}

	bool WindowApplication::InitializeGamepadButtonMap()
	{
		// the map [button ID] => [bitmap name + text generator alias]
#define CHAOS_ADD_BUTTONMAP(x, y) gamepad_button_map[GamepadButton::x] = {"xboxController" #y, #y}
		CHAOS_ADD_BUTTONMAP(A, ButtonA);
		CHAOS_ADD_BUTTONMAP(B, ButtonB);
		CHAOS_ADD_BUTTONMAP(X, ButtonX);
		CHAOS_ADD_BUTTONMAP(Y, ButtonY);
		CHAOS_ADD_BUTTONMAP(LEFT_BUMPER, LeftShoulder);
		CHAOS_ADD_BUTTONMAP(RIGHT_BUMPER, RightShoulder);
		CHAOS_ADD_BUTTONMAP(LEFT_TRIGGER, LeftTrigger);
		CHAOS_ADD_BUTTONMAP(RIGHT_TRIGGER, RightTrigger);
#undef CHAOS_ADD_BUTTONMAP

		return true;
	}

	bool WindowApplication::FillAtlasGeneratorInput(BitmapAtlas::AtlasInput& input)
	{
		if (!FillAtlasGeneratorInputSprites(input))
			return false;
		if (!FillAtlasGeneratorInputFonts(input))
			return false;
		return true;
	}

	bool WindowApplication::FillAtlasGeneratorInputSprites(BitmapAtlas::AtlasInput& input)
	{
		// get the directory where the sprites are
		std::string sprite_directory;
		if (!JSONTools::GetAttribute(configuration, "sprite_directory", sprite_directory))
			return true;
		// find or create folder
		BitmapAtlas::FolderInfoInput* folder_info = input.AddFolder("sprites", 0);
		if (folder_info == nullptr)
			return false;
		// Add sprites
		folder_info->AddBitmapFilesFromDirectory(sprite_directory, true);

		return true;
	}

	bool WindowApplication::FillAtlasGeneratorInputFonts(BitmapAtlas::AtlasInput& input)
	{
		nlohmann::json const* fonts_config = JSONTools::GetStructure(configuration, "fonts");
		if (fonts_config != nullptr)
		{
			// read the default font parameters
			BitmapAtlas::FontInfoInputParams font_params;

			nlohmann::json const* default_font_param_json = JSONTools::GetStructure(*fonts_config, "default_font_param");
			if (default_font_param_json != nullptr)
				LoadFromJSON(*default_font_param_json, font_params);

			// Add the fonts
			nlohmann::json const* fonts_json = JSONTools::GetStructure(*fonts_config, "fonts");
			if (fonts_json != nullptr && fonts_json->is_object())
			{
				for (nlohmann::json::const_iterator it = fonts_json->begin(); it != fonts_json->end(); ++it)
				{
					if (!it->is_string())
						continue;
					// read information
					std::string font_name = it.key();
					std::string font_path = it->get<std::string>();
					if (input.AddFont(font_path.c_str(), nullptr, true, font_name.c_str(), 0, font_params) == nullptr)
						return false;
				}
			}
		}

		return true;
	}

	CHAOS_HELP_TEXT(CMD, "-UseCachedAtlas");
#if !_DEBUG
	CHAOS_HELP_TEXT(CMD, "-DumpCachedAtlas");
#endif

	bool WindowApplication::CreateTextureAtlas()
	{
		char const* CachedAtlasFilename = "CachedAtlas";

		// Try to load already computed data 
		if (Application::HasApplicationCommandLineFlag("-UseCachedAtlas")) // CMDLINE
		{
			BitmapAtlas::TextureArrayAtlas* tmp_texture_atlas = new BitmapAtlas::TextureArrayAtlas;
			if (tmp_texture_atlas != nullptr)
			{
				if (tmp_texture_atlas->LoadAtlas(GetUserLocalTempPath() / CachedAtlasFilename))
				{
					texture_atlas = tmp_texture_atlas;
					return true;
				}
				delete(tmp_texture_atlas);
			}
		}

		// fill sub images for atlas generation
		BitmapAtlas::AtlasInput input;
		if (!FillAtlasGeneratorInput(input))
			return false;

		// atlas generation params
		int const DEFAULT_ATLAS_SIZE = 1024;
		int const DEFAULT_ATLAS_PADDING = 10;

		BitmapAtlas::AtlasGeneratorParams params = BitmapAtlas::AtlasGeneratorParams(DEFAULT_ATLAS_SIZE, DEFAULT_ATLAS_SIZE, DEFAULT_ATLAS_PADDING, PixelFormatMergeParams());

		nlohmann::json const* atlas_json = JSONTools::GetStructure(configuration, "atlas");
		if (atlas_json != nullptr)
			LoadFromJSON(*atlas_json, params);

		// atlas generation params : maybe a dump
		char const* dump_atlas_dirname = nullptr;
#if _DEBUG
		dump_atlas_dirname = CachedAtlasFilename;
#else
		if (Application::HasApplicationCommandLineFlag("-DumpCachedAtlas")) // CMDLINE
			dump_atlas_dirname = CachedAtlasFilename;
#endif

		// generate the atlas
		BitmapAtlas::TextureArrayAtlasGenerator generator;
		texture_atlas = generator.ComputeResult(input, params, dump_atlas_dirname);
		if (texture_atlas == nullptr)
			return false;

		return true;
	}

	bool WindowApplication::CreateTextGenerator()
	{
		// get the font sub objects
		nlohmann::json const* fonts_config = JSONTools::GetStructure(configuration, "fonts");

		// create the generator
		particle_text_generator = new ParticleTextGenerator::Generator(*texture_atlas);
		if (particle_text_generator == nullptr)
			return false;

		// bitmaps in generator
		BitmapAtlas::FolderInfo const* folder_info = texture_atlas->GetFolderInfo("sprites");
		if (folder_info != nullptr)
		{
			// for each bitmap, that correspond to a button, register a [NAME] in the generator	
			for (auto it = gamepad_button_map.begin(); it != gamepad_button_map.end(); ++it)
			{
				std::string const& bitmap_name = it->second.first;
				BitmapAtlas::BitmapInfo const* info = folder_info->GetBitmapInfo(bitmap_name.c_str());
				if (info == nullptr)
					continue;
				std::string const& generator_alias = it->second.second;
				particle_text_generator->AddBitmap(generator_alias.c_str(), info);
			}
			// embedded sprites
			if (fonts_config != nullptr)
			{
				nlohmann::json const* font_bitmaps_json = JSONTools::GetStructure(*fonts_config, "bitmaps");
				if (font_bitmaps_json != nullptr && font_bitmaps_json->is_object())
				{
					for (nlohmann::json::const_iterator it = font_bitmaps_json->begin(); it != font_bitmaps_json->end(); ++it)
					{
						if (!it->is_string())
							continue;
						std::string bitmap_name = it.key();
						std::string bitmap_path = it->get<std::string>();
						BitmapAtlas::BitmapInfo const* info = folder_info->GetBitmapInfo(bitmap_path.c_str());
						if (info == nullptr)
							continue;
						particle_text_generator->AddBitmap(bitmap_name.c_str(), info);
					}
				}
			}
		}

		// the colors
		if (fonts_config != nullptr)
		{
			nlohmann::json const* font_colors_json = JSONTools::GetStructure(*fonts_config, "colors");
			if (font_colors_json != nullptr && font_colors_json->is_object())
			{
				for (nlohmann::json::const_iterator it = font_colors_json->begin(); it != font_colors_json->end(); ++it)
				{
					glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // initialization for if input is smaller than 4
					if (!LoadFromJSON(*it, color))
						continue;
					std::string color_name = it.key();
					particle_text_generator->AddColor(color_name.c_str(), color);
				}
			}
		}
		return true;
	}

	bool WindowApplication::PreMessageLoop()
	{
		assert(glfwGetCurrentContext() == shared_context);

		if (!CreateTextureAtlas())
		{
			chaos::Log::Error("WindowApplication::CreateTextureAtlas(...) failure");
			return false;
		}
		if (!InitializeGamepadButtonMap())
		{
			chaos::Log::Error("WindowApplication::InitializeGamepadButtonMap(...) failure");
			return false;
		}
		if (!CreateTextGenerator())
		{
			chaos::Log::Error("WindowApplication::CreateTextGenerator(...) failure");
			return false;
		}
		return true;
	}

	void WindowApplication::Tick(float delta_time)
	{
		assert(glfwGetCurrentContext() == shared_context);

		// tick the managers
		if (main_clock != nullptr)
			main_clock->TickClock(delta_time);
		if (sound_manager != nullptr)
			sound_manager->Tick(delta_time);
		// update keyboard state
		UpdateKeyStates(delta_time);
	}

	void WindowApplication::OnGLFWError(int code, const char* msg)
	{
		Log::Message("Window(...) [%d] failure : %s", code, msg);
	}

	bool WindowApplication::CreateGPUResourceManager()
	{
		assert(glfwGetCurrentContext() == shared_context);

		// create and start the GPU manager
		gpu_resource_manager = new GPUResourceManager;
		if (gpu_resource_manager == nullptr)
			return false;
		gpu_resource_manager->StartManager();
		// create internal resource
		if (!gpu_resource_manager->InitializeInternalResources())
			return false;
		// get JSON section and load all requested resources
		nlohmann::json const* gpu_config = JSONTools::GetStructure(configuration, "gpu");
		if (gpu_config != nullptr)
			if (!gpu_resource_manager->InitializeFromConfiguration(*gpu_config))
				return false;
		return true;
	}

	bool WindowApplication::ReloadGPUResources()
	{
		return WithGLContext<bool>(shared_context, [this]() 
		{
			// this call may block for too much time
			FreezeNextFrameTickDuration();

			// reload the configuration file
			nlohmann::json config;
			if (!ReloadConfigurationFile(config))
				return false;
			// get the structure of interrest
			nlohmann::json const* gpu_config = JSONTools::GetStructure(config, "gpu");
			if (gpu_config == nullptr)
				return false;
			// create a temporary manager
			shared_ptr<GPUResourceManager> other_gpu_resource_manager = new GPUResourceManager; // destroyed at the end of the function
			if (other_gpu_resource_manager == nullptr)
				return false;
			if (!other_gpu_resource_manager->StartManager())
				return false;
			// reload all resources ... (even unchanged)
			if (other_gpu_resource_manager->InitializeFromConfiguration(*gpu_config))
				gpu_resource_manager->RefreshGPUResources(other_gpu_resource_manager.get());
			other_gpu_resource_manager->StopManager();
			return true;
		});
	}

	bool WindowApplication::FinalizeGPUResourceManager()
	{
		// stop the resource manager
		if (gpu_resource_manager != nullptr)
		{
			gpu_resource_manager->StopManager();
			gpu_resource_manager = nullptr;
		}
		return true;
	}

	bool WindowApplication::InitializeManagers()
	{
		if (!Application::InitializeManagers())
			return false;

		// update some internals
		JSONTools::GetAttribute(configuration, "max_tick_duration", max_tick_duration);
		JSONTools::GetAttribute(configuration, "forced_tick_duration", forced_tick_duration);

		// initialize the clock
		main_clock = new Clock("main_clock");
		if (main_clock == nullptr)
			return false;
		nlohmann::json const* clock_config = JSONTools::GetStructure(configuration, "clocks");
		if (clock_config != nullptr)
			main_clock->InitializeFromConfiguration(*clock_config);

		// initialize the sound manager
		sound_manager = new SoundManager();
		if (sound_manager == nullptr)
			return false;
		sound_manager->StartManager();
		nlohmann::json const* sound_config = JSONTools::GetStructure(configuration, "sounds");
		if (sound_config != nullptr)
			sound_manager->InitializeFromConfiguration(*sound_config);

		return true;
	}

	bool WindowApplication::FinalizeManagers()
	{
		// stop the clock
		main_clock = nullptr;
		// stop the sound manager
		if (sound_manager != nullptr)
		{
			sound_manager->StopManager();
			sound_manager = nullptr;
		}
		// stop the resource manager
		if (gpu_resource_manager != nullptr)
		{
			gpu_resource_manager->StopManager();
			gpu_resource_manager = nullptr;
		}
		// super method
		Application::FinalizeManagers();
		return true;
	}

	bool WindowApplication::Initialize()
	{
		// super method
		if (!Application::Initialize())
			return false;
		// set error callback
		glfwSetErrorCallback(OnGLFWError);
		return true;
	}

	bool WindowApplication::Finalize()
	{	
		// destroy the resources
		FinalizeGPUResourceManager();
		// destroy all windows
		for (auto& window : windows)
		{
			if (window != nullptr)
			{
				window->Finalize();
				window->DestroyGLFWWindow();
			}
		}
		windows.clear();
		main_window = nullptr;
		// super
		Application::Finalize();
		return true;
	}

	void WindowApplication::FreeImageOutputMessageFunc(FREE_IMAGE_FORMAT fif, const char* msg)
	{
		Log::Message("FreeImage warning message [%d][%s]", fif, msg);
	}

	bool WindowApplication::InitializeStandardLibraries()
	{
		if (!Application::InitializeStandardLibraries())
			return false;
		FreeImage_Initialise(); // glew will be initialized 
		FreeImage_SetOutputMessage(&FreeImageOutputMessageFunc);
		glfwInit();
		return true;
	}

	bool WindowApplication::FinalizeStandardLibraries()
	{
		glfwTerminate();
		FreeImage_DeInitialise();
		Application::FinalizeStandardLibraries();
		return true;
	}

	void WindowApplication::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
	{
		if (main_window != nullptr)
			main_window->OnInputModeChanged(new_mode, old_mode);
	}

	Clock* WindowApplication::GetMainClockInstance()
	{
		WindowApplication* application = GetInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetMainClock();
	}

	Clock const* WindowApplication::GetMainClockConstInstance()
	{
		WindowApplication const* application = GetConstInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetMainClock();
	}

	SoundManager* WindowApplication::GetSoundManagerInstance()
	{
		WindowApplication* application = GetInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetSoundManager();
	}

	SoundManager const* WindowApplication::GetSoundManagerConstInstance()
	{
		WindowApplication const* application = GetConstInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetSoundManager();
	}

	GPUResourceManager* WindowApplication::GetGPUResourceManagerInstance()
	{
		WindowApplication* application = GetInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetGPUResourceManager();
	}

	GPUResourceManager const* WindowApplication::GetGPUResourceManagerConstInstance()
	{
		WindowApplication const* application = GetConstInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetGPUResourceManager();
	}

#if _DEBUG
	CHAOS_HELP_TEXT(SHORTCUTS, "F8  : ReloadGPUResources");
#endif

	bool WindowApplication::OnKeyEventImpl(KeyEvent const& event)
	{
		// reloading GPU resources		
#if _DEBUG
		if (event.IsKeyPressed(GLFW_KEY_F8))
		{
			// CMD GLFW_KEY_F8 : ReloadGPUResources(...)
			ReloadGPUResources();
			return true;
		}
#endif
		return Application::OnKeyEventImpl(event);
	}

	bool WindowApplication::GetApplicationKeyState(Key key, bool previous_frame)
	{
		WindowApplication* application = Application::GetInstance();
		if (application != nullptr)
			return application->GetKeyState(key, previous_frame);
		return false;
	}

	void WindowApplication::SetKeyboardButtonState(KeyboardButton key, int action)
	{
		int raw_value = (int)key;
		if (raw_value >= 0 && raw_value < keyboard_state.size())
			keyboard_state[raw_value].SetValue(action == GLFW_PRESS || action == GLFW_REPEAT);
	}

	void WindowApplication::SetMouseButtonState(MouseButton key, int action)
	{
		int raw_value = (int)key;
		if (raw_value >= 0 && raw_value < mouse_button_state.size())
			mouse_button_state[raw_value].SetValue(action == GLFW_PRESS || action == GLFW_REPEAT);
	}

	bool WindowApplication::GetKeyState(Key key, bool previous_frame) const
	{
		int raw_value = key.GetRawValue();

		if (key.GetType() == KeyType::KEYBOARD)
		{
			if (raw_value >= 0 && raw_value < keyboard_state.size())
				return keyboard_state[raw_value].GetValue(previous_frame);
		}
		else if (key.GetType() == KeyType::MOUSE)
		{
			if (raw_value >= 0 && raw_value < mouse_button_state.size())
				return mouse_button_state[raw_value].GetValue(previous_frame);
		}
		return false;
	}

	void WindowApplication::UpdateKeyStates(float delta_time)
	{
		for (ButtonState& button : keyboard_state)
			button.UpdateSameValueTimer(delta_time);
	}

	GLFWwindow* WindowApplication::GetSharedGLContext()
	{
		WindowApplication* application = Application::GetInstance();
		if (application != nullptr)
			return application->shared_context;
		return nullptr;
	}



}; // namespace chaos
