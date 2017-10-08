#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/MathTools.h>


namespace chaos
{
  namespace MyGLFW
  {

    // XXX : some sticks are not abled to physicaly returns 1.0 when they are fully triggered (depend on the device)
    //       that's why i use some min/max values (initialized with a coherent value)
    //       i the stick goes further than theses values, we update them.
    //       that help us to have a good evaluation of the stick range over time.

    void AxisData::UpdateValue(float in_raw_value, float dead_zone)
    {
      in_raw_value = MathTools::Clamp(in_raw_value, -1.0f, +1.0f);

      // store raw value
      raw_value = in_raw_value;
      max_value = max(max_value, in_raw_value);
      min_value = min(min_value, in_raw_value);

      // apply dead zone and normalization
      if (in_raw_value > dead_zone || in_raw_value < -dead_zone)
      {
        if (in_raw_value > 0.0f)
          final_value = (in_raw_value - dead_zone) / (max_value - dead_zone);
        else
          final_value = -(in_raw_value + dead_zone) / (min_value + dead_zone);
      }
      else
        final_value = 0.0f;
    }

 
    //
    // PhysicalGamepad functions
    //

    size_t PhysicalGamepad::GetButtonCount() const
    {
      return buttons.size() / 2; // divide by 2 because there is the previous frame in the upper part of the array
    }

    void PhysicalGamepad::ClearInputs()
    {
      axis.clear();
      buttons.clear();
    }

    size_t PhysicalGamepad::GetAxisCount() const
    {
      return axis.size() / 2; // divide by 2 because there is the previous frame in the upper part of the array
    }

    int PhysicalGamepad::GetButtonChanges(size_t button_index) const
    {
      bool current_state  = IsButtonPressed(button_index, false);
      bool previous_state = IsButtonPressed(button_index, true);

      if (current_state == previous_state)
        return (current_state) ? BUTTON_STAY_PRESSED : BUTTON_STAY_RELEASED;
      else
        return (current_state) ? BUTTON_BECOME_PRESSED : BUTTON_BECOME_RELEASED;
    }

    bool PhysicalGamepad::IsButtonPressed(size_t button_index, bool previous_frame) const
    {
      if (!IsPresent())
        return false;

      size_t count = GetButtonCount();
      if (button_index >= count)
        return false;

      if (previous_frame)
        button_index += count; // upper part of the array for previous_frame

      return (buttons[button_index] != 0);
    }

    float PhysicalGamepad::GetAxisValue(size_t axis_index, bool previous_frame) const
    {
      if (!IsPresent())
        return 0.0f;

      size_t count = GetAxisCount();
      if (axis_index >= count)
        return 0.0f;

      if (previous_frame)
        axis_index += count; // upper part of the array for previous_frame

      return axis[axis_index].GetValue();
    }

    bool PhysicalGamepad::IsAnyButtonPressed(bool previous_frame) const
    {
      if (!IsPresent())
        return false;

      size_t count = GetButtonCount();
      size_t start = (previous_frame) ? count : 0; // the array is split in 2 parts (first elements for current values, then previous frame history)
      size_t end = start + count;

      for (size_t i = start; i < end; ++i)
        if (buttons[i])
          return true;
      return false;
    }

    bool PhysicalGamepad::IsAnyAxisAction(bool previous_frame) const
    {
      if (!IsPresent())
        return false;

      size_t count = GetAxisCount();
      size_t start = (previous_frame) ? count : 0; // the array is split in 2 parts (first elements for current values, then previous frame history)
      size_t end = start + count;

      for (size_t i = start; i < end; ++i)
        if (axis[i].GetValue() != 0.0f)
          return true;
      return false;
    }

    bool PhysicalGamepad::IsAnyAction(bool previous_frame) const
    {
      return IsAnyButtonPressed(previous_frame) || IsAnyAxisAction(previous_frame);
    }

