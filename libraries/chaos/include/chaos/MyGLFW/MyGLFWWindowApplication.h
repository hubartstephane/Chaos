#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace MyGLFW
	{
		class WindowParams;
		class WindowApplication;

	}; // namespace MyGLFW

}; // namespace chaos

#else

namespace chaos
{
	namespace MyGLFW
	{

		/**
		* WindowParams : parameters for playing single window application
		*/

		class WindowParams
		{
		public:

			/** the title */
			char const * title = nullptr;
			/** the wanted monitor */
			GLFWmonitor * monitor = nullptr;
			/** the monitor index */
			int monitor_index = 0;
			/** window width */
			int width = 0;
			/** window height */
			int height = 0;
		};

		bool SaveIntoJSON(nlohmann::json& json_entry, WindowParams const& src);

		bool LoadFromJSON(nlohmann::json const& json_entry, WindowParams& dst);

		/**
		* WindowApplication
		*/

		class WindowApplication : public Application
		{
			friend class Window;

		public:

			/** constructor */
			WindowApplication(SubClassOf<Window> in_main_window_class, WindowParams const & in_window_params, WindowHints const in_window_hints);

			/** getter of the main clock */
			static Clock * GetMainClockInstance();
			/** getter of the main clock */
			static Clock const * GetMainClockConstInstance();

			/** getter of the sound manager */
			static SoundManager * GetSoundManagerInstance();
			/** getter of the sound manager */
			static SoundManager const * GetSoundManagerConstInstance();

			/** getter of the GPU resource manager */
			static GPUResourceManager * GetGPUResourceManagerInstance();
			/** getter of the GPU resource manager */
			static GPUResourceManager const * GetGPUResourceManagerConstInstance();


			/** gets the main clock */
			Clock * GetMainClock() { return main_clock.get(); }
			/** gets the main clock */
			Clock const * GetMainClock() const { return main_clock.get(); }

			/** gets the sound manager */
			SoundManager * GetSoundManager() { return sound_manager.get(); }
			/** gets the sound manager */
			SoundManager const * GetSoundManager() const { return sound_manager.get(); }

			/** gets the graphic resource manager */
			GPUResourceManager * GetGPUResourceManager() { return gpu_resource_manager.get(); }
			/** gets the graphic resource manager */
			GPUResourceManager const * GetGPUResourceManager() const { return gpu_resource_manager.get(); }

			/** used to force for one frame the duration of tick function to 0 : usefull for function that are long and would block the game for some time */
			void FreezeNextFrameTickDuration();

			/** reload all GPU resources */
			virtual bool ReloadGPUResources();
			/** override */
			virtual bool OnKeyEventImpl(KeyEvent const & event) override;

			/** get the state of a keyboard key */
			static bool GetApplicationKeyState(int key, bool previous_frame = false);
			/** change the state of a keyboard key */
			bool GetKeyState(int key, bool previous_frame = false) const;

		protected:

			/** Main method */
			virtual bool Main() override;
			/** the possibility to have final initialization before the main loop is run */
			virtual bool PreMainLoop();
			/** Window Loop */
			virtual bool MessageLoop();

			/** an error callback */
			static void OnGLFWError(int code, const char* msg);
			/** a debugging function to output some message from FreeImage */
			static void FreeImageOutputMessageFunc(FREE_IMAGE_FORMAT fif, const char *msg);
			/** initializing standard libraries */
			virtual bool InitializeStandardLibraries() override;
			/** Finalizalizing standard libraries */
			virtual bool FinalizeStandardLibraries() override;
			/** initialize the application */
			virtual bool Initialize() override;
			/** finalize the application */
			virtual bool Finalize() override;
			/** initialize the managers */
			virtual bool InitializeManagers() override;
			/** finalize the managers */
			virtual bool FinalizeManagers() override;
			/** the GPU manager must be initialized after the OpenGL context is OK. */
			virtual bool InitializeGPUResourceManager();
			/** finalize the GPU manager */
			virtual bool FinalizeGPUResourceManager();

			/** change the state of a keyboard key */
			void SetKeyState(int key, int action);
			/** update the internal timers of keyboard states */
			void UpdateKeyStates(float delta_time);

			/** custom tick */
			virtual void Tick(float delta_time);

			/** the user callback called when current input mode changes */
			virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode) override;

			/** create a window */
			Window * CreateTypedWindow(SubClassOf<Window> window_class, WindowParams const & params, WindowHints const & hints);

		protected:
		
			/** the main clock of the manager */
			shared_ptr<Clock> main_clock;
			/** the sound manager */
			shared_ptr<SoundManager> sound_manager;
			/** the graphic resource manager */
			shared_ptr<GPUResourceManager> gpu_resource_manager;

			/** the initial_window param */
			WindowParams window_params;
			/** the initial_window hints */
			WindowHints window_hints;
			/** the class of the main window */
			SubClassOf<Window> main_window_class;

			/** the main window */
			shared_ptr<Window> main_window;
			/** the window list */
			std::vector<shared_ptr<Window>> windows;

			/** the state of all buttons */
			std::array<ButtonState, GLFW_KEY_LAST> keyboard_state;
			
			/** forced time slice for tick */
			float forced_tick_duration = 0.0f;
			/** maximum time slice for tick */
			float max_tick_duration = 0.0f;
			/** whether the delta time is forced to 0 for one frame (usefull for long operations like screen capture or GPU resource reloading) */
			bool forced_zero_tick_duration = false;
		};

		/**
		* RunWindowApplication : utility template function to run an application only from a class
		*/

		template<typename WINDOW_TYPE, typename ...PARAMS>
		bool RunWindowApplication(int argc, char** argv, char** env, PARAMS... params)
		{
			return RunApplication<WindowApplication>(argc, argv, env, WINDOW_TYPE::GetStaticClass(), params...);
		}

	}; // namespace MyGLFW

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


