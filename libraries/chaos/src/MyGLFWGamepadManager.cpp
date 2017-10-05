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

    void GamepadAxisData::UpdateValue(float in_raw_value, float dead_zone)
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

    Gamepad::~Gamepad()
    {
      if (physical_device != nullptr)
        physical_device->allocated = false;
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
      bool current_state = IsButtonPressed(button_index, false);
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

    void PhysicalGamepad::UpdateAxisAndButtons(float dead_zone)
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
        axis.insert(axis.begin(), ac * 2, GamepadAxisData(dead_zone)); // 2 * because we want to insert row for previous frame

        for (size_t i = 0; i < ac; ++i)
        {
          axis[i].UpdateValue(axis_buffer[i]);
          axis[i + ac] = axis[i];
        }
      }
      else
      {
        for (size_t i = 0; i < ac; ++i)
        {
          axis[i + ac] = axis[i]; // copy current frame to previous
          axis[i].UpdateValue(axis_buffer[i]);
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











    bool MyGLFWGamepadPresenceInfo::InsertGamepadEntry(MyGLFWGamepadEntry & entry, size_t entry_index)
    {
      if (entry.gamepad == nullptr)
        return false;
      if (!entry.gamepad->IsPresent())
        return false;

      int index = entry.gamepad->GetGamepadIndex();
      assert(index >= 0 && index <= MyGLFWGamepadPresenceInfo::MAX_SUPPORT_STICK_COUNT);

      entries[index] = entry_index;
      return true;
    }

    MyGLFWGamepadPresenceInfo GamepadManager::GetGamepadPresenceInfo()
    {
      MyGLFWGamepadPresenceInfo result;
      for (size_t i = 0; i < gamepad_entries.size(); ++i)
        result.InsertGamepadEntry(gamepad_entries[i], i); // collect all GAMEPAD sorted by ID
      return result;
    }

    // XXX : We want to give a gamepad ID => search a gamepad that is not yet bound to an ID
    //       Prefere a gamepad that is allocated for someone usage
    MyGLFWGamepadEntry * GamepadManager::FindNotPresentGamepadEntry()
    {
      for (int step = 0; step <= 2; ++step)
      {
        for (size_t i = 0; i < gamepad_entries.size(); ++i)
        {
          MyGLFWGamepadEntry & entry = gamepad_entries[i];
          if (entry.gamepad == nullptr)
            continue;

          if (entry.gamepad->IsPresent()) // we do not want a gamepad that already has an ID
            continue;

          if (step == 0) // the best is to find a gamepad IN USE that has already been connected once (recycle)
          {
            if (entry.allocated && entry.gamepad->IsEverConnected()) // this is a lost gamepad
              return &entry;
          }
          else if (step == 1) // gamepad IN USE that never has been connected in fine too (2nd better choice)
          {
            if (entry.allocated)
              return &entry;
          }
          else if (step == 2) // last choice
          {
            return &entry; // any gamepad still not present will be enought
          }
        }
      }
      return nullptr;
    }

    // XXX : user explicitly require a gamepad
    //       try to give one that is bound to an ID
    MyGLFWGamepadEntry * GamepadManager::FindUnallocatedGamepadEntry()
    {
      for (int step = 0; step <= 2; ++step)
      {
        for (size_t i = 0; i < gamepad_entries.size(); ++i)
        {
          MyGLFWGamepadEntry & entry = gamepad_entries[i];
          if (entry.gamepad == nullptr)
            continue;

          if (entry.allocated) // we want an entry that is owned by nobody
            continue;

          if (step == 0) // the best is to find a gamepad PRESENT and that has any input
          {
            if (entry.gamepad->IsPresent() && entry.gamepad->IsAnyAction(false))
              return &entry;
          }
          else if (step == 1) // the best is to find a gamepad PRESENT (even if no input)
          {
            if (entry.gamepad->IsPresent())
              return &entry;
          }
          else if (step == 2)
          {
            return &entry; // any gamepad will be enought
          }
        }
      }
      return nullptr;
    }


    MyGLFWGamepad * GamepadManager::NewGamepadInstance()
    {
      return new MyGLFWGamepad(this, dead_zone);
    }

    MyGLFWGamepad * GamepadManager::AllocateGamepad(MyGLFWGamepadCallbacks * in_callbacks) // user explicitly require a gamepad
    {
      MyGLFWGamepadEntry * gamepad_entry = AllocateGamepadEntry(true);
      if (gamepad_entry == nullptr)
        return nullptr;
      gamepad_entry->allocated = true;
      gamepad_entry->gamepad->ever_connected = gamepad_entry->gamepad->IsPresent();
      gamepad_entry->gamepad->callbacks = in_callbacks;
      return gamepad_entry->gamepad;
    }

    // XXX : this function may be called with 2 purposes :
    //
    //       -the user require explicitly a gamepad for is own ownership
    //           => try to recycle a gamepad that is used by nobody
    //
    //       -the tick(..) function has detect a new incoming gamepad
    //           => try to recycle a gamepad that has no ID
    //           => preference is to find a gamepad that is USED by someone

    MyGLFWGamepadEntry * GamepadManager::AllocateGamepadEntry(bool want_unallocated)
    {
      // try to recycle a gamepad
      MyGLFWGamepadEntry * new_entry = (want_unallocated) ? FindUnallocatedGamepadEntry() : FindNotPresentGamepadEntry();
      if (new_entry != nullptr)
        return new_entry;

      // or create a new one
      MyGLFWGamepad * new_gamepad = NewGamepadInstance();
      if (new_gamepad == nullptr)
        return nullptr;

      // update the information
      new_gamepad->UpdateAxisAndButtons();

      // insert new entry
      MyGLFWGamepadEntry tmp;
      tmp.gamepad = new_gamepad;
      tmp.allocated = false;
      gamepad_entries.push_back(tmp);

      return &gamepad_entries.back();
    }

    void GamepadManager::Tick(float delta_time)
    {
      MyGLFWGamepadPresenceInfo presence_info = GetGamepadPresenceInfo();

      for (size_t stick_index = 0; stick_index < MyGLFWGamepadPresenceInfo::MAX_SUPPORT_STICK_COUNT; ++stick_index) // iterate over MAX SUPPORTED stick index
      {
        size_t entry_index = presence_info.entries[stick_index];

        // get the ENTRY corresponding to this ID (if any)
        MyGLFWGamepadEntry * entry = nullptr;
        if (entry_index >= 0 && entry_index < gamepad_entries.size()) // indirection with the map (value is std::numeric_limit<size_t>::max() if not initialized)
          entry = &gamepad_entries[entry_index];

        // is the gamepad corresponding to this ID present ?
        bool gamepad_present = (glfwJoystickPresent(stick_index) > 0);

        // no gamepad is associated with this ID : detect new ?
        if (entry == nullptr)
        {
          if (gamepad_present) // require to take/recycle a gamepad
            HandleGamepadConnection(stick_index);
        }
        // some gamepad associated with the ID
        else
        {
          if (gamepad_present) // gamepad still present
            HandleGamepadTick(*entry, delta_time); // no state change
          else // gamepad lost
            HandleGamepadDisconnection(*entry); // was present, is no more present
        }
      }
    }

    void GamepadManager::HandleGamepadTick(MyGLFWGamepadEntry & entry, float delta_time) // no state change
    {
      entry.gamepad->UpdateAxisAndButtons();
      if (!entry.allocated)
        entry.allocated = OnUnallocatedGamepadInput(entry.gamepad); // auto Allocate the gamepad to a client
    }

    void GamepadManager::HandleGamepadDisconnection(MyGLFWGamepadEntry & entry) // gamepad was connected, it is no more
    {
      MyGLFWGamepad * gamepad = entry.gamepad;

      gamepad->stick_index = -1; // the gamepad is no more connected
      gamepad->ClearInputs();
      if (entry.allocated && gamepad->callbacks != nullptr)
        gamepad->callbacks->OnGamepadDisconnected(gamepad);
    }

    void GamepadManager::HandleGamepadConnection(int stick_index)
    {
      MyGLFWGamepadEntry * gamepad_entry = AllocateGamepadEntry(false);
      if (gamepad_entry == nullptr)
        return;

      MyGLFWGamepad * gamepad = gamepad_entry->gamepad;
      gamepad->stick_index = stick_index;
      if (gamepad_entry->allocated)
      {
        if (gamepad->callbacks != nullptr)
          gamepad->callbacks->OnGamepadConnected(gamepad); // if stick was already given to someone, warn him about the reconnection      
      }
      else
        gamepad_entry->allocated = OnUnallocatedGamepadInput(gamepad);

      gamepad->ever_connected = true;
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

    void GamepadManager::FreeGamepad(MyGLFWGamepad * gamepad)
    {
      assert(gamepad != nullptr);
      assert(gamepad->manager == this);

      size_t count = gamepad_entries.size();
      for (size_t i = 0; i < count; ++i)
      {
        if (gamepad_entries[i].gamepad == gamepad) // search th entry corresponding to the gamepad
        {
          if (i != count - 1)
            gamepad_entries[i] = gamepad_entries.back(); // remove swap
          gamepad_entries.pop_back();
          return;
        }
      }
    }

  }; // namespace MyGLFW

}; // namespace chaos