    glm::vec2 PhysicalGamepad::GetXBOXStickDirection(int stick_index, bool previous_frame) const
    {
      glm::vec2 result(0.0f, 0.0f);
      if (IsPresent())
      {
        if (stick_index == XBOX_LEFT_AXIS)
        {
          result.x = GetAxisValue(XBOX_LEFT_AXIS_X, previous_frame);
          result.y = GetAxisValue(XBOX_LEFT_AXIS_Y, previous_frame);
        }
        else if (stick_index == XBOX_RIGHT_AXIS)
        {
          result.x = GetAxisValue(XBOX_RIGHT_AXIS_X, previous_frame);
          result.y = GetAxisValue(XBOX_RIGHT_AXIS_Y, previous_frame);
        }
        else
          return result;

        // if the length is greater than 1, renormalize it to 1.0f !
        float sqr_len = result.x * result.x + result.y * result.y;
        if (sqr_len > 1.0f)
        {
          float len = MathTools::Sqrt(sqr_len);
          result.x /= len;
          result.y /= len;
        }
      }
      return result;
    }

    void PhysicalGamepad::UpdateAxisAndButtons(float delta_time, float dead_zone)
    {
      if (!IsPresent())
        return;

      int buttons_count = 0;
      int axis_count = 0;

      // update the axis
      float const * axis_buffer = glfwGetJoystickAxes(stick_index, &axis_count);

      size_t ac = (size_t)axis_count;

      bool axis_reallocated = (axis.size() != ac * 2);
      if (axis_reallocated)
      {
        axis.clear();
        axis.insert(axis.begin(), ac * 2, AxisData()); // 2 * because we want to insert row for previous frame

        for (size_t i = 0; i < ac; ++i)
        {
          axis[i].UpdateValue(axis_buffer[i], dead_zone);
          axis[i + ac] = axis[i];
        }
      }
      else
      {
        for (size_t i = 0; i < ac; ++i)
        {
          axis[i + ac] = axis[i]; // copy current frame to previous
          axis[i].UpdateValue(axis_buffer[i], dead_zone);
        }
      }

      // update the buttons
      unsigned char const * buttons_buffer = glfwGetJoystickButtons(stick_index, &buttons_count);

      size_t bc = (size_t)buttons_count;

      bool buttons_reallocated = (buttons.size() != bc * 2);
      if (buttons_reallocated)
      {
        buttons.clear();
        buttons.insert(buttons.begin(), bc * 2, 0);

        for (size_t i = 0; i < ac; ++i)
        {
          buttons[i] = buttons_buffer[i];
          buttons[i + bc] = buttons[i];
        }
      }
      else
      {
        for (size_t i = 0; i < bc; ++i)
        {
          buttons[i + bc] = buttons[i]; // copy current frame to previous
          buttons[i] = buttons_buffer[i];
        }
      }
    }


    //
    // Gamepad functions
    //

    Gamepad::Gamepad(GamepadManager * in_gamepad_manager, PhysicalGamepad * in_physical_device) : 
      gamepad_manager(in_gamepad_manager), 
      physical_device(in_physical_device)
    {
      assert(gamepad_manager != nullptr);
      if (physical_device != nullptr)
        physical_device->user_gamepad = this;
    }

    Gamepad::~Gamepad()
    {
      if (gamepad_manager != nullptr)
        gamepad_manager->OnGamepadDestroyed(this);
      if (physical_device != nullptr)
        physical_device->user_gamepad = nullptr;
    }

    size_t Gamepad::GetButtonCount() const
    {
      if (physical_device != nullptr)
        return physical_device->GetButtonCount();
      return 0;
    }

    size_t Gamepad::GetAxisCount() const
    {
      if (physical_device != nullptr)
        return physical_device->GetAxisCount();
      return 0;
    }

    int Gamepad::GetButtonChanges(size_t button_index) const
    {
      if (physical_device != nullptr)
        return physical_device->GetButtonChanges(button_index);
      return 0;
    }

    bool Gamepad::IsButtonPressed(size_t button_index, bool previous_frame) const
    {
      if (physical_device != nullptr)
        return physical_device->IsButtonPressed(button_index, previous_frame);
      return 0;
    }

