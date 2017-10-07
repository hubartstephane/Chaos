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
    // Gamepad functions
    //

    Gamepad::Gamepad(PhysicalGamepad * in_physical_device) : physical_device(in_physical_device)
    {
      assert(in_physical_device);

      physical_device->user_gamepad = this;
    }

    Gamepad::~Gamepad()
    {
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
   
    bool Gamepad::IsEverConnected() const
    {
      if (physical_device != nullptr)
        return physical_device->IsEverConnected();
      return false;
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
    // GamepadPresenceInfo functions
    //

    bool GamepadPresenceInfo::InsertGamepad(PhysicalGamepad * gamepad, size_t entry_index)
    {
      if (gamepad == nullptr)
        return false;
      if (!gamepad->IsPresent())
        return false;

      int index = gamepad->GetGamepadIndex();
      assert(index >= 0 && index <= MAX_SUPPORTED_GAMEPAD_COUNT);

      entries[index] = entry_index;
      return true;
    }

    //
    // GamepadManager functions
    //

    GamepadPresenceInfo GamepadManager::GetGamepadPresenceInfo()
    {
      GamepadPresenceInfo result;
      for (size_t i = 0; i < physical_gamepads.size(); ++i)
        result.InsertGamepad(physical_gamepads[i].get(), i); // collect all GAMEPAD sorted by ID
      return result;
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

    // XXX : user explicitly require a gamepad
    //       try to give one that is bound to an ID
    PhysicalGamepad * GamepadManager::FindUnallocatedPhysicalGamepad()
    {
      for (int step = 0; step <= 2; ++step)
      {
        for (size_t i = 0; i < physical_gamepads.size(); ++i)
        {
          PhysicalGamepad * physical_gamepad = physical_gamepads[i].get();
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



    Gamepad * GamepadManager::AllocateGamepad(GamepadCallbacks * in_callbacks) // user explicitly require a gamepad
    {
      PhysicalGamepad * physical_gamepad = AllocatePhysicalGamepad(true);
      if (physical_gamepad == nullptr)
        return nullptr;

      Gamepad * result = new Gamepad(physical_gamepad);
      if (result != nullptr)
      {
        result->SetCallbacks(in_callbacks);
        physical_gamepad->ever_connected = physical_gamepad->IsPresent();
      }
      return result;
    }









    // XXX : this function may be called with 2 purposes :
    //
    //       -the user require explicitly a gamepad for is own ownership
    //           => try to recycle a gamepad that is used by nobody
    //
    //       -the tick(..) function has detect a new incoming gamepad
    //           => try to recycle a gamepad that has no ID
    //           => preference is to find a gamepad that is USED by someone

    PhysicalGamepad * GamepadManager::AllocatePhysicalGamepad(bool want_unallocated)
    {
      // try to recycle a gamepad
      PhysicalGamepad * result = (want_unallocated) ? FindUnallocatedPhysicalGamepad() : FindNotPresentPhysicalGamepad();
      if (result != nullptr)
        return result;

      // or create a new one
      result = new PhysicalGamepad();
      if (result == nullptr)
        return nullptr;

      // update the information
      result->UpdateAxisAndButtons(0.0f, dead_zone);

      physical_gamepads.push_back(result);

      return result;
    }

    void GamepadManager::Tick(float delta_time)
    {
      GamepadPresenceInfo presence_info = GetGamepadPresenceInfo();

      for (size_t stick_index = 0; stick_index < MAX_SUPPORTED_GAMEPAD_COUNT; ++stick_index) // iterate over MAX SUPPORTED stick index
      {
        size_t entry_index = presence_info.entries[stick_index];

        // get the ENTRY corresponding to this ID (if any)
        PhysicalGamepad * physical_gamepad = nullptr;
        if (entry_index >= 0 && entry_index < physical_gamepads.size()) // indirection with the map (value is std::numeric_limit<size_t>::max() if not initialized)
          physical_gamepad = physical_gamepads[entry_index].get();

        // is the gamepad corresponding to this ID present ?
        bool gamepad_present = (glfwJoystickPresent(stick_index) > 0);

        // no gamepad is associated with this ID : detect new ?
        if (physical_gamepad == nullptr)
        {
          if (gamepad_present) // require to take/recycle a gamepad
            HandleGamepadConnection(stick_index);
        }
        // some gamepad associated with the ID
        else
        {
          if (gamepad_present) // gamepad still present
            HandleGamepadTick(physical_gamepad, delta_time); // no state change
          else // gamepad lost
            HandleGamepadDisconnection(physical_gamepad); // was present, is no more present
        }
      }
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

    void GamepadManager::HandleGamepadConnection(int stick_index)
    {
      PhysicalGamepad * physical_gamepad = AllocatePhysicalGamepad(false);
      if (physical_gamepad == nullptr)
        return;

      physical_gamepad->stick_index = stick_index;

      Gamepad * gamepad = physical_gamepad->user_gamepad;
      if (gamepad != nullptr)
      {
        if (gamepad->callbacks != nullptr)
          gamepad->callbacks->OnGamepadConnected(gamepad); // if stick was already given to someone, warn him about the reconnection      
      }
      else
        physical_gamepad->user_gamepad = OnUnallocatedGamepadInput(physical_gamepad);

      physical_gamepad->ever_connected = true;
    }








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
