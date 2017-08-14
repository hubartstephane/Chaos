#include <chaos/FPSViewInputController.h>
#include <chaos/LogTools.h>
#include <chaos/GLTools.h>

namespace chaos
{
  double const FPSViewInputController::INVALID_MOUSE_VALUE = std::numeric_limits<double>::max();



  void FPSViewInputController::SetMouseEnabled(bool in_mouse_enabled)
  {
    if (mouse_enabled == in_mouse_enabled)
      return;

    mouse_captured = false;
    mouse_enabled = in_mouse_enabled;
  }


  void FPSViewInputController::Tick(GLFWwindow * glfw_window, double delta_time)
  {
    HandleMouseInputs(glfw_window, delta_time);
    HandleKeyboardInputs(glfw_window, delta_time);
  }

  void FPSViewInputController::HandleMouseInputs(GLFWwindow * glfw_window, double delta_time)
  {
    if (!IsMouseEnabled())
      return;

    // handle mouse movement
    if (mouse_captured || !must_click_to_rotate)
    {
      if (must_click_to_rotate && glfwGetMouseButton(glfw_window, 0) == GLFW_RELEASE)
        mouse_captured = false;
      else
      {
        double new_mouse_x = 0.0;
        double new_mouse_y = 0.0;
        glfwGetCursorPos(glfw_window, &new_mouse_x, &new_mouse_y);

        if (previous_mouse_x != INVALID_MOUSE_VALUE && previous_mouse_y != INVALID_MOUSE_VALUE) // only want to rotate if we have a valid mouse position
        {
          if (new_mouse_x != previous_mouse_x)
            fps_controller.IncrementYaw(-(float)((new_mouse_x - previous_mouse_x) * movement_speed.mouse_sensibility));
          if (new_mouse_y != previous_mouse_y)
            fps_controller.IncrementPitch(-(float)((new_mouse_y - previous_mouse_y) * movement_speed.mouse_sensibility));
        }
        previous_mouse_x = new_mouse_x;
        previous_mouse_y = new_mouse_y;
      }
    }
    else
    {
      if (glfwGetMouseButton(glfw_window, 0) == GLFW_PRESS)
      {
        mouse_captured = true;
        previous_mouse_x = INVALID_MOUSE_VALUE;
        previous_mouse_y = INVALID_MOUSE_VALUE;
        glfwGetCursorPos(glfw_window, &previous_mouse_x, &previous_mouse_y);
      }
    }
  }

  void FPSViewInputController::HandleKeyboardInputs(GLFWwindow * glfw_window, double delta_time)
  {
    float dt = (float)delta_time;

    // handles the keys for displacement
    if (CheckKeyInput(glfw_window, keyboard_config.key_left))
      fps_controller.StrafeLeft(movement_speed.strafe * dt);
    if (CheckKeyInput(glfw_window, keyboard_config.key_right))
      fps_controller.StrafeRight(movement_speed.strafe * dt);
    if (CheckKeyInput(glfw_window, keyboard_config.key_forward))
      fps_controller.GoForward(movement_speed.forward * dt);
    if (CheckKeyInput(glfw_window, keyboard_config.key_backward))
      fps_controller.GoBackward(movement_speed.back * dt);
    if (CheckKeyInput(glfw_window, keyboard_config.key_up))
      fps_controller.position.y -= movement_speed.down * dt;
    if (CheckKeyInput(glfw_window, keyboard_config.key_down))
      fps_controller.position.y += movement_speed.up * dt;

    if (CheckKeyInput(glfw_window, keyboard_config.key_yaw_left))
      fps_controller.IncrementYaw(movement_speed.yaw * dt);
    if (CheckKeyInput(glfw_window, keyboard_config.key_yaw_right))
      fps_controller.IncrementYaw(-movement_speed.yaw * dt);

    if (CheckKeyInput(glfw_window, keyboard_config.key_pitch_up))
      fps_controller.IncrementPitch(movement_speed.pitch * dt);
    if (CheckKeyInput(glfw_window, keyboard_config.key_pitch_down))
      fps_controller.IncrementPitch(-movement_speed.pitch * dt);
  }

  bool FPSViewInputController::CheckKeyInput(GLFWwindow * glfw_window, int key) const
  {
    return (key >= 0 && glfwGetKey(glfw_window, key) == GLFW_PRESS);
  }

}; // namespace chaos