    float Gamepad::GetAxisValue(size_t axis_index, bool previous_frame) const
    {
      if (physical_device != nullptr)
        return physical_device->GetAxisValue(axis_index, previous_frame);
      return 0.0f;
    }

    bool Gamepad::IsAnyButtonPressed(bool previous_frame) const
    {
      if (physical_device != nullptr)
        return physical_device->IsAnyButtonPressed(previous_frame);
      return false;
    }

    bool Gamepad::IsAnyAxisAction(bool previous_frame) const
    {
      if (physical_device != nullptr)
        return physical_device->IsAnyAxisAction(previous_frame);
      return false;
    }

    bool Gamepad::IsAnyAction(bool previous_frame) const
    {
      if (physical_device != nullptr)
        return physical_device->IsAnyAction(previous_frame);
      return false;
    }

    glm::vec2 Gamepad::GetXBOXStickDirection(int stick_index, bool previous_frame) const
    {
      if (physical_device != nullptr)
        return physical_device->GetXBOXStickDirection(stick_index, previous_frame);
      return glm::vec2(0.0f, 0.0f);
    }

    bool Gamepad::IsPresent() const
    {
      if (physical_device != nullptr)
        return physical_device->IsPresent();
      return false;
    }

    int Gamepad::GetGamepadIndex() const
    {
      if (physical_device != nullptr)
        return physical_device->GetGamepadIndex();
      return -1;
    }

    void Gamepad::SetCallbacks(GamepadCallbacks * in_callbacks)
    {
      callbacks = in_callbacks;
    }

    //
    // GamepadManager functions
    //

    GamepadManager::GamepadManager(float in_dead_zone) : dead_zone(in_dead_zone) 
    {
      physical_gamepads.reserve(MAX_SUPPORTED_GAMEPAD_COUNT); // allocate a PhysicalGamepad for all supported inputs
      for (int i = 0; i < MAX_SUPPORTED_GAMEPAD_COUNT; ++i)
        physical_gamepads.push_back(new PhysicalGamepad(i));
    }

    GamepadManager::~GamepadManager()
    {
      for (PhysicalGamepad * physical_gamepad : physical_gamepads) // destroy all physical gamepads
        if (physical_gamepad != nullptr)
          delete(physical_gamepad);
      physical_gamepads.clear();

      for (Gamepad * gamepad : user_gamepads) // notify all user gamepads about the destruction
      {
        if (gamepad != nullptr)
        {
          gamepad->gamepad_manager = nullptr;
          gamepad->physical_device = nullptr;
        }
      }
      user_gamepads.clear();
    }

#if 0
    bool GamepadManager::OnGamepadDisconnected(Gamepad * gamepad) 
    {
      assert(gamepad != nullptr);
      return true; 
    }

    bool GamepadManager::OnGamepadConnected(Gamepad * gamepad) 
    {
      assert(gamepad != nullptr);
      return true; 
    }
#endif

    bool GamepadManager::OnGamepadDestroyed(Gamepad * gamepad)
    {
      assert(gamepad != nullptr);

      size_t count = user_gamepads.size();
      for (size_t i = 0; i < count; ++i)
      {
        if (user_gamepads[i] == gamepad) // remove the gamepad from the list
        {
          if (i != count - 1)
            user_gamepads[i] = user_gamepads[count - 1]; // remove swap
          user_gamepads.pop_back();
          break;
        }
      }
      return true;
    }

    Gamepad * GamepadManager::AllocateGamepad(bool want_connected, GamepadCallbacks * in_callbacks) // user explicitly require a gamepad
    {
      PhysicalGamepad * physical_gamepad = FindUnallocatedPhysicalGamepad();
      if (want_connected && (physical_gamepad == nullptr || !physical_gamepad->IsPresent())) // all physical device in use or not present ?
        return nullptr;

      Gamepad * result = new Gamepad(this, physical_gamepad);
      if (result != nullptr)
      {
        user_gamepads.push_back(result);
        if (in_callbacks != nullptr)
          result->SetCallbacks(in_callbacks);
      }
      return result;
    }


