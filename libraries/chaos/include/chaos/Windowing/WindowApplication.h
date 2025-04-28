namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WindowApplication;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* WindowApplication
	*/

	class CHAOS_API WindowApplication : public Application, public TickableInterface, public GPUProgramProviderInterface, public InputEventReceiverInterface
	{
		friend class Window;

		CHAOS_DECLARE_OBJECT_CLASS(WindowApplication, Application);

	public:

		using CreateWindowFunc = LightweightFunction<Window*()>;
		using EnumerateKnownWindowsFunc = LightweightFunction<bool(char const*, CreateWindowFunc)>;

		/** gets the number of windows */
		size_t GetWindowCount() const;
		/** get the window per index */
		AutoCastable<Window> GetWindow(size_t index);
		/** get the window per index */
		AutoConstCastable<Window> GetWindow(size_t index) const;
		/** get the window per name */
		AutoCastable<Window> FindWindow(ObjectRequest request);
		/** get the window per name */
		AutoConstCastable<Window> FindWindow(ObjectRequest request) const;

		/** require application end */
		virtual void Quit();

		/** create some window by its name*/
		Window* CreateNamedWindow(char const* name, CreateWindowFunc create_func);
		/** destroy some window by its name */
		void DestroyNamedWindow(char const* name);

		/** gets the GPUDevice */
		GPUDevice * GetDevice () const { return gpu_device.get(); }

		/** getter of the main clock */
		static Clock* GetMainClockInstance();
		/** getter of the main clock */
		static Clock const* GetMainClockConstInstance();

		/** getter of the sound manager */
		static SoundManager* GetSoundManagerInstance();
		/** getter of the sound manager */
		static SoundManager const* GetSoundManagerConstInstance();

		/** getter of the GPU resource manager */
		static GPUResourceManager* GetGPUResourceManagerInstance();
		/** getter of the GPU resource manager */
		static GPUResourceManager const* GetGPUResourceManagerConstInstance();

		/** gets the main clock */
		Clock* GetMainClock() { return main_clock.get(); }
		/** gets the main clock */
		Clock const* GetMainClock() const { return main_clock.get(); }

		/** gets the sound manager */
		SoundManager* GetSoundManager() { return sound_manager.get(); }
		/** gets the sound manager */
		SoundManager const* GetSoundManager() const { return sound_manager.get(); }

		/** gets the graphic resource manager */
		GPUResourceManager* GetGPUResourceManager() { return gpu_resource_manager.get(); }
		/** gets the graphic resource manager */
		GPUResourceManager const* GetGPUResourceManager() const { return gpu_resource_manager.get(); }







		// shuyyy


		/** getter on the texture atlas */
		GPUAtlas* GetTextureAtlas() { return texture_atlas.get(); }
		/** getter on the texture atlas */
		GPUAtlas const* GetTextureAtlas() const { return texture_atlas.get(); }






		// shuyyy


		/** getter on the text generator */
		ParticleTextGenerator::Generator* GetTextGenerator() { return particle_text_generator.get(); }
		/** getter on the text generator */
		ParticleTextGenerator::Generator const* GetTextGenerator() const { return particle_text_generator.get(); }



		// shuyyy

		/** get the button map */
		std::map<GamepadButton, std::pair<std::string, std::string>>& GetGamepadButtonMap() { return gamepad_button_map; }
		/** get the button map */
		std::map<GamepadButton, std::pair<std::string, std::string>> const& GetGamepadButtonMap() const { return gamepad_button_map; }








		/** used to force for one frame the duration of tick function to 0 : usefull for function that are long and would block the game for some time */
		void FreezeNextFrameTickDuration();

		/** reload all GPU resources */
		virtual bool ReloadGPUResources();
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const& key_event) override;

		/** get the OpenGL context, call the function, restore previous context after */
		template<typename FUNC>
		static decltype(auto) WithGLFWContext(GLFWwindow* glfw_context, FUNC const& func)
		{
			using L = meta::LambdaInfo<FUNC>;

			GLFWwindow* previous_glfw_context = glfwGetCurrentContext();
			glfwMakeContextCurrent(glfw_context);

			if constexpr (std::is_same_v<void, typename L::result_type>)
			{
				func();
				glfwMakeContextCurrent(previous_glfw_context);
			}
			else
			{
				decltype(auto) result = func();
				glfwMakeContextCurrent(previous_glfw_context);
				return result;
			}
		}

		/** get the OpenGL main context */
		static GLFWwindow* GetSharedGLContext();

		/** Run the message loop while the condition is true */
		virtual void RunMessageLoop(LightweightFunction<bool()> loop_condition_func = {});

		/** create a window */
		Window* CreateTypedWindow(CreateWindowFunc create_func, WindowPlacementInfo placement_info = {}, WindowCreateParams const& create_params = {}, ObjectRequest = {});

		/** enable or disable ImGui menu */
		static void SetImGuiMenuEnabled(bool enabled);
		/** gets the whether the menu is enabled */
		static bool IsImGuiMenuEnabled();

		/** create a window from its name */
		bool CreateOrDestroyKnownWindow(char const* name, bool create);

		/** destroy all windows */
		void DestroyAllWindows();

		/** search whether there is at least one main window */
		bool HasMainWindow() const;

		/** gets the imgui manager */
		ImGuiManager* GetImGuiManager() const;

		/** enumerate all windows */
		template<typename FUNC>
		decltype(auto) ForAllWindows(FUNC func)
		{
			using L = meta::LambdaInfo<FUNC, Window*>;

			// enumerate all windows
			for (weak_ptr<Window>& window : GetWeakWindowArray()) // use a 'weak' copy of all existing windows because some windows may be erased during the loop
			{
				if (window != nullptr) // check whether the window has be erased
				{
					if constexpr (L::convertible_to_bool)
					{
						if (decltype(auto) result = func(window.get()))
							return result;
					}
					else
					{
						func(window.get());
					}
				}
			}

			// default result
			if constexpr (L::convertible_to_bool)
				return typename L::result_type{};
		}

		/** create or destroy a window */
		void CreateOrDestroyWindow(bool create, char const* name, CreateWindowFunc create_func);

		/** get the current input mode (search the application) */
		static InputMode GetApplicationInputMode();
		/** change the current input mode (search the application) */
		static void SetApplicationInputMode(InputMode new_mode);

	protected:

		/** Main method */
		virtual int Main() override;
		/** The main body method */
		virtual int MainBody();

		/** create an invisible window to share its context with all other windows */
		bool CreateSharedContext(JSONReadConfiguration config);
		/** destroy the shared window */
		void FinalizeSharedContext();

		/** called whenever a monitor is connected or disconnected */
		virtual void OnMonitorEvent(GLFWmonitor* monitor, int monitor_state);
		/** called whenever a monitor is connected or disconnected */
		static void DoOnMonitorEvent(GLFWmonitor* monitor, int monitor_state);

		/** enumerate windows that the application can open */
		virtual bool EnumerateKnownWindows(EnumerateKnownWindowsFunc func) const;

		/** override */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;

		/** an error callback */
		static void OnGLFWError(int code, const char* msg);
		/** a debugging function to output some message from FreeImage */
		static void FreeImageOutputMessageFunc(FREE_IMAGE_FORMAT fif, const char* msg);
		/** override */
		virtual bool InitializeStandardLibraries() override;
		/** override */
		virtual void FinalizeStandardLibraries() override;
		/** override */
		virtual bool Initialize(JSONReadConfiguration config) override;
		/** override */
		virtual bool OnInitialize(JSONReadConfiguration config) override;
		/** override */
		virtual void Finalize() override;
		/** override */
		virtual bool InitializeManagers(JSONReadConfiguration config) override;
		/** override */
		virtual void FinalizeManagers() override;
		/** initialize the GPU resources */
		virtual bool InitializeGPUResources(JSONReadConfiguration config);
		/** finalize the GPU resources */
		virtual void FinalizeGPUResources();



		/** initialize a mapping with button names / text generator joker */
		virtual bool InitializeGamepadButtonMap(JSONReadConfiguration config);


		/** create the texture atlas */
		virtual bool CreateTextureAtlas(JSONReadConfiguration config);
		/** create the text generator */
		virtual bool CreateTextGenerator(JSONReadConfiguration config);

		/** main method to generate atlas entries */
		virtual bool FillAtlasGeneratorInput(AtlasInput& input);
		/** generate atlas entries relative to sprite directory */
		virtual bool FillAtlasGeneratorInputSprites(AtlasInput& input);
		/** generate atlas entries relative to fonts */
		virtual bool FillAtlasGeneratorInputFonts(AtlasInput& input);






		/** restore all tool windows that were opened at the end of previous session */
		void RestorePreviousSessionKnownWindows();






		/** override */
		virtual bool DoTick(float delta_time) override;

		/** override */
		virtual bool OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context) override;
		/** override */
		virtual bool OnStorePersistentProperties(JSONWriteConfiguration config) const override;

		/** the user callback called when current input mode changes */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode) override;

		/** called at window creation */
		virtual void OnWindowCreated(Window* window);
		/** called at window destruction */
		virtual void OnWindowDestroyed(Window* window);

		/** add some items to a windows menu */
		virtual void OnDrawApplicationImGuiMenu(Window * window, BeginImGuiMenuFunc begin_menu_func);

		/** some window may be removed or created during a loop on 'windows' array itself. this intermediate array helps to detect such issues */
		std::vector<weak_ptr<Window>> GetWeakWindowArray() const;

