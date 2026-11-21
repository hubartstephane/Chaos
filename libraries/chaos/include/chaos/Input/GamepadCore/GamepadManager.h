namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** maximum number of supported physical gamepads */
	static constexpr size_t CHAOS_SUPPORTED_GAMEPAD_COUNT = GLFW_JOYSTICK_LAST + 1;

	/**
	* GamepadManager : used to handle gamepads, there allocation, the dynamic change of their index ...
	*/

	class CHAOS_API GamepadManager : public Object, public ConfigurationUserInterface
	{
		CHAOS_GAMEPAD_ALL_FRIENDS;

#if _WIN64
		// the type of the function pointer used for forcefeedback
		typedef DWORD(*XINPUT_SET_STATE_FUNC)(DWORD, XINPUT_VIBRATION*);
#endif

	public:

		/** constructor */
		GamepadManager(GamepadInputFilterSettings const& in_gamepad_filter_settings = {});
		/** destructor */
		virtual ~GamepadManager();

		/** update all the system */
		void Tick(float delta_time, GamepadPollInterface* in_poll_target);
		/** create a gamepad */
		Gamepad* AllocateGamepad(bool want_present = false, GamepadInputFilterSettings const * in_filter_settings = nullptr, GamepadCallbacks* in_callbacks = nullptr);
		
		/** returns whether the given gamepad has any input set */
		static bool HasAnyInputs(size_t in_gamepad_index, float in_dead_zone);
		/** returns the number of physical devices */
		size_t GetPresentPhysicalGamepadCount() const;
		/** gets a physical device (beware, the gamepad might not be connected) */
		PhysicalGamepad const & GetPhysicalGamepad(size_t in_index) const { return physical_gamepads[in_index]; }
		/** iterate over all present physical gamepads */
		bool ForEachPresentPhysicalGamepad(LightweightFunction<bool(PhysicalGamepad const&)> in_func) const;

		/** return whether the manager is enabled */
		bool IsEnabled() const;
		/** change the enable state */
		void SetEnabled(bool in_enabled);
		/** enable / disable polling */
		void SetPollingEnabled(bool in_polling_enabled);
		/** returns true whether input polling is enabled */
		bool IsPollingEnabled() const;

		/** override */
		virtual bool OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context) override;

	protected:

		/** find a gamepad that is used by nobody */
		PhysicalGamepad* FindUnallocatedPhysicalDevice(GamepadCallbacks* in_callbacks);
		/** update the physical devices and detect disconnections */
		void UpdateAndHandlePhysicalDeviceDisconnection();
		/** try to give a physical device to all disconnected logical device */
		void AssociateUnallocatedPhysicalDevicesToGamepads();
		/** search and bound a gamepad for a given physical device */
		Gamepad * DoGivePhysicalDeviceToAnyGamepad(PhysicalGamepad * in_physical_gamepad);
		/** find the best gamepad that can be bound */
		Gamepad * FindBestGamepadCandidateForPhysicalDevice(PhysicalGamepad * in_physical_gamepad);
		/** remove a gamepad from manager */
		bool RemoveGamepad(Gamepad* in_gamepad);
		/** remove a gamepad from manager by its index */
		bool RemoveGamepadAt(size_t index);
		/** called to poll inputs on unbound connected physical device */
		void PollInputs(GamepadPollInterface* in_poll_target);
		/** internal method to allocate and initialize a gamepad */
		Gamepad* DoAllocateGamepad(PhysicalGamepad * in_physical_gamepad, GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks);
		/** capture a physical device and get a logical device */
		Gamepad* DoCaptureDevice(PhysicalGamepad * in_physical_gamepad, GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks);

		/** tick the force feedback effects of all allocated gamepad */
		void TickForceFeedbackEffects(float delta_time);

	protected:

		/** configuration to handle input1D & input2D */
		GamepadInputFilterSettings gamepad_filter_settings;

		/** the logical gamepads */
		std::vector<shared_ptr<Gamepad>> user_gamepads;

		/** the physical gamepads */
		std::array<PhysicalGamepad, CHAOS_SUPPORTED_GAMEPAD_COUNT> physical_gamepads;
		/** present physical gamepads */
		uint32_t present_physical_gamepads = 0;
		/** allocated physical gamepads */
		uint32_t allocated_physical_gamepads = 0;
				
		/** enable polling unused inputs */
		bool polling_enabled = true;
		/** whether the manager is enabled */
		bool enabled = true;

#if _WIN64
		/** the function pointer to use for ForceFeedback */
		static XINPUT_SET_STATE_FUNC XInputSetStateFunc;
#endif
	};


#endif

}; // namespace chaos