    // user explicitly require a physical gamepad
    PhysicalGamepad * GamepadManager::FindUnallocatedPhysicalGamepad()
    {
      for (int step = 0; step <= 2; ++step)
      {
        for (size_t i = 0; i < physical_gamepads.size(); ++i)
        {
          PhysicalGamepad * physical_gamepad = physical_gamepads[i];
          if (physical_gamepad == nullptr)
            continue;

          if (physical_gamepad->IsAllocated()) // we want an entry that is owned by nobody
            continue;

          if (step == 0) // the best is to find a gamepad PRESENT and that has any input
          {
            if (physical_gamepad->IsPresent() && physical_gamepad->IsAnyAction(false))
              return physical_gamepad;
          }
          else if (step == 1) // the best is to find a gamepad PRESENT (even if no input)
          {
            if (physical_gamepad->IsPresent())
              return physical_gamepad;
          }
          else if (step == 2)
          {
            return physical_gamepad; // any gamepad will be enought
          }
        }
      }
      return nullptr;
    }







    void GamepadManager::Tick(float delta_time)
    {
      // XXX : we use a 2 steps algorithm because we want to be sure to have all physical gamepads state correct before working with user device

      bool was_present[MAX_SUPPORTED_GAMEPAD_COUNT] = { false };
      bool is_present [MAX_SUPPORTED_GAMEPAD_COUNT] = { false };

      size_t count = MAX_SUPPORTED_GAMEPAD_COUNT;
      assert(count == physical_gamepads.size());

      // update physical stick status
      for (size_t i = 0; i < count; ++i)
      {
        PhysicalGamepad * physical_gamepad = physical_gamepads[i];
        if (physical_gamepad == nullptr)
          continue;

        was_present[i] = physical_gamepad->IsPresent();
        is_present[i]  = (glfwJoystickPresent(i) > 0);

        if (is_present[i])
          physical_gamepad->UpdateAxisAndButtons(delta_time, dead_zone);

        physical_gamepad->is_present = is_present[i];
      }

      // handle user gamepads
      for (size_t i = 0; i < count; ++i)
      {
        PhysicalGamepad * physical_gamepad = physical_gamepads[i];
        if (physical_gamepad == nullptr)
          continue;

        if (is_present[i] == was_present[i]) // do not connect/disconnect any user gamepad
          continue;

        if (is_present[i]) // new connection
          HandleGamepadConnection(physical_gamepad);
        else // disconnection
          HandleGamepadDisconnection(physical_gamepad);
      }
    }

    void GamepadManager::HandleGamepadConnection(PhysicalGamepad * physical_gamepad) // a new physical device is beeing detected. Give it to a user
    {
      Gamepad * gamepad = FindUnconnectedGamepad();
      if (gamepad != nullptr)
      {
        gamepad->physical_device = physical_gamepad; // bind physical and logical device altogether
        physical_gamepad->user_gamepad = gamepad;

        if (gamepad->callbacks != nullptr)
          gamepad->callbacks->OnGamepadConnected(gamepad); // if stick was already given to someone, warn him about the reconnection      
      }
      else
        OnUnallocatedGamepadInput(physical_gamepad);
    }

    Gamepad * GamepadManager::FindUnconnectedGamepad()
    {
      for (Gamepad * gamepad : user_gamepads)
      {
        if (gamepad == nullptr)
          continue;
        if (gamepad->physical_device != nullptr)
          continue;
        return gamepad;
      }
    }

    void GamepadManager::HandleGamepadDisconnection(PhysicalGamepad * physical_gamepad)
    {
      Gamepad * user_gamepad = physical_gamepad->user_gamepad;
      if (user_gamepad != nullptr)
      {
        user_gamepad->physical_device = nullptr;
        physical_gamepad->user_gamepad = nullptr;

        if (user_gamepad->callbacks != nullptr)
          user_gamepad->callbacks->OnGamepadDisconnected(user_gamepad);



        ... try to auto reconnect

      }
    }


#if 0

















