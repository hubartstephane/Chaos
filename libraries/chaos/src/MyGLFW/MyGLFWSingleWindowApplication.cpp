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
		// SingleWindowApplication
		//

		SingleWindowApplication::SingleWindowApplication(SubClassOf<Window> in_main_window_class, WindowParams const & in_window_params, WindowHints const in_window_hints) :
			main_window_class(in_main_window_class),
			window_params(in_window_params),
			window_hints(in_window_hints)
		{
			assert(in_main_window_class.IsValid());
		}
	
		void SingleWindowApplication::FreezeNextFrameTickDuration()
		{
			forced_zero_tick_duration = true;
		}

		bool SingleWindowApplication::MessageLoop()
		{
			GLFWwindow * glfw_window = window->GetGLFWHandler();

			double t1 = glfwGetTime();

			while (!window->ShouldClose())
			{




				BYTE NewKeyboardState[256] = { 0 };
				if (::GetKeyboardState(NewKeyboardState))
				{
					for (int i = 0; i < 256; ++i)
						if ((KeyboardState[i] & 128) != (NewKeyboardState[i] & 128))
							i = i;
					memcpy(KeyboardState, NewKeyboardState, sizeof(NewKeyboardState));
				}

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
				// tick the manager
				TickManagers(delta_time);
				// tick the window
				window->MainTick(delta_time, real_delta_time);
				// update time
				t1 = t2;
			}
			return true;
		}

		Window* SingleWindowApplication::CreateTypedWindow(SubClassOf<Window> window_class, WindowParams params, WindowHints hints)
		{
			// create the window class
			Window * result = window_class.CreateInstance();
			if (result == nullptr)
				return false;

			// compute the monitor upon which the window will be : use it for pixel format
			if (params.monitor == nullptr)
				params.monitor = GetMonitorByIndex(params.monitor_index);

			// retrieve the position of the monitor
			int monitor_x = 0;
			int monitor_y = 0;
			glfwGetMonitorPos(params.monitor, &monitor_x, &monitor_y);

			// compute the position and size of the window
			bool pseudo_fullscreen = (params.width <= 0 && params.height <= 0);

			// prepare window creation
			result->TweakHints(hints, params.monitor, pseudo_fullscreen);
			hints.ApplyHints();
			glfwWindowHint(GLFW_VISIBLE, 0); // override the initial visibility

			// compute window size and position
			GLFWvidmode const* mode = glfwGetVideoMode(params.monitor);

			int x = 0;
			int y = 0;
			if (pseudo_fullscreen) // full-screen, the window use the full-size
			{
				params.width = mode->width;
				params.height = mode->height;

				x = monitor_x;
				y = monitor_y;
			}
			else
			{
				if (params.width <= 0)
					params.width = mode->width;
				else
					params.width = std::min(mode->width, params.width);

				if (params.height <= 0)
					params.height = mode->height;
				else
					params.height = std::min(mode->height, params.height);

				x = monitor_x + (mode->width - params.width) / 2;
				y = monitor_y + (mode->height - params.height) / 2;
			}

			// title cannot be null
			if (params.title == nullptr) 
				params.title = "";

			// we are doing a pseudo fullscreen => monitor parameters of glfwCreateWindow must be null or it will "capture" the screen
			GLFWwindow* glfw_window = glfwCreateWindow(params.width, params.height, params.title, nullptr /* monitor */, nullptr /* share list */);
			if (glfw_window == nullptr)
			{
				delete(result);
				return nullptr;
			}

			glfwMakeContextCurrent(glfw_window);

			// vsync ?
			if (hints.unlimited_fps)
				glfwSwapInterval(0);

			// bind the window
			result->BindGLFWWindow(glfw_window, hints.double_buffer ? true : false);

			// x and y are the coordinates of the client area : when there is a decoration, we want to tweak the window size / position with that
			int left, top, right, bottom;
			glfwGetWindowFrameSize(glfw_window, &left, &top, &right, &bottom);
			if (left != 0 || top != 0 || right != 0 || bottom != 0)
			{
				x += left;
				y += top;
				params.width = params.width - left - right;
				params.height = params.height - top - bottom;
				glfwSetWindowSize(glfw_window, params.width, params.height);
			}

			glfwSetWindowPos(glfw_window, x, y);
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			//  glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			glfwSetInputMode(glfw_window, GLFW_STICKY_KEYS, 1);

			// now that the window is fully placed ... we can show it
			if (hints.start_visible)
				glfwShowWindow(glfw_window);

			return result;
		}

		bool SingleWindowApplication::Main()
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
			window = CreateTypedWindow(main_window_class, params, hints);
			if (window == nullptr)
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
			window->InitializeFromConfiguration(configuration, configuration_path);

			// the main loop
			MessageLoop();

			return true;
		}

		
		void SingleWindowApplication::TickManagers(float delta_time)
		{

			if (main_clock != nullptr)
				main_clock->TickClock(delta_time);
			if (sound_manager != nullptr)
				sound_manager->Tick(delta_time);
		}

		void SingleWindowApplication::OnGLFWError(int code, const char* msg)
		{
			LogTools::Log("Window(...) [%d] failure : %s", code, msg);
		}

		bool SingleWindowApplication::InitializeGPUResourceManager()
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

		bool SingleWindowApplication::ReloadGPUResources()
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

		bool SingleWindowApplication::FinalizeGPUResourceManager()
		{
			// stop the resource manager
			if (gpu_resource_manager != nullptr)
			{
				gpu_resource_manager->StopManager();
				gpu_resource_manager = nullptr;
			}
			return true;
		}

		bool SingleWindowApplication::InitializeManagers()
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

		bool SingleWindowApplication::FinalizeManagers()
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

		bool SingleWindowApplication::Initialize()
		{
			// super method
			if (!Application::Initialize())
				return false;
			// set error callback
			glfwSetErrorCallback(OnGLFWError);
			return true;
		}

		bool SingleWindowApplication::Finalize()
		{
			FinalizeGPUResourceManager();

			// stop the window
			if (window != nullptr)
			{
				window->Finalize();

				GLFWwindow * glfw_window = window->GetGLFWHandler();
				if (glfw_window != nullptr)
					glfwDestroyWindow(glfw_window);

				delete(window);
				window = nullptr;
			}
			Application::Finalize();
			return true;
		}

		void SingleWindowApplication::FreeImageOutputMessageFunc(FREE_IMAGE_FORMAT fif, const char *msg)
		{
			LogTools::Log("FreeImage warning message [%d][%s]", fif, msg);
		}

		bool SingleWindowApplication::InitializeStandardLibraries()
		{
			if (!Application::InitializeStandardLibraries())
				return false;
			FreeImage_Initialise(); // glew will be initialized 
			FreeImage_SetOutputMessage(&FreeImageOutputMessageFunc);
			glfwInit();
			return true;
		}

		bool SingleWindowApplication::FinalizeStandardLibraries()
		{
			glfwTerminate();
			FreeImage_DeInitialise();
			Application::FinalizeStandardLibraries();
			return true;
		}

		void SingleWindowApplication::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
		{
			if (window != nullptr)
				window->OnInputModeChanged(new_mode, old_mode);			
		}

		Clock * SingleWindowApplication::GetMainClockInstance()
		{
			SingleWindowApplication * application = GetInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetMainClock();
		}

		Clock const * SingleWindowApplication::GetMainClockConstInstance()
		{
			SingleWindowApplication const * application = GetConstInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetMainClock();
		}

		SoundManager * SingleWindowApplication::GetSoundManagerInstance()
		{
			SingleWindowApplication * application = GetInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetSoundManager();
		}

		SoundManager const * SingleWindowApplication::GetSoundManagerConstInstance()
		{
			SingleWindowApplication const * application = GetConstInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetSoundManager();
		}

		GPUResourceManager * SingleWindowApplication::GetGPUResourceManagerInstance()
		{
			SingleWindowApplication * application = GetInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetGPUResourceManager();
		}

		GPUResourceManager const * SingleWindowApplication::GetGPUResourceManagerConstInstance()
		{
			SingleWindowApplication const * application = GetConstInstance();
			if (application == nullptr)
				return nullptr;
			return application->GetGPUResourceManager();
		}

#if _DEBUG
		CHAOS_HELP_TEXT(SHORTCUTS, "F8  : ReloadGPUResources");
#endif

		bool SingleWindowApplication::OnKeyEventImpl(KeyEvent const& event)
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

		GLFWwindow* SingleWindowApplication::GetGLFWWindow() const
		{
			return (window == nullptr) ? nullptr : window->GetGLFWWindow();
		}

	}; // namespace MyGLFW

}; // namespace chaos
