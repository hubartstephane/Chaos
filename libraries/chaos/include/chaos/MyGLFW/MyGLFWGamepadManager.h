#ifdef CHAOS_FORWARD_DECLARATION

// ==============================================================
// FORWARD DECLARATION / FRIENDSHIP MACROS
// ==============================================================

// all classes in this file
#define CHAOS_GAMEPAD_CLASSES \
(GamepadData) \
(AxisData) \
(ButtonData) \
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

namespace chaos
{
	enum class ButtonStateChange;

	enum XBoxButton;
	enum XBoxAxis;

	namespace MyGLFW
	{
		BOOST_PP_SEQ_FOR_EACH(CHAOS_GAMEPAD_FORWARD_DECL, _, CHAOS_GAMEPAD_CLASSES);

	}; // namespace MyGLFW

}; // namespace chaos

#else

// XXX: there are important changes in GLFW 3.3 relative to 3.1
//
//  -XBOX_RIGHT_AXIS_X and XBOX_RIGHT_AXIS_Y : are bound to other indices.
//
//  -in GLFW 3.1, LEFT and RIGHT triggers were bound to the same index
//                LEFT  giving a value in [-1 .. 0] 
//                RIGHT giving a value in [0 .. +1] 
//
//  -in GLFW 3.3, LEFT are RIGHT TRIGGERS are now bound to different indices 
//                LEFT  giving a value in [-1 .. +1] 
//                RIGHT giving a value in [-1 .. +1] 
//
//                while they are not bound to the same index, you can check both values

namespace chaos
{

	enum class ButtonStateChange : int
	{
		/** button status change */
		NO_CHANGE = 0,
		/** button status change */
		STAY_RELEASED = 1,
		/** button status change */
		STAY_PRESSED = 2,
		/** button status change */
		BECOME_RELEASED = 3,
		/** button status change */
		BECOME_PRESSED = 4
	};

	// shuxxx see glfw3.h => some values seem to differ    GLFW_GAMEPAD_BUTTON_LEFT_THUMB ? etc


	enum XBoxButton : int // XXXX: no class, so this can be implicitly converted to int
	{
		/** index in buttons of A for XBOX like pad */
		BUTTON_A = 0,
		/** index in buttons of B for XBOX like pad */
		BUTTON_B = 1,
		/** index in buttons of X for XBOX like pad */
		BUTTON_X = 2,
		/** index in buttons of Y for XBOX like pad */
		BUTTON_Y = 3,
		/** index in buttons of LEFT for XBOX like pad (the one behind the LEFT TRIGGER) */
		BUTTON_LEFTBUT = 4,
		/** index in buttons of RIGHT for XBOX like pad (the one behind the RIGHT TRIGGER) */
		BUTTON_RIGHTBUT = 5,
		/** index in buttons of SELECT for XBOX like pad */
		BUTTON_SELECT = 6,
		/** index in buttons of START for XBOX like pad */
		BUTTON_START = 7,
		/** index in buttons of LEFT-STICK-CLICKED for XBOX like pad */
		BUTTON_LEFTSTICK = 8,
		/** index in buttons of RIGHT-STICK-CLICKED for XBOX like pad */
		BUTTON_RIGHTSTICK = 9,
		/** index in buttons of UP for XBOX like pad */
		BUTTON_UP = 10,
		/** index in buttons of DOWN for XBOX like pad */
		BUTTON_DOWN = 12,
		/** index in buttons of LEFT for XBOX like pad */
		BUTTON_LEFT = 13,
		/** index in buttons of RIGHT for XBOX like pad */
		BUTTON_RIGHT = 11,

		/** index in buttons of LEFT TRIGGER for XBOX like pad (this is a simulate button, while the physical left trigger is an axis) */
		BUTTON_LEFTTRIGGER = 101,
		/** index in buttons of RIGHT TRIGGER for XBOX like pad (this is a simulate button, while the physical right trigger is an axis) */
		BUTTON_RIGHTTRIGGER = 102
	};

	enum XBoxAxis : int // XXXX: no class, so this can be implicitly converted to int
	{
		/** index in axis of LEFT X for XBOX like pad */
		LEFT_AXIS_X = 0,
		/** index in axis of LEFT Y for XBOX like pad */
		LEFT_AXIS_Y = 1, // STICK DOWN = positive values

		/** index in axis of RIGHT X for XBOX like pad */
		RIGHT_AXIS_X = 2,
		/** index in axis of RIGHT Y for XBOX like pad */
		RIGHT_AXIS_Y = 3,  // STICK DOWN = positive values

