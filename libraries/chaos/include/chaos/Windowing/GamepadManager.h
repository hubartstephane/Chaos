namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	// ==============================================================
	// FORWARD DECLARATION / FRIENDSHIP MACROS
	// ==============================================================

	// all classes in this file
#define CHAOS_GAMEPAD_CLASSES \
(PhysicalGamepad) \
(Gamepad) \
(GamepadCallbacks) \
(GamepadManager) \
(ForceFeedbackEffect) \
(DefaultForceFeedbackEffect)

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
	static constexpr int MAX_SUPPORTED_GAMEPAD_COUNT = GLFW_JOYSTICK_LAST + 1;

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
		/** called whenever the manager is destroyed before the gamepad */
		virtual bool OnManagerDestroyed(class Gamepad*) { return true; }
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

		/** returns the stick index */
		int GetGamepadIndex() const { return stick_index; }
		/** returns whether the gamepad is allocated for a user */
		bool IsAllocated() const { return (user_gamepad != nullptr); }
		/** returns true whether the gamepad is connected */
		bool IsPresent() const { return is_present; }

		/** take a physical device, and create a logical device if possible */
		class Gamepad* CaptureDevice(GamepadCallbacks* in_callbacks);

	protected:

		/** the constructor is protected */
		PhysicalGamepad(GamepadManager* in_gamepad_manager, int in_stick_index);
		/** destructor is protected */
		~PhysicalGamepad() {}

		/** override */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const override;

		/** update all the values for the axis and buttons */
		void UpdateAxisAndButtons(float dead_zone, float max_zone);
		/** called at unconnection to be sure input cannot be consulted anymore */
		void ClearInputs();

	protected:

		/** the manager */
		class GamepadManager* gamepad_manager = nullptr;
		/** the current stick index */
		int stick_index = -1;
		/** indicates whether the stick is present */
		bool is_present = false;
		/** indicates whether the stick is allocated to a client */
		class Gamepad* user_gamepad = nullptr;
		/** the device state */
		GamepadState gamepad_state;
	};

	/**
	 * ForceFeedbackEffect : the feedback effects
	 */

	class CHAOS_API ForceFeedbackEffect : public Object
	{
		CHAOS_GAMEPAD_ALL_FRIENDS

	public:

		/** returns the values for left and right motors. returns true whether the effect is to be finished */
		virtual bool GetForceFeedbackValues(float delta_time, float& result_left_value, float& result_right_value);
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
		DefaultForceFeedbackEffect(float in_duration, float in_left_value, float in_right_value) :
			duration(in_duration),
			left_value(in_left_value),
			right_value(in_right_value) {}

		/** override */
		virtual bool GetForceFeedbackValues(float delta_time, float& result_left_value, float& result_right_value) override;

	protected:

		/** the duration of the effect */
		float duration = 0.0f;
		/** the remaining time */
		float timer = 0.0f;
		/** the left engine of the gamepad ('shocks') */
		float left_value = 1.0f;
		/** the right engine of the gamepad ('vibration') */
		float right_value = 1.0f;
	};

	/**
	* Gamepad : this is a logical gamepad .. may change the physical gamepad it is bound on
	*/
	class CHAOS_API Gamepad : public Object, public InputDeviceInterface
	{
	public:

		CHAOS_GAMEPAD_ALL_FRIENDS

	protected:

		Gamepad(GamepadManager* in_gamepad_manager, PhysicalGamepad* in_physical_device); // protected constructor

	public:

		/** destructor */
		virtual ~Gamepad();

		/** get a reference on the state (if connected) */
		GamepadState const* GetGamepadState() const;

		/** returns the stick index */
		int GetGamepadIndex() const;
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
		void RemoveForceFeedbackEffect(ForceFeedbackEffect* effect);
		/** remove force feedback effects */
		void ClearForceFeedbackEffects();

	protected:

		/** override */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const override;

		/** tick force feedback effects */
		void TickForceFeedbackEffects(float delta_time);
		/** setting the left & right values to the device */
		void DoUpdateForceFeedbackDevice(float max_left_value, float max_right_value);
		/** returns whether forcefeedback should be reduced to 0 */
		bool ShouldReduceForceFeedbackToZero() const;

	protected:

		/** the manager */
		class GamepadManager* gamepad_manager = nullptr;
		/* the device */
		PhysicalGamepad* physical_device = nullptr;
		/** the callbacks */
		shared_ptr<GamepadCallbacks> callbacks;
		/** indicates whether the stick has already be connected to a physical device */
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
	* GamepadManager : used to handle gamepads, there allocation, the dynamic change of their index ...
	*/

	class CHAOS_API GamepadManager : public Object
	{
		CHAOS_GAMEPAD_ALL_FRIENDS;

#if _WIN64
		// the type of the function pointer used for forcefeedback
		typedef DWORD(*XINPUT_SET_STATE_FUNC)(DWORD, XINPUT_VIBRATION*);
#endif

	public:

		/** constructor */
		GamepadManager(float in_dead_zone = 0.4f, float in_max_zone = 0.9f);
		/** destructor */
		virtual ~GamepadManager();

		/** update all the joysticks */
		void Tick(float delta_time);
		/** create a gamepad */
		Gamepad* AllocateGamepad(bool want_present = false, GamepadCallbacks* in_callbacks = nullptr);
		/** enable / disable pooling */
		void EnableInputPooling(bool in_pooling_enabled);
		/** returns true whether input pooling is enabled */
		bool IsInputPoolingEnabled() const;

		/** returns whether the given stick has any input set */
		static bool HasAnyInputs(int stick_index, float dead_zone);

		size_t GetPhysicalGamepadCount() const { return physical_gamepads.size(); }

		PhysicalGamepad const * GetPhysicalGamepad(size_t index) const { return physical_gamepads[index]; }

	protected:

		/** find a gamepad that is used by nobody */
		PhysicalGamepad* FindUnallocatedPhysicalGamepad(GamepadCallbacks* in_callbacks);
		/** update the physical devices and detect unconnections */
		void UpdateAndUnconnectPhysicalGamepads(int& unallocated_present_physical_device_count);
		/** try to give a physical device to all unconnected logical device */
		void GiveGamepadPhysicalDevices(int& unallocated_present_physical_device_count);
		/** returns false if no more logical device to bound with */
		bool DoGiveGamepadPhysicalDevice(PhysicalGamepad* physical_gamepad);
		/** find the best gamepad that can be bound */
		Gamepad* FindBestGamepadToBeBoundToPhysicalDevice(PhysicalGamepad* physical_gamepad);
		/** called whenever a gamepad is destroyed */
		bool OnGamepadDestroyed(Gamepad* gamepad);
		/** called to pool inputs on unbound connected physical device */
		void PoolInputs(int& unallocated_present_physical_device_count);
		/** internal method to allocate and initialize a gamepad */
		Gamepad* DoAllocateGamepad(PhysicalGamepad* physical_gamepad, GamepadCallbacks* in_callbacks);
		/** capture a physical device and get a logical device */
		Gamepad* DoCaptureDevice(PhysicalGamepad* in_physical_gamepad, GamepadCallbacks* in_callbacks);

		/** tick the force feedback effects of all allocated gamepad */
		void TickForceFeedbackEffects(float delta_time);

		/** the pool method to override */
		virtual bool DoPoolGamepad(PhysicalGamepad* physical_gamepad);

	protected:

		/** the default dead zone value */
		float dead_zone = 0.4f;
		/** the max zone after which an input1D or 2D is clamped to 1.0f */
		float max_zone = 0.9f;
		/** the logical gamepads */
		std::vector<Gamepad*> user_gamepads;
		/** the physical gamepads */
		std::vector<PhysicalGamepad*> physical_gamepads;
		/** enable pooling unused inputs */
		bool pooling_enabled = true;

#if _WIN64
		/** the function pointer to use for ForceFeedback */
		static XINPUT_SET_STATE_FUNC XInputSetStateFunc;
#endif
	};


#endif

}; // namespace chaos