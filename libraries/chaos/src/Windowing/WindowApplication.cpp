#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// WindowApplication
	//

	WindowApplication::WindowApplication(SubClassOf<Window> in_main_window_class, WindowPlacementInfo const& in_main_window_placement_info, WindowCreateParams const& in_main_window_create_params):
		main_window_class(in_main_window_class),
		main_window_placement_info(in_main_window_placement_info),
		main_window_create_params(in_main_window_create_params)
	{
		assert(in_main_window_class.IsValid());
	}

	InputMode WindowApplication::GetApplicationInputMode()
	{
		WindowApplication const* application = Application::GetConstInstance();
		if (application != nullptr)
			return application->GetInputMode();
		return InputMode::KEYBOARD;
	}

	void WindowApplication::SetApplicationInputMode(InputMode new_mode)
	{
		WindowApplication* application = Application::GetInstance();
		if (application != nullptr)
			application->SetInputMode(new_mode);
	}

	void WindowApplication::FreezeNextFrameTickDuration()
	{
		forced_zero_tick_duration = true;
	}

	bool WindowApplication::IsWindowHandledByApplication(Window const* window) const
	{
		auto it = std::ranges::find_if(windows, [window](shared_ptr<Window> const& w)
		{
			return (w == window);
		});
		return (it != windows.end());
	}

	void WindowApplication::DestroyWindow(Window* window)
	{
		assert(window != nullptr);

		auto it = std::ranges::find_if(windows, [window](shared_ptr<Window> const& w)
		{
			return (w == window);
		});

		if (it != windows.end())
		{
			shared_ptr<Window> prevent_destruction = window;
			windows.erase(it);
			if (window->GetWindowDestructionGuard() == 0) // can destroy immediatly the window or must wait until no current operation ?
				OnWindowDestroyed(window);
		}
	}

	void WindowApplication::RunMessageLoop(LightweightFunction<bool()> loop_condition_func)
	{
		double t1 = glfwGetTime();

		while (!loop_condition_func || loop_condition_func())
		{
			glfwPollEvents();

			double t2 = glfwGetTime();
			float delta_time = (float)(t2 - t1);
			float real_delta_time = delta_time;

			if (forced_zero_tick_duration) // a single frame with a zero delta_time (used whenever a long operation is beeing done during one frame and we don't want to have big dt)
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
			bool tick_result = WithGLFWContext(shared_context, [this, delta_time]()
			{
				return Tick(delta_time);
			});
			if (!tick_result) // quit the loop if the current tick method requires so
			{
				return;
			}

			// destroy windows that mean to be
			ForAllWindows([](Window* window)
			{
				if (window->ShouldClose())
					window->Destroy();
			});

			// tick the windows
			ForAllWindows([delta_time, real_delta_time](Window* window)
			{
				window->WithWindowContext([&window, delta_time, real_delta_time]()
				{
					window->TickRenderer(real_delta_time);
					window->Tick(delta_time);
					window->DrawWindow();
				});
			});
			// update time
			t1 = t2;
		}
	}

	void WindowApplication::DestroyAllWindows()
	{
		ForAllWindows([](Window* window)
		{
			window->Destroy();
		});
	}

	Window* WindowApplication::CreateTypedWindow(CreateWindowFunc create_func, WindowPlacementInfo placement_info, WindowCreateParams const &create_params, ObjectRequest request)
	{
		if (FindWindow(request) != nullptr)
		{
			ApplicationLog::Error("WindowApplication::CreateTypedWindow(...) name already used");
			return nullptr;
		}

		return WithGLFWContext(nullptr, [this, create_func, &placement_info , &create_params, &request]() -> Window*
		{
			// create the window class
			shared_ptr<Window> result = create_func();
			if (result == nullptr)
				return nullptr;
			windows.push_back(result.get());
			// set the name
			result->SetObjectNaming(request);
			// set the configuration
			GiveChildConfiguration(result.get(), StringTools::Join("/", "windows", result->GetName()));

			// search size and position values
			// 1-saved configuration overrides given parameters
			// 2-use given parameters
			// 3-use fallback configuration
			JSONReadConfiguration config = result->GetJSONReadConfiguration();

			glm::ivec2 window_size;
			if (JSONTools::GetAttribute(config, "size", window_size))
				placement_info.size = window_size;
			else if (!placement_info.size.has_value())
				JSONTools::GetAttribute(config, "default_size", placement_info.size);

			glm::ivec2 window_position;
			if (JSONTools::GetAttribute(config, "position", window_position))
				placement_info.position = window_position;
			else if (!placement_info.position.has_value())
				JSONTools::GetAttribute(config, "default_position", placement_info.position);

			int monitor_index = 0;
			if (JSONTools::GetAttribute(config, "monitor_index", monitor_index))
				placement_info.monitor_index = monitor_index;

			if (!JSONTools::GetAttribute(config, "fullscreen", placement_info.fullscreen))
				JSONTools::GetAttribute(config, "default_fullscreen", placement_info.fullscreen);

			// create the GLFW resource
			if (!result->CreateGLFWWindow(placement_info, create_params, shared_context, glfw_hints))
			{
				result->Destroy();
				return nullptr; // the shared_ptr destruction will handle the object lifetime
			}
			// post initialization method
			glfwMakeContextCurrent(result->GetGLFWHandler());
			// initialize ImGui
			result->CreateImGuiContext();

			// finalize the creation
			if (!result->Initialize())
			{
				result->Destroy();
				return nullptr; // the shared_ptr destruction will handle the object lifetime
			}
			// create the root widget
			result->CreateRootWidget();
			// notify the application
			OnWindowCreated(result.get());
			return result.get();
		});
	}

	void WindowApplication::OnWindowDestroyed(Window* window)
	{
		// destroy the window for real
		window->StorePersistentProperties(true);
		window->Finalize();
		window->DestroyImGuiContext();
		window->DestroyGLFWWindow();
	}

	bool WindowApplication::HasMainWindow() const
	{
		for (shared_ptr<Window> const& window : windows)
			if (window->GetWindowCategory() == WindowCategory::MAIN_WINDOW)
				return true;
		return false;
	}

	void WindowApplication::OnWindowCreated(Window* window)
	{
	}

	void WindowApplication::FinalizeSharedContext()
	{
		if (shared_context != nullptr)
		{
			glfwDestroyWindow(shared_context);
			shared_context = nullptr;
		}
	}

	bool WindowApplication::CreateSharedContext()
	{
		// set error callback
		glfwSetErrorCallback(OnGLFWError);

		// the glfw configuration (valid for all windows)
		if (JSONReadConfiguration glfw_configuration = JSONTools::GetAttributeStructureNode(GetJSONReadConfiguration(), "glfw"))
			LoadFromJSON(glfw_configuration, glfw_hints);
		glfw_hints.ApplyHints();

		// create a hidden window whose purpose is to provide a sharable context for all others
		glfwWindowHint(GLFW_VISIBLE, 0);
		shared_context = glfwCreateWindow(100, 100, "", nullptr, nullptr);
		if (shared_context == nullptr)
			return false;

		if (!WithGLFWContext(shared_context, [this]()
		{
			// XXX : seems to be mandatory for some functions like : glGenVertexArrays(...)
			//       see https://www.opengl.org/wiki/OpenGL_Loading_Library
			glewExperimental = GL_TRUE;
			// create the context
			GLenum err = glewInit();
			if (err != GLEW_OK)
			{
				ApplicationLog::Message("glewInit(...) failure : %s", glewGetErrorString(err));
				return false;
			}
			// callback for monitor events
			glfwSetMonitorCallback(DoOnMonitorEvent);
			// set the debug log hander
			GLTools::SetDebugMessageHandler();
			// some generic information
			GLTools::DisplayGenericInformation();
			return true;
		}))
		{
			return false;
		}

		return true;
	}

	bool WindowApplication::Initialize()
	{
		// create an invisible window to share its context with all others (must be called before super method)
		if (!CreateSharedContext())
			return false;

		// super method
		if (!Application::Initialize())
			return false;

		// intialize the resources for GPU
		if (!WithGLFWContext(shared_context, [this]()
		{
			return InitializeGPUResources();
		}))
		{
			return false;
		}
		return true;
	}

	int WindowApplication::Main()
	{

#if _WIN32
		// prevent windows to be in pause
		SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
#endif // #if _WIN32

		// open windows that were there during previous session (after main window because main window can take a few second to initialize)
		std::vector<std::string> opened_window;
		if (JSONTools::GetAttribute(GetJSONReadConfiguration(), "opened_window", opened_window))
			for (std::string const& name : opened_window)
				CreateOrDestroyKnownWindow(name.c_str(), true);

		// real main function
		int result = MainBody();
		// destroy remaining windows
		Quit();

		return result;
	}

	int WindowApplication::MainBody()
	{
		// create the main window
		Window* main_window = CreateMainWindow();
		if (main_window == nullptr)
			return -1;

		// run the main loop as long as there are main windows
		RunMessageLoop([this]()
		{
			return HasMainWindow();
		});

		return 0;
	}

	void WindowApplication::Quit()
	{
		// prevent Quit() reentrance
		if (is_quitting)
			return;
		is_quitting = true;

		// save the name of all windows that are still opened before closing them all
		std::vector<char const*> names;
		for (shared_ptr<Window> const& window : windows)
			names.push_back(window->GetName());
		JSONTools::SetAttribute(GetJSONWriteConfiguration(), "opened_window", names);

		// effectively quit the application
		DestroyAllWindows();
	}

	Window* WindowApplication::CreateMainWindow()
	{
		WindowCreateParams create_params = main_window_create_params;

		// gives a title to the window (if necessary)
		if (StringTools::IsEmpty(create_params.title))
			if (GetArguments().size() > 0)
				create_params.title = PathTools::PathToName(GetArguments()[0]);
		// create the window
		return DoCreateMainWindow(create_params);
	}

	Window * WindowApplication::DoCreateMainWindow(WindowCreateParams const & create_params)
	{
		return CreateTypedWindow(main_window_class, main_window_placement_info, create_params, "main_window");
	}




	// shuyyy

	bool WindowApplication::InitializeGamepadButtonMap()
	{
		// the map [button ID] => [bitmap name + text generator alias]
		gamepad_button_map[GamepadButton::A] = { "XboxOne_A", "ButtonA" };
		gamepad_button_map[GamepadButton::B] = { "XboxOne_B", "ButtonB" };
		gamepad_button_map[GamepadButton::X] = { "XboxOne_X", "ButtonX" };
		gamepad_button_map[GamepadButton::Y] = { "XboxOne_Y", "ButtonY" };

		gamepad_button_map[GamepadButton::DPAD_DOWN] = { "XboxOne_DPad_Down", "DPAD_Down" };
		gamepad_button_map[GamepadButton::DPAD_UP] = { "XboxOne_DPad_Up", "DPAD_Up" };
		gamepad_button_map[GamepadButton::DPAD_LEFT] = { "XboxOne_DPad_Left", "DPAD_Left" };
		gamepad_button_map[GamepadButton::DPAD_RIGHT] = { "XboxOne_DPad_Right", "DPAD_Right" };

		gamepad_button_map[GamepadButton::LEFT_BUMPER] = { "XboxOne_LB", "LeftShoulder" };
		gamepad_button_map[GamepadButton::RIGHT_BUMPER] = { "XboxOne_RB", "RightShoulder" };
		gamepad_button_map[GamepadButton::LEFT_TRIGGER] = { "XboxOne_LT", "LeftTrigger" };
		gamepad_button_map[GamepadButton::RIGHT_TRIGGER] = { "XboxOne_RT", "RightTrigger" };

		gamepad_button_map[GamepadButton::SPECIAL_LEFT] = { "XboxOne_Windows", "SpecialLeft" };
		gamepad_button_map[GamepadButton::SPECIAL_RIGHT] = { "XboxOne_Menu", "SpecialRight" };

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
		if (!JSONTools::GetAttribute(GetJSONReadConfiguration(), "sprite_directory", sprite_directory))
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
		if (JSONReadConfiguration fonts_config = JSONTools::GetAttributeStructureNode(GetJSONReadConfiguration(), "fonts"))
		{
			// read the default font parameters
			BitmapAtlas::FontInfoInputParams font_params;
			if (JSONReadConfiguration default_font_param_json = JSONTools::GetAttributeStructureNode(fonts_config, "default_font_param"))
				LoadFromJSON(default_font_param_json, font_params);

			// Add the fonts
			if (JSONReadConfiguration fonts_json = JSONTools::GetAttributeObjectNode(fonts_config, "fonts"))
			{
				JSONTools::ForEachSource(fonts_json, [this, &input, &font_params](nlohmann::json const * json)
				{
					for (nlohmann::json::const_iterator it = json->begin(); it != json->end(); ++it)
					{
						if (!it->is_string())
							continue;
						// read information
						std::string const& font_name = it.key();
						std::string font_path = it->get<std::string>();
						if (input.AddFont(font_path.c_str(), nullptr, true, font_name.c_str(), 0, font_params) == nullptr)
							ApplicationLog::Error("FillAtlasGeneratorInputFonts(...): fails to AddFont name = [%s]    path = [%s]", font_name.c_str(), font_path.c_str());
					}
					return true; // stop at the very first source
				});
			}
		}
		return true;
	}

	bool WindowApplication::CreateTextureAtlas()
	{
		// fill sub images for atlas generation
		BitmapAtlas::AtlasInput input;
		if (!FillAtlasGeneratorInput(input))
			return false;

		// atlas generation params
		int const DEFAULT_ATLAS_SIZE = 1024;
		int const DEFAULT_ATLAS_PADDING = 10;

		BitmapAtlas::AtlasGeneratorParams params = BitmapAtlas::AtlasGeneratorParams(DEFAULT_ATLAS_SIZE, DEFAULT_ATLAS_SIZE, DEFAULT_ATLAS_PADDING, PixelFormatMergeParams());

		if (JSONReadConfiguration atlas_config = JSONTools::GetAttributeStructureNode(GetJSONReadConfiguration(), "atlas"))
			LoadFromJSON(atlas_config, params);

		// generate the atlas
		BitmapAtlas::TextureArrayAtlasGenerator generator;
		texture_atlas = generator.ComputeResult(input, params);
		if (texture_atlas == nullptr)
			return false;

		return true;
	}

	bool WindowApplication::CreateTextGenerator()
	{
		// create the generator
		particle_text_generator = new ParticleTextGenerator::Generator(*texture_atlas);
		if (particle_text_generator == nullptr)
			return false;

		// get the font sub objects
		JSONReadConfiguration fonts_config = JSONTools::GetAttributeStructureNode(GetJSONReadConfiguration(), "fonts");

		// bitmaps in generator
		if (BitmapAtlas::FolderInfo const* folder_info = texture_atlas->GetFolderInfo("sprites"))
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
			if (fonts_config)
			{
				if (JSONReadConfiguration font_bitmaps_config = JSONTools::GetAttributeObjectNode(fonts_config, "bitmaps"))
				{
					JSONTools::ForEachSource(font_bitmaps_config, [this, folder_info](nlohmann::json const * json)
					{
						for (nlohmann::json::const_iterator it = json->begin(); it != json->end(); ++it)
						{
							if (!it->is_string())
								continue;
							std::string const& bitmap_name = it.key();
							std::string bitmap_path = it->get<std::string>();
							BitmapAtlas::BitmapInfo const* info = folder_info->GetBitmapInfo(bitmap_path.c_str());
							if (info == nullptr)
								continue;
							particle_text_generator->AddBitmap(bitmap_name.c_str(), info);
						}
						return true; // stop at the very first source
					});
				}
			}
		}

		// the colors
		if (fonts_config)
		{
			if (JSONReadConfiguration font_colors_config = JSONTools::GetAttributeObjectNode(fonts_config, "colors"))
			{
				JSONTools::ForEachSource(font_colors_config, [this](nlohmann::json const* json)
				{
					for (nlohmann::json::const_iterator it = json->begin(); it != json->end(); ++it)
					{
						glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // initialization for if input is smaller than 4
						if (!LoadFromJSON(&(*it), color))
							continue;
						std::string const& color_name = it.key();
						particle_text_generator->AddColor(color_name.c_str(), color);
					}
					return true; // stop at the very first source
				});
			}
		}
		return true;
	}

	bool WindowApplication::DoTick(float delta_time)
	{
		assert(glfwGetCurrentContext() == shared_context);

		// tick the managers
		if (main_clock != nullptr)
			main_clock->TickClock(delta_time);
		if (sound_manager != nullptr)
			sound_manager->Tick(delta_time);
		// update keyboard and mouse state
		KeyboardState::UpdateKeyStates(delta_time);
		return true;
	}

	void WindowApplication::OnGLFWError(int code, const char* msg)
	{
		ApplicationLog::Message("Window(...) [%d] failure : %s", code, msg);
	}

	bool WindowApplication::InitializeGPUResources()
	{
		assert(glfwGetCurrentContext() == shared_context);

		if (!CreateTextureAtlas())
		{
			ApplicationLog::Error("WindowApplication::CreateTextureAtlas(...) failure");
			return false;
		}
		if (!InitializeGamepadButtonMap())
		{
			ApplicationLog::Error("WindowApplication::InitializeGamepadButtonMap(...) failure");
			return false;
		}
		if (!CreateTextGenerator())
		{
			ApplicationLog::Error("WindowApplication::CreateTextGenerator(...) failure");
			return false;
		}
		return true;
	}

	void WindowApplication::FinalizeGPUResources()
	{
		particle_text_generator = nullptr;
		gamepad_button_map.clear();
		texture_atlas = nullptr;
	}

	bool WindowApplication::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		if (!Application::OnReadConfigurableProperties(config, context))
			return false;

		JSONTools::GetAttribute(config, "max_tick_duration", max_tick_duration);
		JSONTools::GetAttribute(config, "forced_tick_duration", forced_tick_duration);

		return true;
	}

	bool WindowApplication::OnStorePersistentProperties(JSONWriteConfiguration config) const
	{
		if (!Application::OnStorePersistentProperties(config))
			return false;

		return true;
	}

	bool WindowApplication::ReloadGPUResources()
	{
		assert(gpu_resource_manager != nullptr);

		return WithGLFWContext(shared_context, [this]()
		{
			// this call may block for too much time
			FreezeNextFrameTickDuration();
			// reload GPU resources
			return gpu_resource_manager->ReloadDefaultPropertiesFromFile(true, true); // partial, send notification
		});
	}

	bool WindowApplication::InitializeManagers()
	{
		if (!Application::InitializeManagers())
			return false;

		// initialize the clock
		main_clock = new Clock("main_clock");
		if (main_clock == nullptr)
			return false;

		// initialize the sound manager
		sound_manager = new SoundManager();
		if (sound_manager == nullptr)
			return false;
		GiveChildConfiguration(sound_manager.get(), "sounds");
		sound_manager->StartManager();

		// initialize the imgui manager
		imgui_manager = new ImGuiManager;
		if (imgui_manager == nullptr)
			return false;
		GiveChildConfiguration(imgui_manager.get(), "imgui");
		imgui_manager->StartManager();

		// create and start the GPU manager
		if (!WithGLFWContext(shared_context, [this]()
		{
			gpu_resource_manager = new GPUResourceManager;
			if (gpu_resource_manager == nullptr)
				return false;
			GiveChildConfiguration(gpu_resource_manager.get(), "gpu");
			return gpu_resource_manager->StartManager();
		}))
		{
			return false;
		}
		return true;
	}

	void WindowApplication::FinalizeManagers()
	{
		// stop the GPU manager
		if (gpu_resource_manager != nullptr)
		{
			WithGLFWContext(shared_context, [this]()
			{
				gpu_resource_manager->StopManager();
				gpu_resource_manager = nullptr;
			});
		}
		// stop the clock
		main_clock = nullptr;
		// stop the sound manager
		if (sound_manager != nullptr)
		{
			sound_manager->StopManager();
			sound_manager = nullptr;
		}
		// stop the imgui manager
		if (imgui_manager != nullptr)
		{
			imgui_manager->StopManager();
			imgui_manager = nullptr;
		}
		// super method
		Application::FinalizeManagers();
	}

	void WindowApplication::Finalize()
	{
		// destroy all windows
		DestroyAllWindows();
		// destroy the resources
		WithGLFWContext(shared_context, [this]()
		{
			FinalizeGPUResources();
		});
		// destroy the shared context
		FinalizeSharedContext();
		// super
		Application::Finalize();
	}

	void WindowApplication::FreeImageOutputMessageFunc(FREE_IMAGE_FORMAT fif, const char* msg)
	{
		ApplicationLog::Message("FreeImage warning message [%d][%s]", fif, msg);
	}

	bool WindowApplication::InitializeStandardLibraries()
	{
		if (!Application::InitializeStandardLibraries())
			return false;

		IMGUI_CHECKVERSION();
		FreeImage_Initialise();
		FreeImage_SetOutputMessage(&FreeImageOutputMessageFunc);
		glfwInit();
		return true;
	}

	void WindowApplication::FinalizeStandardLibraries()
	{
		glfwTerminate();
		FreeImage_DeInitialise();
		Application::FinalizeStandardLibraries();
	}

	void WindowApplication::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
	{
		ForAllWindows([new_mode, old_mode](Window * window)
		{
			window->WithWindowContext([&window, new_mode, old_mode]()
			{
				window->OnInputModeChanged(new_mode, old_mode);
			});
		});
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
	CHAOS_HELP_TEXT(SHORTCUTS, "F7  : ToggleImGuiMenuMode");
	CHAOS_HELP_TEXT(SHORTCUTS, "F8  : ReloadGPUResources");
#endif

	bool WindowApplication::OnKeyEventImpl(KeyEvent const& key_event)
	{
		if (key_event.IsKeyPressed(KeyboardButton::F7))
		{
			SetImGuiMenuEnabled(!IsImGuiMenuEnabled());
			return true;
		}

#if _DEBUG
		// reloading GPU resources
		if (key_event.IsKeyPressed(KeyboardButton::F8))
		{
			// CMD F8 : ReloadGPUResources(...)
			ReloadGPUResources();
			return true;
		}
#endif
		return InputEventReceiverInterface::OnKeyEventImpl(key_event);
	}

	GLFWwindow* WindowApplication::GetSharedGLContext()
	{
		WindowApplication* application = Application::GetInstance();
		if (application != nullptr)
			return application->shared_context;
		return nullptr;
	}

	size_t WindowApplication::GetWindowCount() const
	{
		return windows.size();
	}

	AutoCastable<Window> WindowApplication::GetWindow(size_t index)
	{
		return windows[index].get();
	}

	AutoConstCastable<Window> WindowApplication::GetWindow(size_t index) const
	{
		return windows[index].get();
	}

	AutoCastable<Window> WindowApplication::FindWindow(ObjectRequest request)
	{
		return request.FindObject(windows);
	}

	AutoConstCastable<Window> WindowApplication::FindWindow(ObjectRequest request) const
	{
		return request.FindObject(windows);
	}

	bool WindowApplication::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const
	{
		if (execution_data.Match("material", GPUProgramProviderPassType::FALLBACK))
		{
			return execution_data.Process(GetTextureAtlas()->GetTexture());
		}
		return false;
	}

	void WindowApplication::OnMonitorEvent(GLFWmonitor* monitor, int monitor_state)
	{
		// the GLFW monitor delegate does not send event to any window
		// that's why we are catching here the event and dispatching to all application's windows
		ForAllWindows([monitor, monitor_state](Window* window)
		{
			window->WithWindowContext([&window, monitor, monitor_state]()
			{
				if (window->GetImGuiContext() != nullptr)
					ImGui_ImplGlfw_MonitorCallback(monitor, monitor_state); // manually call ImGui delegate (see comment in WindowApplication::OnWindowCreated(...)
				window->OnMonitorEvent(monitor, monitor_state);
			});
		});
	}

	void WindowApplication::DoOnMonitorEvent(GLFWmonitor* monitor, int monitor_state)
	{
		if (WindowApplication* application = Application::GetInstance())
			application->OnMonitorEvent(monitor, monitor_state);
	}

	bool WindowApplication::IsImGuiMenuEnabled()
	{
		if (WindowApplication const* window_application = Application::GetInstance())
			return window_application->imgui_menu_enabled;
		return false;
	}

	void WindowApplication::SetImGuiMenuEnabled(bool enabled)
	{
		if (WindowApplication * window_application = Application::GetInstance())
		{
			if (window_application->imgui_menu_enabled != enabled)
			{
				window_application->imgui_menu_enabled = enabled;
				window_application->ForAllWindows([enabled](Window* window)
				{
					window->WithWindowContext([&window, enabled]()
					{
						window->OnImGuiMenuEnabledChanged(enabled);
					});
				});
			}
		}
	}

	Window * WindowApplication::CreateNamedWindow(char const* name, CreateWindowFunc create_func)
	{
		Window* existing_window = FindWindow(name);
		if (existing_window != nullptr)
			return existing_window;

		WindowCreateParams create_params;
		create_params.title = name;

		WindowPlacementInfo placement_info;
		placement_info.size = { 800, 800 };
		return CreateTypedWindow(create_func, placement_info, create_params, name);
	}

	void  WindowApplication::DestroyNamedWindow(char const* name)
	{
		Window* existing_window = FindWindow(name);
		if (existing_window != nullptr)
			existing_window->RequireWindowClosure();
	}

	void WindowApplication::CreateOrDestroyWindow(bool create, char const * name, CreateWindowFunc create_func)
	{
		if (create)
			CreateNamedWindow(name, create_func);
		else
			DestroyNamedWindow(name);
	}

	bool WindowApplication::CreateOrDestroyKnownWindow(char const* name, bool create)
	{
		return EnumerateKnownWindows([this, name, create](char const* window_name, CreateWindowFunc create_func)
		{
			// is it the window we are searching?
			if (StringTools::Stricmp(name, window_name) != 0)
				return false;
			CreateOrDestroyWindow(create, window_name, create_func);
			return true; // stop the search
		});
	}

	bool WindowApplication::EnumerateKnownWindows(EnumerateKnownWindowsFunc func) const
	{
		if (func("Log", &ImGuiWindow::CreateImGuiWindow<ImGuiLogObject>))
			return true;
		if (func("Global Variables", &ImGuiWindow::CreateImGuiWindow<ImGuiGlobalVariablesObject>))
			return true;
#if _DEBUG
		if (func("ImGui Demo", &ImGuiWindow::CreateImGuiWindow<ImGuiDemoObject>))
			return true;
		if (func("ImPlot Demo", &ImGuiWindow::CreateImGuiWindow<ImGuiImPlotDemoObject>))
			return true;
#endif
		return false;
	}

	void WindowApplication::OnDrawApplicationImGuiMenu(Window * window, BeginImGuiMenuFunc begin_menu_func)
	{
		begin_menu_func([this]()
		{
			if (ImGui::BeginMenu("Actions"))
			{
				if (ImGui::BeginMenu("Directories"))
				{
					if (ImGui::MenuItem("Open Resources Dir.", nullptr, false, true))
					{
						WinTools::ShowFile(GetResourcesPath());
					}
					if (ImGui::MenuItem("Open Temp Dir.", nullptr, false, true))
					{
						WinTools::ShowFile(GetUserLocalTempPath());
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Configuration"))
				{
					if (ImGui::MenuItem("Show Configuration", nullptr, false, true))
					{
						WinTools::ShowFile(JSONTools::DumpConfigFile(GetJSONReadConfiguration().default_config));
					}
					if (ImGui::MenuItem("Show Persistent Configuration", nullptr, false, true))
					{
						SavePersistentPropertiesToFile(false); // do not require an update from clients
						WinTools::ShowFile(GetPersistentDataPath());
					}
					if (ImGui::MenuItem("Delete Persistent Configuration", nullptr, false, true))
					{
						std::remove(GetPersistentDataPath().string().c_str());
					}
					ImGui::EndMenu();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Quit", nullptr, false, true))
				{
					Quit();
				}
				ImGui::EndMenu();
			}

			EnumerateKnownWindows([this](char const* name, CreateWindowFunc create_func)
			{
				if (ImGui::BeginMenu("Windows"))
				{
					bool window_exists = (FindWindow(name) != nullptr);
					if (ImGui::MenuItem(name, nullptr, window_exists, true))
						CreateOrDestroyWindow(!window_exists, name, create_func);
					ImGui::EndMenu();
				}
				return false; // don't stop the search
			});
		});

		if (gpu_resource_manager != nullptr)
			gpu_resource_manager->OnDrawImGuiMenu(window, begin_menu_func);
		if (imgui_manager != nullptr)
			imgui_manager->OnDrawImGuiMenu(window, begin_menu_func);
	}

	std::vector<weak_ptr<Window>> WindowApplication::GetWeakWindowArray() const
	{
		std::vector<weak_ptr<Window>> result;
		result.reserve(windows.size());
		for (shared_ptr<Window> const& window : windows)
			result.emplace_back(window.get());
		return result;
	}

#ifdef _WIN32

	void WindowApplication::OnInputLanguageChanged()
	{
		// update the layout
		KeyboardLayout::InvalidateCachedLayout();
		// propage the information to all windows (WM_INPUTLANGCHANGE is only sent to the topmost one)
		ForAllWindows([](Window * window)
		{
			window->OnInputLanguageChanged();
		});
	}

#endif // #if _WIN32

	ImGuiManager* WindowApplication::GetImGuiManager() const
	{
		return imgui_manager.get();
	}

}; // namespace chaos