#ifdef _WIN32
		/** called whenever the application language is being changed */
		virtual void OnInputLanguageChanged();
#endif

	protected:

		/** the GPU device */
		shared_ptr<GPUDevice> gpu_device;
		/** the graphic resource manager */
		shared_ptr<GPUResourceManager> gpu_resource_manager;
		/** the sound manager */
		shared_ptr<SoundManager> sound_manager;
		/** the imgui manager */
		shared_ptr<ImGuiManager> imgui_manager;
		/** the main clock of the manager */
		shared_ptr<Clock> main_clock;

		/** the texture atlas */
		shared_ptr<GPUAtlas> texture_atlas;
		/** the text generator */
		shared_ptr<ParticleTextGenerator::Generator> particle_text_generator;



		// shuyyy

		/** a mapping between the button index and its resource name + text generator alias */
		std::map<GamepadButton, std::pair<std::string, std::string>> gamepad_button_map;












		// shuyyy


		/** the window list */
		std::vector<shared_ptr<Window>> windows;

		/** forced time slice for tick */
		float forced_tick_duration = 0.0f;
		/** maximum time slice for tick */
		float max_tick_duration = 0.0f;
		/** whether the delta time is forced to 0 for one frame (usefull for long operations like screen capture or GPU resource reloading) */
		bool forced_zero_tick_duration = false;

		/** the imgui menu mode */
		bool imgui_menu_enabled = false;

		/** the hints for all windows */
		GLFWHints glfw_hints;

		/** an invisible window that is used as a OpenGL context for all others */
		GLFWwindow* shared_context = nullptr;

		/** indicates whenever the application is being quit (closing windows in cascade may cause several Quit() calls */
		bool is_quitting = false;
	};

#endif

}; // namespace chaos