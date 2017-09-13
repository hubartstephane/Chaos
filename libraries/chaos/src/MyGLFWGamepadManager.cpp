#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/MathTools.h>


namespace chaos
{

  void MyGLFWGamepadAxisData::UpdateValue(float in_raw_value)
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

  size_t MyGLFWGamepad::GetButtonCount() const
  {
    return buttons.size() / 2; // divide by 2 because there is the previous frame in the upper part of the array
  }

  MyGLFWGamepad::~MyGLFWGamepad()
  {
    manager->FreeGamepad(this);
  }

  size_t MyGLFWGamepad::GetAxisCount() const
  {
    return axis.size() / 2; // divide by 2 because there is the previous frame in the upper part of the array
  }

  int MyGLFWGamepad::GetButtonChanges(size_t button_index) const
  {
    bool current_state = IsButtonPressed(button_index, false);
    bool previous_state = IsButtonPressed(button_index, true);

    if (current_state == previous_state)
      return (current_state) ? BUTTON_STAY_PRESSED : BUTTON_STAY_RELEASED;
    else
      return (current_state) ? BUTTON_BECOME_PRESSED : BUTTON_BECOME_RELEASED;
  }

  bool MyGLFWGamepad::IsButtonPressed(size_t button_index, bool previous_frame) const
  {
    if (!IsPresent())
      return false;

    size_t count = buttons.size();
    if (button_index >= count / 2)
      return false;

    if (previous_frame)
      button_index += count / 2; // upper part of the array for previous_frame

    return (buttons[button_index] != 0);
  }

  float MyGLFWGamepad::GetAxisValue(size_t axis_index, bool previous_frame) const
  {
    if (!IsPresent())
      return 0.0f;

    size_t count = axis.size();
    if (axis_index >= count / 2)
      return 0.0f;

    if (previous_frame)
      axis_index += count / 2; // upper part of the array for previous_frame

    return axis[axis_index].GetValue();
  }

  bool MyGLFWGamepad::IsAnyButtonPressed(bool previous_frame) const
  {
    if (!IsPresent())
      return false;

    size_t count = buttons.size();
    size_t start = (previous_frame) ? count / 2 : 0;
    size_t end = start + count / 2;

    for (size_t i = start; i < end; ++i)
      if (buttons[i])
        return true;
    return false;
  }

  bool MyGLFWGamepad::IsAnyAxisAction(bool previous_frame) const
  {
    if (!IsPresent())
      return false;

    size_t count = axis.size();
    size_t start = (previous_frame) ? count / 2 : 0;
    size_t end = start + count / 2;

    for (size_t i = start; i < end; ++i)
      if (axis[i].GetValue() != 0.0f)
        return true;
    return false;
  }

  bool MyGLFWGamepad::IsAnyAction(bool previous_frame) const
  {
    return IsAnyButtonPressed(previous_frame) || IsAnyAxisAction(previous_frame);
  }

  glm::vec2 MyGLFWGamepad::GetXBOXStickDirection(int stick_index, bool previous_frame) const
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

  void MyGLFWGamepad::UpdateAxisAndButtons()
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
      axis.insert(axis.begin(), ac * 2, MyGLFWGamepadAxisData(dead_zone)); // 2 * because we want to insert row for previous frame

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

  MyGLFWGamepadPresenceInfo MyGLFWGamepadManager::GetGamepadPresenceInfo()
  {
    MyGLFWGamepadPresenceInfo result;
    for (size_t i = 0; i < gamepad_entries.size(); ++i)
      result.InsertGamepadEntry(gamepad_entries[i], i); // collect all GAMEPAD sorted by ID
    return result;
  }

  MyGLFWGamepadEntry * MyGLFWGamepadManager::FindNotPresentGamepadEntry()
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
          if (entry.allocated && entry.gamepad->IsEverConnected())
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

  MyGLFWGamepadEntry * MyGLFWGamepadManager::FindUnallocatedGamepadEntry()
  {
    for (int step = 0; step <= 1; ++step)
    {
      for (size_t i = 0; i < gamepad_entries.size(); ++i)
      {
        MyGLFWGamepadEntry & entry = gamepad_entries[i];
        if (entry.gamepad == nullptr)
          continue;

        if (entry.allocated) // we want an entry that is owned by nobody
          continue;

        if (step == 0) // the best is to find a gamepad PRESENT
        {
          if (entry.gamepad->IsPresent())
            return &entry;
        }
        else if (step == 1)
        {
          return &entry; // any gamepad will be enought
        }
      }
    }
    return nullptr;
  }


  MyGLFWGamepad * MyGLFWGamepadManager::NewGamepadInstance(MyGLFWGamepadCallbacks * in_callbacks)
  {
    return new MyGLFWGamepad(this, dead_zone, in_callbacks);
  }

