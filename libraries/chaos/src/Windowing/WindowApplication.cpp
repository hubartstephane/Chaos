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

	bool SaveIntoJSON(nlohmann::json& json_entry, GLFWHints const& src)
	{
		if (!json_entry.is_object())
			json_entry = nlohmann::json::object();
		JSONTools::SetAttribute(json_entry, "debug_context", src.debug_context);
		JSONTools::SetAttribute(json_entry, "major_version", src.major_version);
		JSONTools::SetAttribute(json_entry, "minor_version", src.minor_version);
		JSONTools::SetAttribute(json_entry, "refresh_rate", src.refresh_rate);
		JSONTools::SetAttribute(json_entry, "opengl_profile", src.opengl_profile);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json_entry, GLFWHints& dst)
	{
		if (!json_entry.is_object())
			return false;
		JSONTools::GetAttribute(json_entry, "debug_context", dst.debug_context);
		JSONTools::GetAttribute(json_entry, "major_version", dst.major_version);
		JSONTools::GetAttribute(json_entry, "minor_version", dst.minor_version);
		JSONTools::GetAttribute(json_entry, "refresh_rate", dst.refresh_rate);
		JSONTools::GetAttribute(json_entry, "opengl_profile", dst.opengl_profile);
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
			LoadFromJSON(glfw_configuration, glfw_hints);
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
			if (!InitializeGPUResourceManager())
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
			return main_window->InitializeFromConfiguration(configuration, configuration_path);
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

	bool WindowApplication::PreMessageLoop()
	{
		assert(glfwGetCurrentContext() == shared_context);
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

	bool WindowApplication::InitializeGPUResourceManager()
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
			if (!gpu_resource_manager->InitializeFromConfiguration(*gpu_config, configuration_path))
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
			if (other_gpu_resource_manager->InitializeFromConfiguration(*gpu_config, configuration_path))
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
			main_clock->InitializeFromConfiguration(*clock_config, configuration_path);

		// initialize the sound manager
		sound_manager = new SoundManager();
		if (sound_manager == nullptr)
			return false;
		sound_manager->StartManager();
		nlohmann::json const* sound_config = JSONTools::GetStructure(configuration, "sounds");
		if (sound_config != nullptr)
			sound_manager->InitializeFromConfiguration(*sound_config, configuration_path);

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

	void WindowApplication::SetKeyState(int key, int action)
	{
		if (key >= 0 && key < keyboard_state.size())
			keyboard_state[key].SetValue(action == GLFW_PRESS || action == GLFW_REPEAT);
	}




	bool WindowApplication::GetKeyState(Key key, bool previous_frame) const
	{
		int raw_value = key.GetRawValue();

		if (key.GetType() == KeyType::GAMEPAD)
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
