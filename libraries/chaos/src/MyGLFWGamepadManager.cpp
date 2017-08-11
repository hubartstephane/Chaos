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
      final_value =  (in_raw_value - dead_zone) / (max_value - dead_zone);
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

size_t MyGLFWGamepad::GetAxisCount() const
{
	return axis.size() / 2; // divide by 2 because there is the previous frame in the upper part of the array
}

int MyGLFWGamepad::GetButtonChanges(size_t button_index) const
{
	bool current_state  = IsButtonPressed(button_index, false);
	bool previous_state = IsButtonPressed(button_index, true);

	if (current_state == previous_state)
		return (current_state)? BUTTON_STAY_PRESSED : BUTTON_STAY_RELEASED;
	else
		return (current_state)? BUTTON_BECOME_PRESSED : BUTTON_BECOME_RELEASED;
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
  size_t start = (previous_frame)? count / 2 : 0;
  size_t end   = start + count / 2;

  for (size_t i = start ; i < end ; ++i)
	  if (buttons[i])
		  return true;
  return false;
}

bool MyGLFWGamepad::IsAnyAxisAction(bool previous_frame) const
{
  if (!IsPresent())
    return false;

  size_t count = axis.size();
  size_t start = (previous_frame)? count / 2 : 0;
  size_t end   = start + count / 2;

  for (size_t i = start ; i < end ; ++i)
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
  int buttons_count = 0;
  int axis_count    = 0;

  // update the axis
  float const * axis_buffer = glfwGetJoystickAxes(stick_index,  &axis_count);

  size_t ac = (size_t)axis_count;
  
  bool axis_reallocated = (axis.size() != ac * 2);
  if (axis_reallocated)
  {
    axis.clear();
    axis.insert(axis.begin(), ac * 2, MyGLFWGamepadAxisData(dead_zone)); // 2 * because we want to insert row for previous frame

	for (size_t i = 0 ; i < ac ; ++i)
	{
		axis[i].UpdateValue(axis_buffer[i]);
		axis[i + ac] = axis[i];
	}
  }
  else
  {
	  for (size_t i = 0 ; i < ac ; ++i)
	  {
		  axis[i + ac] = axis[i]; // copy current frame to previous
		  axis[i].UpdateValue(axis_buffer[i]);
	  }  
  }

  // update the buttons
  unsigned char const * buttons_buffer = glfwGetJoystickButtons(stick_index,  &buttons_count);

  size_t bc = (size_t)buttons_count;

  bool buttons_reallocated = (buttons.size() != bc * 2);
  if (buttons_reallocated)
  {
    buttons.clear();
    buttons.insert(buttons.begin(), bc * 2, 0);   

	for (size_t i = 0 ; i < ac ; ++i)
	{
		buttons[i] = buttons_buffer[i]; 
		buttons[i + ac] = buttons[i];
	}
  }
  else
  {
	  for (size_t i = 0 ; i < bc ; ++i)
	  {
		  buttons[i + ac] = buttons[i]; // copy current frame to previous
		  buttons[i] = buttons_buffer[i];
	  }  
  }
}

void MyGLFWGamepad::TryCaptureStick(bool wanted_ever_connected)
{
  if (ever_connected == wanted_ever_connected)
  {
    int start_search_index = GLFW_JOYSTICK_1;
    while (start_search_index <= GLFW_JOYSTICK_LAST) // search very first stick that is accepted by the manager
    {
      // get a stick with some input 
      int new_stick_index = manager->GetFreeStickIndex(start_search_index);
      if (new_stick_index < 0)
        break; // no stick with an input
              
      // does the manager accept it ?
      stick_index = new_stick_index;
      UpdateAxisAndButtons();
      if (manager->OnGamepadConnected(this))
      {
        ever_connected = true;
        break;
      }
      // manager, refused to acquire this stick, try next one
      stick_index = -1;
      start_search_index = new_stick_index + 1;
    }
  }
}

// Some explanation : considering that a gamepad represents a player,
//                    if a gamepad is disconnected, it has now greater priority for recapture that a gamepad that never has been captured
//                    (want to reconnect old player instead of introducing a new one)
//
//                    the manager has the opportunity to refuse a stick input

void MyGLFWGamepad::Tick(float delta_time, int step) 
{
  // STEP 0 : update values for connected sticks
  if (step == 0)
  {
    if (IsPresent()) // stick already present
    {
      if (!glfwJoystickPresent(stick_index))
      {
        stick_index = -1;
        manager->OnGamepadDisconnected(this);    // stick no more present => disconnection
      }
      else
        UpdateAxisAndButtons();
    }
  }
  // STEP 1 : try to recapture uncaptured stick that already have been captured once
  else if (step == 1)
  {
    if (!IsPresent())
      TryCaptureStick(true);
  }
  // STEP 2 : try to recapture never used stick
  else if (step == 2)
  {
    if (!IsPresent())
      TryCaptureStick(false);
  }
}

bool MyGLFWGamepadManager::HasAnyInputs(int stick_index, float dead_zone)
{
  if (glfwJoystickPresent(stick_index)) // ensure any input is triggered
  {
    int buttons_count = 0;
    int axis_count    = 0;

    float const * axis_buffer = glfwGetJoystickAxes(stick_index,  &axis_count);
    if (axis_buffer != nullptr)
      for (size_t i = 0 ; i < (size_t)axis_count ; ++i)
        if (axis_buffer[i] > dead_zone || axis_buffer[i] < -dead_zone) // is axis valid
          return true;

    unsigned char const * buttons_buffer = glfwGetJoystickButtons(stick_index,  &buttons_count);
    if (buttons_buffer != nullptr)
    for (size_t i = 0 ; i < (size_t)buttons_count ; ++i)
      if (buttons_buffer[i]) // is button pressed ?
        return true;
  }
  return false;
}


int MyGLFWGamepadManager::GetFreeStickIndex(int start_index) const
{
  for (int i = start_index ; i <= GLFW_JOYSTICK_LAST ; ++i)
    if (!IsStickIndexInUse(i))
      if (HasAnyInputs(i, dead_zone))
        return i;
  return -1;
}

bool MyGLFWGamepadManager::IsStickIndexInUse(int stick_index) const
{
  for (MyGLFWGamepad * gamepad : gamepads)
    if (gamepad->stick_index == stick_index)
      return true;
  return false;
}

void MyGLFWGamepadManager::Tick(float delta_time)
{
  for (int step = 0 ; step < 3 ; ++step)
    for (MyGLFWGamepad * gamepad : gamepads)
      gamepad->Tick(delta_time, step);
}

MyGLFWGamepad * MyGLFWGamepadManager::AllocateGamepad()
{
  MyGLFWGamepad * result = NewGamepadInstance();
  if (result != nullptr)
    gamepads.push_back(result);
  return result;
}

MyGLFWGamepad * MyGLFWGamepadManager::NewGamepadInstance()
{
  return new MyGLFWGamepad(this, dead_zone);
}

void MyGLFWGamepadManager::FreeGamepad(MyGLFWGamepad * gamepad)
{
  assert(gamepad != nullptr);
  assert(gamepad->manager == this);

  auto it = std::find(gamepads.begin(), gamepads.end(), gamepad);
  if (it != gamepads.end())
  {
    size_t index = &*it - &gamepads[0];
    gamepads[index] = gamepads.back();
    gamepads.pop_back();
    delete(gamepad);
  }
}

void MyGLFWGamepadManager::Reset()
{
  for (MyGLFWGamepad * gamepad : gamepads)
    if (gamepad != nullptr)
      delete(gamepad);
  gamepads.clear();
}

}; // namespace chaos