  MyGLFWGamepad * MyGLFWGamepadManager::AllocateGamepad(MyGLFWGamepadCallbacks * in_callbacks) // user explicitly require a gamepad
  {
    MyGLFWGamepadEntry * gamepad_entry = AllocateGamepadEntry(in_callbacks, true);
    if (gamepad_entry == nullptr)
      return nullptr;
    gamepad_entry->allocated = true;
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

  MyGLFWGamepadEntry * MyGLFWGamepadManager::AllocateGamepadEntry(MyGLFWGamepadCallbacks * in_callbacks, bool want_unallocated)
  {
    // try to recycle a gamepad
    MyGLFWGamepadEntry * new_entry = (want_unallocated) ? FindUnallocatedGamepadEntry() : FindNotPresentGamepadEntry();

    if (new_entry != nullptr)
      return new_entry;

    // or create a new one
    MyGLFWGamepad * new_gamepad = NewGamepadInstance(nullptr);
    if (new_gamepad == nullptr)
      return nullptr;

    new_gamepad->UpdateAxisAndButtons();

    MyGLFWGamepadEntry tmp;
    tmp.gamepad = new_gamepad;
    tmp.allocated = false;
    gamepad_entries.push_back(tmp);

    return &gamepad_entries.back();
  }







  void MyGLFWGamepadManager::Tick(float delta_time)
  {
    MyGLFWGamepadPresenceInfo presence_info = GetGamepadPresenceInfo();

    for (size_t i = 0; i < MyGLFWGamepadPresenceInfo::MAX_SUPPORT_STICK_COUNT; ++i) // iterate over MAX SUPPORTED stick index
    {
      size_t entry_index = presence_info.entries[i];
      if (entry_index < 0 && entry_index >= gamepad_entries.size()) // indirection with the map
        continue;

      MyGLFWGamepadEntry & entry = gamepad_entries[entry_index];

      bool gamepad_present = (glfwJoystickPresent(i) > 0);

      if (entry.gamepad == nullptr) // no gamepad is associated with this entry : detect new ?
      {
        if (gamepad_present) // require to take/recycle a gamepad
          HandleGamepadConnection(i);
      }
      else // gamepad associated with the entry
      {
        if (gamepad_present) // gamepad still present
          HandleGamepadTick(entry, delta_time);
        else // gamepad lost
          HandleGamepadDisconnection(entry);
      }
    }
  }

  void MyGLFWGamepadManager::HandleGamepadDisconnection(MyGLFWGamepadEntry & entry)
  {

  }
  
  void MyGLFWGamepadManager::HandleGamepadTick(MyGLFWGamepadEntry & entry, float delta_time)
  {

  }

  void MyGLFWGamepadManager::HandleGamepadConnection(int stick_index)
  {


#if 0
    MyGLFWGamepadEntry * new_entry = nullptr;

    // try to recycle a gamepad
    for (int step = 0; (step <= 2) && (new_entry == nullptr); ++step)
      new_entry = FindUnusedGamepadEntry(step);

    // or create a new one
    if (new_entry == nullptr)
    {
      MyGLFWGamepad * new_gamepad = NewGamepadInstance(nullptr);
      if (new_gamepad == nullptr)
        return;

      MyGLFWGamepadEntry tmp;
      tmp.gamepad = new_gamepad;
      tmp.allocated = false;
      gamepad_entries.push_back(tmp);

      new_entry = &gamepad_entries.back();
    }

    // handle the stick
    MyGLFWGamepad * gamepad = new_entry->gamepad;

    gamepad->stick_index = stick_index;
    gamepad->UpdateAxisAndButtons();

    gamepad->callbacks->OnGamepadDisconnected(gamepad);

    gamepad->callbacks.

      if ()
        manager->OnGamepadConnected(this, first_connection))



#endif

  }
  
  // Some explanation : considering that a gamepad represents a player,
  //                    if a gamepad is disconnected, it has now greater priority for recapture that a gamepad that never has been captured
  //                    (want to reconnect old player instead of introducing a new one)
  //
  //                    the manager has the opportunity to refuse a stick input

  bool MyGLFWGamepadManager::HasAnyInputs(int stick_index, float dead_zone)
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








  void MyGLFWGamepadManager::FreeGamepad(MyGLFWGamepad * gamepad)
  {
#if 0
    assert(gamepad != nullptr);
    assert(gamepad->manager == this);

    auto it = std::find(gamepads.begin(), gamepads.end(), gamepad);
    if (it != gamepads.end())
    {
      size_t index = &*it - &gamepads[0];
      gamepads[index] = gamepads.back();
      gamepads.pop_back();
    }
#endif
  }

  void MyGLFWGamepadManager::Reset()
  {
#if 0
    for (MyGLFWGamepad * gamepad : gamepads)
      if (gamepad != nullptr)
        delete(gamepad);
    gamepads.clear();
#endif
  }

}; // namespace chaos
