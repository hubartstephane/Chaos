namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	// ==============================================================
	// FORWARD DECLARATION / FRIENDSHIP MACROS
	// ==============================================================

class GamepadInputFilterSettings;

	// all classes in this file
#define CHAOS_GAMEPAD_CLASSES \
(PhysicalGamepad) \
(Gamepad) \
(GamepadCallbacks) \
(GamepadManager) \
(ForceFeedbackMotorValues) \
(ForceFeedbackEffect) \
(DefaultForceFeedbackEffect)\
(GamepadPollInterface)

// forward declaration
#define CHAOS_GAMEPAD_FORWARD_DECL(r, data, elem) class elem;

// friendship macro
#define CHAOS_GAMEPAD_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_GAMEPAD_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_GAMEPAD_FRIEND_DECL, _, CHAOS_GAMEPAD_CLASSES)

BOOST_PP_SEQ_FOR_EACH(CHAOS_GAMEPAD_FORWARD_DECL, _, CHAOS_GAMEPAD_CLASSES);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Some constants
	*/

	/** maximum number of supported physical gamepads */
	static constexpr size_t MAX_SUPPORTED_GAMEPAD_COUNT = GLFW_JOYSTICK_LAST + 1;

	/**
	* GamepadInputFilterSettings: some configuration describing how to handle gamepad input
	*/

	class GamepadInputFilterSettings
	{
	public:

		/** how axis and sticks are interpolated */
		InterpolationType length_interpolation_type = InterpolationType::LINEAR;
		/** dead_zone applied to axes and sticks (using length for input2D). If value lower than this, consider it as 0 */
		float dead_zone = 0.2f;
		/** max_zone applied to axes and sticks (using length for input2D). If value greater than this, consider it as 1 */
		float max_zone  = 0.9f;
		/** factor of snapping. A factor of 0 mean no snapping. 1 full snapping */
		float sector_snap_angle_ratio = 0.3f;
		/** number of sector for angle snapping */
		int sector_snap_count = 4;
	};

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, GamepadInputFilterSettings const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, GamepadInputFilterSettings& dst);

	/**
	* GamepadCallbacks : some callbacks that may be plugged into a gamepad
	*/

	class CHAOS_API GamepadCallbacks : public Object
	{
		CHAOS_GAMEPAD_ALL_FRIENDS

	public:

		/** destructor */
		virtual ~GamepadCallbacks() = default;

		/** called to determin if a physical gamepad is a good candidate for binding */
		virtual bool AcceptPhysicalDevice(class PhysicalGamepad*) { return true; }
		/** called whenever a gamepad is disconnected */
		virtual bool OnGamepadDisconnected(class Gamepad*) { return true; }
		/** called whenever a gamepad is "connected" (a new ID is given to it) */
		virtual bool OnGamepadConnected(class Gamepad*) { return true; }
		/** called to add some filters on inputs */
		virtual void OnGamepadStateUpdated(class GamepadState&) {}
	};

	/**
	* PhysicalGamepad : the physical device. Client do not directly use it
	*/
	class CHAOS_API PhysicalGamepad : public InputDeviceInterface
	{
		CHAOS_GAMEPAD_ALL_FRIENDS

	public:

		/** get a reference on the state */
		GamepadState const* GetGamepadState() const { return &gamepad_state; }

		/** returns the gamepad index */
		size_t GetGamepadIndex() const { return gamepad_index; }
		/** returns whether the gamepad is allocated for a user */
		bool IsAllocated() const { return (user_gamepad != nullptr); }
		/** returns true whether the gamepad is connected */
		bool IsPresent() const { return is_present; }

		/** take a physical device, and create a logical device if possible */
		class Gamepad* CaptureDevice(GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks);

	protected:

		/** override */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const override;

		/** update all the values for the axis and buttons */
		void UpdateAxisAndButtons(GamepadInputFilterSettings const & in_filter_settings);
		/** called at disconnection to be sure input cannot be consulted anymore */
		void ClearInputs();

	protected:

		/** the manager */
		class GamepadManager* gamepad_manager = nullptr;
		/** the current gamepad index */
		size_t gamepad_index = 0;
		/** indicates whether the gamepad is present */
		bool is_present = false;
		/** indicates whether the gamepad is allocated to a client */
		class Gamepad* user_gamepad = nullptr;
		/** the device state */
		GamepadState gamepad_state;
	};

	/**
	 * ForceFeedbackMotorValues: get the motor values for forcefeedback effects
	 */

	class CHAOS_API ForceFeedbackMotorValues
	{
	public:

		/** the left engine of the gamepad ('shocks') */
		float left_value = 1.0f;
		/** the right engine of the gamepad ('vibration') */
		float right_value = 1.0f;
	};

	/**
	 * ForceFeedbackEffect : the feedback effects
	 */

	class CHAOS_API ForceFeedbackEffect : public Object
	{
		CHAOS_GAMEPAD_ALL_FRIENDS

	public:

		/** returns the values for left and right motors. returns true whether the effect is to be finished */
		virtual bool TickAndGetMotorValues(float delta_time, ForceFeedbackMotorValues& out_motor_values);
		/** remove the forcefeedback from its gamepad */
		void RemoveFromGamepad();
		/** the force feedbaack effect may deleted itself as soon as a reference is removed and the last one is from the gamepad */
		virtual void SubReference() override;

	protected:

		/** the gamepad using this effect */
		Gamepad* gamepad = nullptr;
	};

	/**
	 * DefaultForceFeedbackEffect : a simple constant effect with a timer
	 */

	class CHAOS_API DefaultForceFeedbackEffect : public ForceFeedbackEffect
	{
		CHAOS_GAMEPAD_ALL_FRIENDS

	public:

		/** constructor */
		DefaultForceFeedbackEffect(float in_duration, ForceFeedbackMotorValues const & in_motor_values):
			duration(in_duration),
			motor_values(in_motor_values){}

		/** override */
		virtual bool TickAndGetMotorValues(float delta_time, ForceFeedbackMotorValues & out_motor_values) override;

	protected:

		/** the duration of the effect */
		float duration = 0.0f;
		/** the remaining time */
		float timer = 0.0f;
		/** the motor values */
		ForceFeedbackMotorValues motor_values;
	};

	/**
	* Gamepad : this is a logical gamepad .. may change the physical gamepad it is bound on
	*/
	class CHAOS_API Gamepad : public Object, public InputDeviceInterface
	{
	public:

		CHAOS_GAMEPAD_ALL_FRIENDS

	protected:

		/** constructor */
		Gamepad(GamepadManager* in_gamepad_manager, PhysicalGamepad* in_physical_gamepad, GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks);

	public:

		/** get a reference on the state (if connected) */
		GamepadState const* GetGamepadState() const;

		/** returns the gamepad index */
		size_t GetGamepadIndex() const;
		/** returns true whether the gamepad is connected */
		bool IsPresent() const;
		/** returns whether the gamepad has already been connected once */
		bool IsEverConnected() const { return ever_connected; }

		/** give a callback object to the gamepad */
		void SetCallbacks(GamepadCallbacks* in_callbacks);

		/** returns whether the force feedback is enabled */
		bool IsForceFeedbackEnabled() const { return force_feedback_enabled; }
		/** set whether the force feedback is enabled */
		void SetForceFeedbackEnabled(bool in_enabled);

		/** returns whether the force feedback is muted */
		bool IsForceFeedbackMuted() const { return force_feedback_muted; }
		/** set whether the force feedback is muted */
		void SetForceFeedbackMuted(bool in_muted);

		/** returns whether the force feedback is paused */
		bool IsForceFeedbackPaused() const { return force_feedback_paused; }
		/** set whether the force feedback is paused */
		void SetForceFeedbackPaused(bool in_paused);

		/** add a force feedback effect */
		void AddForceFeedbackEffect(ForceFeedbackEffect* effect);
		/** remove a forcefeedback effect */
		bool RemoveForceFeedbackEffect(ForceFeedbackEffect* effect);
		/** remove force feedback effects */
		void ClearForceFeedbackEffects();

		/** clear all inputs of the gamepad */
		void ClearInputs();

		/** get the filter settings */
		GamepadInputFilterSettings const& GetInputFilterSettings() const;
		/** set the filter settings */
		void SetInputFilterSettings(GamepadInputFilterSettings const& in_filter_settings);

		/** override */
		virtual void SubReference() override;

	protected:

		/** override */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const override;

		/** tick force feedback effects */
		void TickForceFeedbackEffects(float delta_time);
		/** setting the left & right values to the device */
		void DoUpdateForceFeedbackDevice(ForceFeedbackMotorValues in_motor_values);
		/** returns whether forcefeedback should be reduced to 0 */
		bool ShouldReduceForceFeedbackToZero() const;

		/** remove an effect at a given index */
		bool RemoveForceFeedbackAt(size_t index);

	protected:

		/** the manager */
		class GamepadManager* gamepad_manager = nullptr;
		/* the device */
		PhysicalGamepad* physical_gamepad = nullptr;
		/** configuration to handle input1D & input2D */
		GamepadInputFilterSettings filter_settings;
		/** the callbacks */
		shared_ptr<GamepadCallbacks> callbacks;
		/** indicates whether the gamepad has already be connected to a physical device */
		bool ever_connected = false;
		/** indicates whether the force feedback is enabled */
		bool force_feedback_enabled = true;
		/** indicates whether the force feedback is muted */
		bool force_feedback_muted = false;
		/** indicates whether the force feedback must be ticked or not */
		bool force_feedback_paused = false;

		/** the forcefeedback effects */
		std::vector<shared_ptr<ForceFeedbackEffect>> feedback_effects;
	};

	/**
	* GamepadPollInterface: an interface for objects that want to poll gamepad states
	*/

	class GamepadPollInterface
	{
		CHAOS_GAMEPAD_ALL_FRIENDS;

	public:

		/** called whenever a physical state is to be polled */
		virtual bool DoPollGamepad(PhysicalGamepad * physical_gamepad);
	};

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
		size_t GetPhysicalGamepadCount() const;
		/** gets a physical device */
		PhysicalGamepad const & GetPhysicalGamepad(size_t in_index) const { return physical_gamepads[in_index]; }

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
		std::array<PhysicalGamepad, MAX_SUPPORTED_GAMEPAD_COUNT> physical_gamepads;
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