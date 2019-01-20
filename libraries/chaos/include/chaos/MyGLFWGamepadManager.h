#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{
	namespace MyGLFW
	{
		/**
		* Some constants
		*/

		/** button status change */
		static int const BUTTON_STAY_RELEASED = 1;
		/** button status change */
		static int const BUTTON_STAY_PRESSED = 2;
		/** button status change */
		static int const BUTTON_BECOME_RELEASED = 3;
		/** button status change */
		static int const BUTTON_BECOME_PRESSED = 4;

		/** index in buttons of A for XBOX like pad */
		static int const XBOX_BUTTON_A = 0;
		/** index in buttons of B for XBOX like pad */
		static int const XBOX_BUTTON_B = 1;
		/** index in buttons of X for XBOX like pad */
		static int const XBOX_BUTTON_X = 2;
		/** index in buttons of Y for XBOX like pad */
		static int const XBOX_BUTTON_Y = 3;
		/** index in buttons of LEFT for XBOX like pad (the one behind the LEFT TRIGGER) */
		static int const XBOX_BUTTON_LEFTBUT = 4;
		/** index in buttons of RIGHT for XBOX like pad (the one behind the RIGHT TRIGGER) */
		static int const XBOX_BUTTON_RIGHTBUT = 5;
		/** index in buttons of SELECT for XBOX like pad */
		static int const XBOX_BUTTON_SELECT = 6;
		/** index in buttons of START for XBOX like pad */
		static int const XBOX_BUTTON_START = 7;
		/** index in buttons of LEFT-STICK-CLICKED for XBOX like pad */
		static int const XBOX_BUTTON_LEFTSTICK = 8;
		/** index in buttons of RIGHT-STICK-CLICKED for XBOX like pad */
		static int const XBOX_BUTTON_RIGHTSTICK = 9;
		/** index in buttons of UP for XBOX like pad */
		static int const XBOX_BUTTON_UP = 10;
		/** index in buttons of DOWN for XBOX like pad */
		static int const XBOX_BUTTON_DOWN = 12;
		/** index in buttons of LEFT for XBOX like pad */
		static int const XBOX_BUTTON_LEFT = 13;
		/** index in buttons of RIGHT for XBOX like pad */
		static int const XBOX_BUTTON_RIGHT = 11;


		/** index in buttons of LEFT TRIGGER for XBOX like pad (this is a simulate button, while the physical left trigger is an axis) */
		static int const XBOX_BUTTON_LEFTTRIGGER = 101;
		/** index in buttons of RIGHT TRIGGER for XBOX like pad (this is a simulate button, while the physical right trigger is an axis) */
		static int const XBOX_BUTTON_RIGHTTRIGGER = 102;


		/** index in axis of LEFT X for XBOX like pad */
		static int const XBOX_LEFT_AXIS_X = 0;
		/** index in axis of LEFT Y for XBOX like pad */
		static int const XBOX_LEFT_AXIS_Y = 1; // STICK DOWN = positive values
		/** index in axis for the trigger for XBOX like pad */
		static int const XBOX_TRIGGER = 2; // LEFT TRIGGER = positive values,  RIGHT TRIGGER = negative values
		/** index in axis of RIGHT Y for XBOX like pad */
		static int const XBOX_RIGHT_AXIS_Y = 3;  // STICK DOWN = positive values
		/** index in axis of RIGHT X for XBOX like pad */
		static int const XBOX_RIGHT_AXIS_X = 4;

		/** returns the direction of left stick (beware the low level interface only knows for axis) */
		static int const XBOX_LEFT_AXIS = 0;
		/** returns the direction of right stick (beware the low level interface only knows for axis) */
		static int const XBOX_RIGHT_AXIS = 1;

		/** maximum number of supported physical gamepads */
		static int const MAX_SUPPORTED_GAMEPAD_COUNT = GLFW_JOYSTICK_LAST + 1;

		/**
		* GamepadCallbacks : some callbacks that may be plugged into a gamepad
		*/

		class GamepadCallbacks : public ReferencedObject
		{
		public:

			/** destructor */
			virtual ~GamepadCallbacks() = default;

			/** called to determin if a physical gamepad is a good candidate for binding */
			virtual bool AcceptPhysicalDevice(class PhysicalGamepad *) { return true; }
			/** called whenever a gamepad is disconnected */
			virtual bool OnGamepadDisconnected(class Gamepad *) { return true; }
			/** called whenever a gamepad is "connected" (a new ID is given to it) */
			virtual bool OnGamepadConnected(class Gamepad *) { return true; }
			/** called whenever the manager is destroyed before the gamepad */
			virtual bool OnManagerDestroyed(class Gamepad *) { return true; }
			/** called to add some filters on inputs */
			virtual void OnGamepadDataUpdated(class GamepadData &) {}
		};

		/**
		* AxisData : while max and min values for sticks are not always 1 (some XBOX has value lesser that 1.0),
		*            we have to store the upper and lower values to renormalize the output
		*/
		class AxisData
		{
		public:

			/** update the value */
			void UpdateValue(float in_raw_value, float dead_zone);
			/** get the value */
			inline float GetValue() const { return final_value; }
			/** clear the input */
			inline void Clear() { raw_value = final_value = 0.0f; }

		protected:

			/** value of the stick (with no filter) */
			float raw_value = 0.0f;
			/** min value always encountered */
			float min_value = -0.8f;
			/** max value always encountered */
			float max_value = +0.8f;
			/** the final value of the stick after computation */
			float final_value = 0.0f;
		};

		/**
		* GamepadData : the data contained in the device
		*/

		class GamepadData
		{
			friend class PhysicalGamepad;

		public:

			/* returns a status giving the change of button relative to previous frame */
			int GetButtonChanges(size_t button_index) const;
			/** returns the button state */
			bool IsButtonPressed(size_t button_index, bool previous_frame = false) const;
			/** returns the button state */
			float GetAxisValue(size_t axis_index, bool previous_frame = false) const;
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

			/** check whether a button is pressed. Reset it state to 0 */
			bool IsButtonPressedAndConsume(size_t button_index, bool clear_state);
			/** clear the state of a button */
			void ClearButtonState(size_t button_index);

		protected:

			/** update all the values for the axis and buttons */
			void UpdateAxisAndButtons(int stick_index, float delta_time, float dead_zone);
			/** reset the content of the object */
			void Clear(bool only_set_to_zero = false);

		protected:

			/** the value for axis */
			std::vector<AxisData> axis;
			/** the value for buttons */
			std::vector<int> buttons;
		};

		/**
		* PhysicalGamepad : the physical device. Client do not directly use it
		*/
		class PhysicalGamepad
		{
			friend class GamepadManager;
			friend class Gamepad;

		public:

			/** get a reference on the data */
			GamepadData const * GetGamepadData() const { return &gamepad_data; }

			/* returns a status giving the change of button relative to previous frame */
			int GetButtonChanges(size_t button_index) const;
			/** returns the button state */
			bool IsButtonPressed(size_t button_index, bool previous_frame = false) const;
			/** returns the button state */
			float GetAxisValue(size_t axis_index, bool previous_frame = false) const;
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
			inline int GetGamepadIndex() const { return stick_index; }
			/** returns whether the gamepad is allocated for a user */
			inline bool IsAllocated() const { return (user_gamepad != nullptr); }
			/** returns true whether the gamepad is connected */
			inline bool IsPresent() const { return is_present; }

			/** take a physical device, and create a logical device if possible */
			class Gamepad * CaptureDevice(GamepadCallbacks * in_callbacks);

		protected:

			/** the constructor is protected */
			PhysicalGamepad(GamepadManager * in_gamepad_manager, int in_stick_index);
			/** destructor is protected */
			~PhysicalGamepad(){}

			/** update all the values for the axis and buttons */
			void UpdateAxisAndButtons(float delta_time, float dead_zone);
			/** called at unconnection to be sure input cannot be consulted anymore */
			void ClearInputs();

		protected:

			/** the manager */
			class GamepadManager * gamepad_manager = nullptr;
			/** the current stick index */
			int stick_index = -1;
			/** indicates whether the stick is present */
			bool is_present = false;
			/** indicates whether the stick is allocated to a client */
			class Gamepad * user_gamepad = nullptr;
			/** the device data */
			GamepadData gamepad_data;
		};


		/**
		* Gamepad : this is a logical gamepad .. may change the physical gamepad it is bound on
		*/
		class Gamepad : public ReferencedObject
		{
		public:

			friend class GamepadManager;

		protected:

			Gamepad(GamepadManager * in_gamepad_manager, PhysicalGamepad * in_physical_device); // protected constructor

		public:

			/** destructor */
			virtual ~Gamepad();

			/** get a reference on the data (if connected) */
			GamepadData const * GetGamepadData() const;

			/* returns a status giving the change of button relative to previous frame */
			int GetButtonChanges(size_t button_index) const;
			/** returns the button state */
			bool IsButtonPressed(size_t button_index, bool previous_frame = false) const;
			/** returns the button state */
			float GetAxisValue(size_t axis_index, bool previous_frame = false) const;
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
			inline bool IsEverConnected() const { return ever_connected; }

			/** give a callback object to the gamepad */
			void SetCallbacks(GamepadCallbacks * in_callbacks);

		protected:

			/** the manager */
			class GamepadManager * gamepad_manager = nullptr;
			/* the device */
			PhysicalGamepad * physical_device = nullptr;
			/** the callbacks */
			shared_ptr<GamepadCallbacks> callbacks;
			/** indicates whether the stick has already be connected to a physical device */
			bool ever_connected = false;
		};

		/**
		* GamepadManager : used to handle gamepads, there allocation, the dynamic change of their index ...
		*/

		class GamepadManager : public ReferencedObject
		{
			friend class Gamepad;
			friend class PhysicalGamepad;

		public:

			/** constructor */
			GamepadManager(float in_dead_zone = 0.2f);
			/** destructor */
			virtual ~GamepadManager();

			/** update all the joysticks */
			void Tick(float delta_time);
			/** create a gamepad */
			Gamepad * AllocateGamepad(bool want_present = false, GamepadCallbacks * in_callbacks = nullptr);
			/** enable / disable pooling */
			void EnableInputPooling(bool in_pooling_enabled);
			/** returns true whether input pooling is enabled */
			bool IsInputPoolingEnabled() const;

			/** returns whether the given stick has any input set */
			static bool HasAnyInputs(int stick_index, float dead_zone);

		protected:

			/** find a gamepad that is used by nobody */
			PhysicalGamepad * FindUnallocatedPhysicalGamepad(GamepadCallbacks * in_callbacks);
			/** update the physical devices and detect unconnections */
			void UpdateAndUnconnectPhysicalGamepads(float delta_time, int & unallocated_present_physical_device_count);
			/** try to give a physical device to all unconnected logical device */
			void GiveGamepadPhysicalDevices(int & unallocated_present_physical_device_count);
			/** returns false if no more logical device to bound with */
			bool DoGiveGamepadPhysicalDevice(PhysicalGamepad * physical_gamepad);
			/** find the best gamepad that can be bound */
			Gamepad * FindBestGamepadToBeBoundToPhysicalDevice(PhysicalGamepad * physical_gamepad);
			/** called whenever a gamepad is destroyed */
			bool OnGamepadDestroyed(Gamepad * gamepad);
			/** called to pool inputs on unbound connected physical device */
			void PoolInputs(int & unallocated_present_physical_device_count);
			/** internal method to allocate and initialize a gamepad */
			Gamepad * DoAllocateGamepad(PhysicalGamepad * physical_gamepad, GamepadCallbacks * in_callbacks);
			/** capture a physical device and get a logical device */
			Gamepad * DoCaptureDevice(PhysicalGamepad * in_physical_gamepad, GamepadCallbacks * in_callbacks);
			
		protected:

			/** the pool method to override */
			virtual bool DoPoolGamepad(PhysicalGamepad * physical_gamepad);

		protected:

			/** the default dead zone value */
			float dead_zone = 0.2f;
			/** the logical gamepads */
			std::vector<Gamepad *> user_gamepads;
			/** the physical gamepads */
			std::vector<PhysicalGamepad *> physical_gamepads;
			/** enable pooling unused inputs */
			bool pooling_enabled = true;
		};

	}; // namespace MyGLFW

}; // namespace chaos