    // XXX : this function may be called with 2 purposes :
    //
    //       -the user require explicitly a gamepad for is own ownership
    //           => try to recycle a gamepad that is used by nobody
    //
    //       -the tick(..) function has detect a new incoming gamepad
    //           => try to recycle a gamepad that has no ID
    //           => preference is to find a gamepad that is USED by someone

    PhysicalGamepad * GamepadManager::FindPhysicalGamepad(bool want_unallocated)
    {
      return (want_unallocated) ? FindUnallocatedPhysicalGamepad() : FindNotPresentPhysicalGamepad();
    }

    // XXX : We want to give a gamepad ID => search a gamepad that is not yet bound to an ID
    //       Prefere a gamepad that is allocated for someone usage
    PhysicalGamepad * GamepadManager::FindNotPresentPhysicalGamepad()
    {
      for (int step = 0; step <= 2; ++step)
      {
        for (size_t i = 0; i < physical_gamepads.size(); ++i)
        {
          PhysicalGamepad * physical_gamepad = physical_gamepads[i].get();
          if (physical_gamepad == nullptr)
            continue;

          if (physical_gamepad->IsPresent()) // we do not want a gamepad that already has an ID
            continue;

          if (step == 0) // the best is to find a gamepad IN USE that has already been connected once (recycle)
          {
            if (physical_gamepad->IsAllocated() && physical_gamepad->IsEverConnected()) // this is a lost gamepad
              return physical_gamepad;
          }
          else if (step == 1) // gamepad IN USE that never has been connected is fine too (2nd better choice)
          {
            if (physical_gamepad->IsAllocated())
              return physical_gamepad;
          }
          else if (step == 2) // last choice
          {
            return physical_gamepad; // any gamepad still not present will be enought
          }
        }
      }
      return nullptr;
    }




































    void GamepadManager::HandleGamepadTick(PhysicalGamepad * physical_gamepad, float delta_time) // no state change
    {
      physical_gamepad->UpdateAxisAndButtons(delta_time, dead_zone);
      if (physical_gamepad == nullptr)
        physical_gamepad->user_gamepad = OnUnallocatedGamepadInput(physical_gamepad); // auto Allocate the gamepad to a client
    }

    void GamepadManager::HandleGamepadDisconnection(PhysicalGamepad * physical_gamepad) // gamepad was connected, it is no more
    {
      physical_gamepad->stick_index = -1; // the gamepad is no more connected
      physical_gamepad->ClearInputs();

      Gamepad * gamepad = physical_gamepad->user_gamepad;

      if (gamepad != nullptr && gamepad->callbacks != nullptr)
        gamepad->callbacks->OnGamepadDisconnected(gamepad);
    }



#endif















    // Some explanation : considering that a gamepad represents a player,
    //                    if a gamepad is disconnected, it has now greater priority for recapture that a gamepad that never has been captured
    //                    (want to reconnect old player instead of introducing a new one)
    //
    //                    the manager has the opportunity to refuse a stick input

    bool GamepadManager::HasAnyInputs(int stick_index, float dead_zone)
    {
      if (glfwJoystickPresent(stick_index)) // ensure any input is triggered
      {
        int buttons_count = 0;
        int axis_count = 0;

        float const * axis_buffer = glfwGetJoystickAxes(stick_index, &axis_count);
        if (axis_buffer != nullptr)
          for (size_t i = 0; i < (size_t)axis_count; ++i)
            if (axis_buffer[i] > dead_zone || axis_buffer[i] < -dead_zone) // is axis valid
              return true;

        unsigned char const * buttons_buffer = glfwGetJoystickButtons(stick_index, &buttons_count);
        if (buttons_buffer != nullptr)
          for (size_t i = 0; i < (size_t)buttons_count; ++i)
            if (buttons_buffer[i]) // is button pressed ?
              return true;
      }
      return false;
    }


  }; // namespace MyGLFW

}; // namespace chaos
