#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace MyGLFW
	{
		class SingleWindowApplicationParams;
		class SingleWindowApplication;

	}; // namespace MyGLFW

}; // namespace chaos

#else

namespace chaos
{
	namespace MyGLFW
	{

		/**
		* SingleWindowApplicationParams : parameters for playing single window application
		*/

		class SingleWindowApplicationParams
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
			/** the hints */
			WindowHints hints;
		};

		/**
		* SingleWindowApplication
		*/

		class SingleWindowApplication : public Application
		{

		public:

			/** constructor */
			SingleWindowApplication(SingleWindowApplicationParams const & in_window_params);

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
			GPUResourceManager * GetGPUResourceManager() { return gpu_manager.get(); }
			/** gets the graphic resource manager */
			GPUResourceManager const * GetGPUResourceManager() const { return gpu_manager.get(); }

			/** getting the renderer */
			GPURenderer * GetRenderer() { return renderer.get(); }

			/** used to force for one frame the duration of tick function to 0 : usefull for function that are long and would block the game for some time */
			void FreezeNextFrameTickDuration();

			/** reload all GPU resources */
			virtual bool ReloadGPUResources();
			/** override */
			virtual bool OnKeyEventImpl(KeyEvent const & event) override;

			/** override */
			virtual GLFWwindow* GetGLFWWindow() const override;

		protected:

			/** Main method */
			virtual bool Main() override;

			/** Window Loop */
			bool MessageLoop();

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
			virtual bool InitializeGPUManager();
			/** finalize the GPU manager */
			virtual bool FinalizeGPUManager();

			/** the method to override for window generation */
			virtual Window * GenerateWindow();
			/** tick all the managers */
			virtual void TickManagers(float delta_time);

			/** Tweak window hints from configuration */
			void TweakHintsFromConfiguration(SingleWindowApplicationParams & params, nlohmann::json const & in_config);

			/** the user callback called when current input mode changes */
			virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode) override;

		protected:

			/** the main clock of the manager */
			shared_ptr<Clock> main_clock;
			/** the sound manager */
			shared_ptr<SoundManager> sound_manager;
			/** the graphic resource manager */
			shared_ptr<GPUResourceManager> gpu_manager;

			/** the initial_window param */
			SingleWindowApplicationParams window_params;
			/** the window created */
			Window * window = nullptr;

			/** the renderer */
			shared_ptr<GPURenderer> renderer;

			
			/** forced time slice for tick */
			float forced_tick_duration = 0.0f;
			/** maximum time slice for tick */
			float max_tick_duration = 0.0f;
			/** whether the delta time is forced to 0 for one frame (usefull for long operations like screen capture or GPU resource reloading) */
			bool   forced_zero_tick_duration = false;
		};

		/**
		* RunWindowApplication : utility template function to run an application only from a class
		*/

		template<typename WINDOW_TYPE>
		bool RunWindowApplication(int argc, char ** argv, char ** env, SingleWindowApplicationParams const & in_window_params)
		{
			class MyApplication : public SingleWindowApplication
			{
			public:
				MyApplication(SingleWindowApplicationParams const & in_window_params) :
					SingleWindowApplication(in_window_params) {}
			protected:
				Window * GenerateWindow() override { return new WINDOW_TYPE; }
			};

			chaos::shared_ptr<MyApplication> application = new MyApplication(in_window_params);
			if (application != nullptr)
			{
				// XXX : under normal circonstances, you should not use CHAOS_PROJECT_SRC_PATH, CHAOS_PROJECT_BUILD_PATH in libraries
				//       here, this is an exception because this function is a template and so is not compiled in libraries by caller code instead
#if CHAOS_CAN_REDIRECT_RESOURCE_FILES
				static boost::filesystem::path src_path = CHAOS_PROJECT_SRC_PATH;
				static boost::filesystem::path build_path = CHAOS_PROJECT_BUILD_PATH;
				application->SetFileRedirectionDirectories(build_path, src_path);
#endif
				return application->Run(argc, argv, env);
			}
			return false;
		}

	}; // namespace MyGLFW

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


