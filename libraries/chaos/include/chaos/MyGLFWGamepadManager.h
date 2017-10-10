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
    static int const BUTTON_STAY_RELEASED = 0;
    /** button status change */
    static int const BUTTON_STAY_PRESSED = 1;
    /** button status change */
    static int const BUTTON_BECOME_RELEASED = 2;
    /** button status change */
    static int const BUTTON_BECOME_PRESSED = 3;

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

      /** callback whenever a gamepad is disconnected */
      virtual bool OnGamepadDisconnected(class Gamepad *) { return true; }
      /** callback whenever a gamepad is "connected" (a new ID is given to it) */
      virtual bool OnGamepadConnected(class Gamepad *) { return true; }
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
    * PhysicalGamepad : the physical device. Client do not directly use it
    */
    class PhysicalGamepad
    {
    public:

      friend class GamepadManager;
      friend class Gamepad;

    protected:

      /** the constructor is protected */
      PhysicalGamepad(int in_stick_index) : stick_index(in_stick_index) {}
      /** update all the values for the axis and buttons */
      void UpdateAxisAndButtons(float delta_time, float dead_zone);

      /* returns a status giving the change of button relative to previous frame */
      int GetButtonChanges(size_t button_index) const;
      /** returns the button state */
      bool IsButtonPressed(size_t button_index, bool previous_frame = false) const;
      /** returns the button state */
      float GetAxisValue(size_t axis_index, bool previous_frame = false) const;
      /** returns the stick index */
      int GetGamepadIndex() const { return stick_index; }
      /** returns true whether there is any pressed button */
      bool IsAnyButtonPressed(bool previous_frame = false) const;
      /** returns true whether there is any axis in use */
      bool IsAnyAxisAction(bool previous_frame = false) const;
      /** returns true whenever a buttons is pressed or an axis is in action */
      bool IsAnyAction(bool previous_frame = false) const;
      /** returns the direction of one stick (a combinaison of 2 axis) */
      glm::vec2 GetXBOXStickDirection(int stick_index, bool previous_frame = false) const;

      /** returns the number of buttons */
      size_t GetButtonCount() const;
      /** returns the number of axis */
      size_t GetAxisCount() const;

      /** returns whether the gamepad has already been connected once */
      inline bool IsEverConnected() const { return ever_connected; }
      /** returns whether the gamepad is allocated for a user */
      inline bool IsAllocated() const { return (user_gamepad != nullptr); }
      /** returns true whether the gamepad is connected */
      inline bool IsPresent() const { return is_present; }

      /** called at unconnection to be sure input cannot be consulted anymore */
      void ClearInputs();

    protected:

      /** the current stick index */
      int stick_index = -1;
      /** indicates whether the stick is present */
      bool is_present = false;
      /** indicates whether the stick has already be connected */
      bool ever_connected = false;
      /** indicates whether the stick is allocated to a client */
      class Gamepad * user_gamepad = nullptr;

      /** the value for axis */
      std::vector<AxisData> axis;
      /** the value for buttons */
      std::vector<int> buttons;
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

      /* returns a status giving the change of button relative to previous frame */
      int GetButtonChanges(size_t button_index) const;
      /** returns the button state */
      bool IsButtonPressed(size_t button_index, bool previous_frame = false) const;
      /** returns the button state */
      float GetAxisValue(size_t axis_index, bool previous_frame = false) const;
      /** returns the stick index */
      int GetGamepadIndex() const;
      /** returns true whether there is any pressed button */
      bool IsAnyButtonPressed(bool previous_frame = false) const;
      /** returns true whether there is any axis in use */
      bool IsAnyAxisAction(bool previous_frame = false) const;
      /** returns true whenever a buttons is pressed or an axis is in action */
      bool IsAnyAction(bool previous_frame = false) const;
      /** returns the direction of one stick (a combinaison of 2 axis) */
      glm::vec2 GetXBOXStickDirection(int stick_index, bool previous_frame = false) const;
      /** returns the number of buttons */
      size_t GetButtonCount() const;
      /** returns the number of axis */
      size_t GetAxisCount() const;

      /** returns true whether the gamepad is connected */
      bool IsPresent() const;

      void SetCallbacks(GamepadCallbacks * in_callbacks);

    protected:

      /** the manager */
      class GamepadManager * gamepad_manager = nullptr;
      /* the device */
      PhysicalGamepad * physical_device = nullptr;
      /** the callbacks */
      boost::intrusive_ptr<GamepadCallbacks> callbacks;
    };

    /**
    * GamepadManager : used to handle gamepads, there allocation, the dynamic change of their index ...
    */

    class GamepadManager : public ReferencedObject
    {
      friend class Gamepad;

    public:

      /** constructor */
      GamepadManager(float in_dead_zone = 0.2f);
      /** destructor */
      virtual ~GamepadManager();

      /** update all the joysticks */
      void Tick(float delta_time);

      /** create a gamepad */
      Gamepad * AllocateGamepad(bool want_connected = true, GamepadCallbacks * in_callbacks = nullptr);

      /** returns whether the given stick has any input set */
      static bool HasAnyInputs(int stick_index, float dead_zone);

    protected:

      /** called whenever a gamepad is being connected */
      void HandleGamepadConnection(PhysicalGamepad * physical_gamepad);
      /** called whenever a gamepad is being diconnected */
      void HandleGamepadDisconnection(PhysicalGamepad * physical_gamepad);

      /** find a gamepad still not connected */
      Gamepad * FindUnconnectedGamepad();
      /** find a gamepad that is used by nobody */
      PhysicalGamepad * FindUnallocatedPhysicalGamepad(bool want_connected);


#if 0
      /** called whenever a gamepad is being disconnected */
      virtual bool OnGamepadDisconnected(Gamepad * gamepad);
      /** called whenever a gamepad is being connected */
      virtual bool OnGamepadConnected(Gamepad * gamepad);
#endif
      /** called whenever a gamepad is destroyed */
      virtual bool OnGamepadDestroyed(Gamepad * gamepad);

    protected:

      /** the default dead zone value */
      float dead_zone;
      /** the logical gamepads */
      std::vector<Gamepad *> user_gamepads;
      /** the physical gamepads */
      std::vector<PhysicalGamepad *> physical_gamepads;
    };

  }; // namespace MyGLFW

}; // namespace chaos

