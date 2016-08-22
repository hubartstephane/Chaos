#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
  /**
   * MyGLFWGamepadAxisData : while max and min values for sticks are not always 1 (some XBOX has value lesser that 1.0),
   *                         we have to store the upper and lower values to renormalize the output
   */
  class MyGLFWGamepadAxisData
  {
  public:

    /** constructor */
    MyGLFWGamepadAxisData(float in_dead_zone) : dead_zone(in_dead_zone), raw_value(0.0f), min_value(-0.8f), max_value(+0.8f), final_value(0.0f){}
    /** update the value */
    void UpdateValue(float in_raw_value);
    /** get the value */
    inline float GetValue() const { return final_value; }

  protected:

    /** the dead zone of the stick */
    float dead_zone;
    /** value of the stick (with no filter) */
    float raw_value;
    /** min value always encountered */
    float min_value;
    /** max value always encountered */
    float max_value;
    /** the final value of the stick after computation */
    float final_value;
  };
  
  /**
   * MyGLFWGamepad : this is a logical gamepad .. may change the physical gamepad it is bound on
   */
  class MyGLFWGamepad
  {
  public:

    /** index in buttons of A for XBOX like pad */
    static int const XBOX_BUTTON_A = 0;
    /** index in buttons of B for XBOX like pad */
    static int const XBOX_BUTTON_B = 1;
    /** index in buttons of X for XBOX like pad */
    static int const XBOX_BUTTON_X = 2;
    /** index in buttons of Y for XBOX like pad */
    static int const XBOX_BUTTON_Y = 3;
    /** index in buttons of LEFT for XBOX like pad (the one behind the LEFT TRIGGER) */
    static int const XBOX_BUTTON_LEFTBUT  = 4;
    /** index in buttons of RIGHT for XBOX like pad (the one behind the RIGHT TRIGGER) */
    static int const XBOX_BUTTON_RIGHTBUT = 5;
    /** index in buttons of SELECT for XBOX like pad */
    static int const XBOX_BUTTON_SELECT = 6;
    /** index in buttons of START for XBOX like pad */
    static int const XBOX_BUTTON_START  = 7;
    /** index in buttons of LEFT-STICK-CLICKED for XBOX like pad */
    static int const XBOX_BUTTON_LEFTSTICK  = 8;
    /** index in buttons of RIGHT-STICK-CLICKED for XBOX like pad */
    static int const XBOX_BUTTON_RIGHTSTICK = 9;
    /** index in buttons of UP for XBOX like pad */
    static int const XBOX_BUTTON_UP    = 10;
    /** index in buttons of DOWN for XBOX like pad */
    static int const XBOX_BUTTON_DOWN  = 12;
    /** index in buttons of LEFT for XBOX like pad */
    static int const XBOX_BUTTON_LEFT  = 13;
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
    static int const XBOX_LEFT_AXIS  = 0;
    /** returns the direction of right stick (beware the low level interface only knows for axis) */
    static int const XBOX_RIGHT_AXIS = 1;

    friend class MyGLFWGamepadManager;

  protected:

    /** the constructor */
    MyGLFWGamepad(class MyGLFWGamepadManager * in_manager, float in_dead_zone) : 
      manager(in_manager),
      dead_zone(in_dead_zone), 
      stick_index(-1),
      ever_connected(false)
    { 
      assert(manager != nullptr);
    }
    /** the destructor is protected */
    virtual ~MyGLFWGamepad(){}
    /** update the gamepad values. There are 3 steps */
    void Tick(float delta_time, int step);
    /** update all the values for the axis and buttons */
    void UpdateAxisAndButtons();
    /** try to capture a stick if the flag correspond */
    void TryCaptureStick(bool wanted_ever_connected);

  public:

    /** returns true whether the gamepad is connected */
    inline bool IsPresent() const { return (stick_index >= 0); }
    /** returns the button state */
    bool IsButtonPressed(size_t button_index) const;
    /** returns the button state */
    float GetAxisValue(size_t axis_index) const;
    /** returns the stick index */
    int GetGamepadIndex() const { return stick_index; }
    /** returns true whether there is any pressed button */
    bool IsAnyButtonPressed() const;
    /** returns true whether there is any axis in use */
    bool IsAnyAxisAction() const;
    /** returns true whenever a buttons is pressed or an axis is in action */
    bool IsAnyAction() const;
    /** returns the direction of one stick (a combinaison of 2 axis) */
    glm::vec2 GetXBOXStickDirection(int stick_index) const;

  protected:

    /** manager */
    MyGLFWGamepadManager * manager;
    /** the zone for axis that is considered 0 */
    float dead_zone;
    /** the current stick index */
    int stick_index;
    /** indicates whether the stick has already be connected */
    bool ever_connected;
    /** the value for axis */
    std::vector<MyGLFWGamepadAxisData> axis;
    /** the value for buttons */
    std::vector<int> buttons;
  };

  /**
   * MyGLFWGamepadManager : used to handle gamepads, there allocation, the dynamic change of their index ...
   */

  class MyGLFWGamepadManager
  {
    friend class MyGLFWGamepad;

  public:

    /** constructor */
    MyGLFWGamepadManager(float in_dead_zone = 0.2f) : dead_zone(in_dead_zone){}

    /** update all the joysticks */
    void Tick(float delta_time);
    /** clean all the gamepad */
    void Reset();
    /** create a gamepad */
    MyGLFWGamepad * AllocateGamepad();
    /** release a gamepad */
    void FreeGamepad(MyGLFWGamepad * gamepad);

    /** returns whether the given stick has any input set */
    static bool HasAnyInputs(int stick_index, float dead_zone);

  protected:

    /** called whenever a gamepad is being disconnected */
    virtual bool OnGamepadDisconnected(MyGLFWGamepad * gamepad){return true;}
    /** called whenever a gamepad is being connected */
    virtual bool OnGamepadConnected(MyGLFWGamepad * gamepad){return true;}
    /** called to allocate a gamepad instance */
    virtual MyGLFWGamepad * NewGamepadInstance();

  protected:

    /** Ensure if a stick index is already in use */
    bool IsStickIndexInUse(int stick_index) const;
    /** Try to get the stick index if a gamepad that is not used and who has at least one entry triggered */
    int GetFreeStickIndex(int start_index) const;

    /** the default dead zone value */
    float dead_zone;
    /** the gamepads that have been allocated */
    std::vector<MyGLFWGamepad*> gamepads;
  };


}; // namespace chaos

