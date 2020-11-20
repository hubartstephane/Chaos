#include <chaos/Chaos.h>

namespace chaos
{
	namespace MyGLFW
	{
		// 
		// WindowParams
		//

		bool SaveIntoJSON(nlohmann::json& json_entry, WindowParams const& src)
		{
			if (!json_entry.is_object())
				json_entry = nlohmann::json::object();
			JSONTools::SetAttribute(json_entry, "monitor_index", src.monitor_index);
			JSONTools::SetAttribute(json_entry, "width", src.width);
			JSONTools::SetAttribute(json_entry, "height", src.height);
			return true;
		}

		bool LoadFromJSON(nlohmann::json const& json_entry, WindowParams& dst)
		{
			if (!json_entry.is_object())
				return false;
			JSONTools::GetAttribute(json_entry, "monitor_index", dst.monitor_index);
			JSONTools::GetAttribute(json_entry, "width", dst.width);
			JSONTools::GetAttribute(json_entry, "height", dst.height);
			return true;
		}

		// 
		// WindowApplication
		//

		WindowApplication::WindowApplication(SubClassOf<Window> in_main_window_class, WindowParams const & in_window_params, WindowHints const in_window_hints) :
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
			GLFWwindow * glfw_window = main_window->GetGLFWHandler();

			double t1 = glfwGetTime();

			while (!main_window->ShouldClose())
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
				Tick(delta_time);
				// tick the windows
				for (auto & window : windows)
					window->MainTick(delta_time, real_delta_time);
				// update time
				t1 = t2;
			}
			return true;
		}

		Window* WindowApplication::CreateTypedWindow(SubClassOf<Window> window_class, WindowParams const & params, WindowHints const & hints)
		{
			// create the window class
			Window * result = window_class.CreateInstance();
			if (result == nullptr)
				return false;
			// create the GLFW resource
			if (!result->CreateGLFWWindow(params, hints, main_window.get())) // this should work even for the main window creation itself
			{
				delete(result);
				return nullptr;
			}
			// store the result
			windows.push_back(result);
			return result;
		}

		bool WindowApplication::Main()
		{
			// the glfw configuration
			GLFWHints glfw_hints;
			nlohmann::json const* glfw_configuration = JSONTools::GetStructure(configuration, "glfw");
			if (glfw_configuration != nullptr)
				LoadFromJSON(glfw_configuration, glfw_hints);
			glfw_hints.ApplyHints();

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

			// XXX : seems to be mandatory for some functions like : glGenVertexArrays(...)
			//       see https://www.opengl.org/wiki/OpenGL_Loading_Library
			glewExperimental = GL_TRUE;
			// create the context
			GLenum err = glewInit();
			if (err != GLEW_OK)
			{
				LogTools::Log("glewInit(...) failure : %s", glewGetErrorString(err));
				return false;
			}

			// set the debug log hander
			GLTools::SetDebugMessageHandler();
			// some generic information
			GLTools::DisplayGenericInformation();

			// initialize the GPU resource Manager (first window/OpenGL context must have been created)
			if (!InitializeGPUResourceManager())
				return false;

			// XXX : initialize the window (once GPUResourceManager is fully initialized)
			main_window->InitializeFromConfiguration(configuration, configuration_path);

			// a final initialization (after main window is constructed ... and OpenGL context)
			if (!PreMainLoop())
				return false;
			// the main loop
			MessageLoop();

			return true;
		}

		bool WindowApplication::PreMainLoop()
		{
			return true;
		}

		void WindowApplication::Tick(float delta_time)
		{
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
			LogTools::Log("Window(...) [%d] failure : %s", code, msg);
		}

		bool WindowApplication::InitializeGPUResourceManager()
		{
			// create and start the GPU manager
			gpu_resource_manager = new GPUResourceManager;
			if (gpu_resource_manager == nullptr)
				return false;
			gpu_resource_manager->StartManager();
			// create internal resource
			if (!gpu_resource_manager->InitializeInternalResources())
				return false;
			// get JSON section and load all requested resources
			nlohmann::json const * gpu_config = JSONTools::GetStructure(configuration, "gpu");
			if (gpu_config != nullptr)
				if (!gpu_resource_manager->InitializeFromConfiguration(*gpu_config, configuration_path))
					return false;
			return true;
		}

		bool WindowApplication::ReloadGPUResources()
		{
			// this call may block for too much time
			FreezeNextFrameTickDuration();

			// reload the configuration file
			nlohmann::json config;
			if (!ReloadConfigurationFile(config))
				return false;
			// get the structure of interrest
			nlohmann::json const * gpu_config = JSONTools::GetStructure(config, "gpu");
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
			nlohmann::json const * clock_config = JSONTools::GetStructure(configuration, "clocks");
			if (clock_config != nullptr)
				main_clock->InitializeFromConfiguration(*clock_config, configuration_path);

			// initialize the sound manager
			sound_manager = new SoundManager();
			if (sound_manager == nullptr)
				return false;
			sound_manager->StartManager();
			nlohmann::json const * sound_config = JSONTools::GetStructure(configuration, "sounds");
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
			FinalizeGPUResourceManager();

			// stop the window
			if (main_window != nullptr)
			{
				main_window->Finalize();

				GLFWwindow * glfw_window = main_window->GetGLFWHandler();
				if (glfw_window != nullptr)
					glfwDestroyWindow(glfw_window);

				main_window = nullptr;
			}
			Application::Finalize();
			return true;
		}

		void WindowApplication::FreeImageOutputMessageFunc(FREE_IMAGE_FORMAT fif, const char *msg)
		{
			LogTools::Log("FreeImage warning message [%d][%s]", fif, msg);
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

		Clock * WindowApplication::GetMainClockInstance()
		{
			WindowApplication * application = GetInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetMainClock();
		}

		Clock const * WindowApplication::GetMainClockConstInstance()
		{
			WindowApplication const * application = GetConstInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetMainClock();
		}

		SoundManager * WindowApplication::GetSoundManagerInstance()
		{
			WindowApplication * application = GetInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetSoundManager();
		}

		SoundManager const * WindowApplication::GetSoundManagerConstInstance()
		{
			WindowApplication const * application = GetConstInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetSoundManager();
		}

		GPUResourceManager * WindowApplication::GetGPUResourceManagerInstance()
		{
			WindowApplication * application = GetInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetGPUResourceManager();
		}

		GPUResourceManager const * WindowApplication::GetGPUResourceManagerConstInstance()
		{
			WindowApplication const * application = GetConstInstance();
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

		bool WindowApplication::GetApplicationKeyState(int key, bool previous_frame)
		{
			WindowApplication * application = Application::GetInstance();
			if (application != nullptr)
				return application->GetKeyState(key, previous_frame);
			return false;
		}

		void WindowApplication::SetKeyState(int key, int action)
		{
			if (key >= 0 && key < keyboard_state.size())
			{
				keyboard_state[key].SetValue(action == GLFW_PRESS || action == GLFW_REPEAT);
			}
		}
		
		bool WindowApplication::GetKeyState(int key, bool previous_frame) const
		{
			if (key >= 0 && key < keyboard_state.size())
			{
				return keyboard_state[key].GetValue(previous_frame);
			}
			return false;
		}

		void WindowApplication::UpdateKeyStates(float delta_time)
		{
			for (ButtonState& button : keyboard_state)
				button.UpdateSameValueTimer(delta_time);
		}


	}; // namespace MyGLFW

}; // namespace chaos