		/** index in axis for the LEFT trigger for XBOX like pad (beware its value is between [-1 .. +1]) */
		LEFT_TRIGGER = 4,
		/** index in axis for the RIGHT trigger for XBOX like pad (beware its value is between [-1 .. +1]) */
		RIGHT_TRIGGER = 5,

		/** returns the direction of left stick (beware the low level interface only knows for axis) */
		LEFT_AXIS = 0,
		/** returns the direction of right stick (beware the low level interface only knows for axis) */
		RIGHT_AXIS = 1
	};

	namespace MyGLFW
	{
		/**
		* Some constants
		*/

		/** maximum number of supported physical gamepads */
		static constexpr int MAX_SUPPORTED_GAMEPAD_COUNT = GLFW_JOYSTICK_LAST + 1;

		/**
		* GamepadCallbacks : some callbacks that may be plugged into a gamepad
		*/

		class GamepadCallbacks : public Object
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
			virtual void OnGamepadDataUpdated(class GamepadData&) {}
		};

		/**
		* ButtonData
		*/

		class ButtonData
		{
			CHAOS_GAMEPAD_ALL_FRIENDS

		public:

			/** update the value */
			void UpdateValue(bool in_value);
			/** get the value */
			bool GetValue() const { return value; }
			/** get the timer for the same value */
			float GetSameValueTimer() const { return same_value_timer; }
			/** clear the input */
			void Clear();

		protected:

			/** value of the button (pressed or not) */
			bool value = false;
			/** the duration for which the value is the same */
			float same_value_timer = 0.0f;
		};

		/**
		* AxisData : while max and min values for sticks are not always 1 (some XBOX has value lesser that 1.0),
		*            we have to store the upper and lower values to renormalize the output
		*/
		class AxisData
		{
			CHAOS_GAMEPAD_ALL_FRIENDS

		public:

			/** update the value */
			void UpdateValue(float in_raw_value, float dead_zone);
			/** get the value */
			float GetValue() const { return final_value; }
			/** get the timer for the same value */
			float GetSameValueTimer() const { return same_value_timer; }
			/** clear the input */
			void Clear();

		protected:

			/** value of the stick (with no filter) */
			float raw_value = 0.0f;
			/** min value always encountered */
			float min_value = -0.8f;
			/** max value always encountered */
			float max_value = +0.8f;
			/** the final value of the stick after computation */
			float final_value = 0.0f;
			/** the duration for which the value is the same (necessary in discret mode < 0, == 0 or > 0) */
			float same_value_timer = 0.0f;
		};

		/**
		* GamepadData : the data contained in the device
		*/

		class GamepadData
		{
			CHAOS_GAMEPAD_ALL_FRIENDS

		public:

			/* returns a status giving the change of button relative to previous frame */
			ButtonStateChange GetButtonStateChange(size_t button_index) const;
			/** returns the button state */
			bool IsButtonPressed(size_t button_index, bool previous_frame = false) const;
			/** returns the button state */
			float GetAxisValue(size_t axis_index, bool previous_frame = false) const;
			/** returns true whether there is a button that just has become pressed */
			bool IsAnyButtonJustPressed() const;
			/** returns true whether there is any pressed button */
			bool IsAnyButtonPressed(bool previous_frame = false) const;
			/** returns true whether there is any axis in use */
			bool IsAnyAxisAction(bool previous_frame = false) const;
			/** returns true whenever a buttons is pressed or an axis is in action */
			bool IsAnyAction(bool previous_frame = false) const;
			/** returns the direction of one stick (a combinaison of 2 axis) */
			glm::vec2 GetXBOXStickDirection(int stick_number, bool previous_frame = false) const;
			/** returns the number of buttons */
			size_t GetButtonCount() const;
			/** returns the number of axis */
			size_t GetAxisCount() const;

		protected:

			/** update all the values for the axis and buttons */
			void UpdateAxisAndButtons(int stick_index, float delta_time, float dead_zone);
			/** reset the content of the object */
			void Clear(bool only_set_to_zero = false);

		protected:

			/** the value for axis */
			std::vector<AxisData> axis;
			/** the value for buttons */
			std::vector<ButtonData> buttons;
		};

		/**
		* PhysicalGamepad : the physical device. Client do not directly use it
		*/
		class PhysicalGamepad
		{
			CHAOS_GAMEPAD_ALL_FRIENDS

		public:

			/** get a reference on the data */
			GamepadData const* GetGamepadData() const { return &gamepad_data; }

			/* returns a status giving the change of button relative to previous frame */
			ButtonStateChange GetButtonStateChange(size_t button_index) const;
			/** returns the button state */
			bool IsButtonPressed(size_t button_index, bool previous_frame = false) const;
			/** returns the button state */
			float GetAxisValue(size_t axis_index, bool previous_frame = false) const;
			/** returns true whether there is a button that just has become pressed */
			bool IsAnyButtonJustPressed() const;
			/** returns true whether there is any pressed button */
			bool IsAnyButtonPressed(bool previous_frame = false) const;
			/** returns true whether there is any axis in use */
			bool IsAnyAxisAction(bool previous_frame = false) const;
			/** returns true whenever a buttons is pressed or an axis is in action */
			bool IsAnyAction(bool previous_frame = false) const;
			/** returns the direction of one stick (a combinaison of 2 axis) */
			glm::vec2 GetXBOXStickDirection(int stick_number, bool previous_frame = false) const;

			/** returns the number of buttons */
			size_t GetButtonCount() const;
			/** returns the number of axis */
			size_t GetAxisCount() const;

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

			/** update all the values for the axis and buttons */
			void UpdateAxisAndButtons(float delta_time, float dead_zone);
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
			/** the device data */
			GamepadData gamepad_data;
		};

		/**
		 * ForceFeedbackEffect : the feedback effects
		 */

		class ForceFeedbackEffect : public Object
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
			Gamepad * gamepad = nullptr;
		};

		/**
		 * DefaultForceFeedbackEffect : a simple constant effect with a timer
		 */

		class DefaultForceFeedbackEffect : public ForceFeedbackEffect
		{
			CHAOS_GAMEPAD_ALL_FRIENDS

		public:

			/** constructor */
			DefaultForceFeedbackEffect(float in_duration, float in_left_value, float in_right_value) :
				duration(in_duration),
				left_value(in_left_value),
				right_value(in_right_value){}

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
		class Gamepad : public Object
		{
		public:

			CHAOS_GAMEPAD_ALL_FRIENDS

		protected:

			Gamepad(GamepadManager* in_gamepad_manager, PhysicalGamepad* in_physical_device); // protected constructor

		public:

			/** destructor */
			virtual ~Gamepad();

			/** get a reference on the data (if connected) */
			GamepadData const* GetGamepadData() const;

			/* returns a status giving the change of button relative to previous frame */
			ButtonStateChange GetButtonStateChange(size_t button_index) const;
			/** returns the button state */
			bool IsButtonPressed(size_t button_index, bool previous_frame = false) const;
			/** returns the button state */
			float GetAxisValue(size_t axis_index, bool previous_frame = false) const;
			/** returns true whether there is a button that just has become pressed */
			bool IsAnyButtonJustPressed() const;
			/** returns true whether there is any pressed button */
			bool IsAnyButtonPressed(bool previous_frame = false) const;
			/** returns true whether there is any axis in use */
			bool IsAnyAxisAction(bool previous_frame = false) const;
			/** returns true whenever a buttons is pressed or an axis is in action */
			bool IsAnyAction(bool previous_frame = false) const;
			/** returns the direction of one stick (a combinaison of 2 axis) */
			glm::vec2 GetXBOXStickDirection(int stick_number, bool previous_frame = false) const;
			/** returns the number of buttons */
			size_t GetButtonCount() const;
			/** returns the number of axis */
			size_t GetAxisCount() const;

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

		class GamepadManager : public Object
		{
			CHAOS_GAMEPAD_ALL_FRIENDS;

#if _WIN64
			// the type of the function pointer used for forcefeedback
			typedef DWORD(*XINPUT_SET_STATE_FUNC)(DWORD, XINPUT_VIBRATION*);
#endif

		public:

			/** constructor */
			GamepadManager(float in_dead_zone = 0.2f);
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

		protected:

			/** find a gamepad that is used by nobody */
			PhysicalGamepad* FindUnallocatedPhysicalGamepad(GamepadCallbacks* in_callbacks);
			/** update the physical devices and detect unconnections */
			void UpdateAndUnconnectPhysicalGamepads(float delta_time, int& unallocated_present_physical_device_count);
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

		protected:

			/** the pool method to override */
			virtual bool DoPoolGamepad(PhysicalGamepad* physical_gamepad);

		protected:

			/** the default dead zone value */
			float dead_zone = 0.4f;
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

	}; // namespace MyGLFW

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

