#include <chaos/MyGLFWFpsCamera.h>
#include <chaos/LogTools.h>
#include <chaos/GLTools.h>

namespace chaos
{

  MyGLFWFpsCamera::MyGLFWFpsCamera() :
    mouse_captured(false),
    strafe_speed(20.0f),
    goforward_speed(20.0f),
    goback_speed(20.0f),
    goup_speed(20.0f),
    godown_speed(20.0f),
    mouse_sensibility(1.0f)
  {

  }

  void MyGLFWFpsCamera::Tick(GLFWwindow * glfw_window, double delta_time)
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_LEFT) == GLFW_PRESS)
      fps_controller.StrafeLeft(strafe_speed * (float)delta_time);
    if (glfwGetKey(glfw_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      fps_controller.StrafeRight(strafe_speed * (float)delta_time);
    if (glfwGetKey(glfw_window, GLFW_KEY_UP) == GLFW_PRESS)
      fps_controller.GoForward(goforward_speed * (float)delta_time);
    if (glfwGetKey(glfw_window, GLFW_KEY_DOWN) == GLFW_PRESS)
      fps_controller.GoBackward(goback_speed * (float)delta_time);
    if (glfwGetKey(glfw_window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
      fps_controller.position.y -= godown_speed * (float)delta_time;
    if (glfwGetKey(glfw_window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
      fps_controller.position.y += goup_speed * (float)delta_time;
  }

  void MyGLFWFpsCamera::OnMouseButton(GLFWwindow * glfw_window, int button, int action, int modifier)
  {
    if (action == GLFW_RELEASE)
      mouse_captured = false;
    else if (action == GLFW_PRESS)
    {
      mouse_captured = true;
      glfwGetCursorPos(glfw_window, &previous_mouse_x, &previous_mouse_y);
    }
  }

  void MyGLFWFpsCamera::OnMouseMove(GLFWwindow * glfw_window, double x, double y)
  {
    if (!mouse_captured)
      return;

    if (glfwGetMouseButton(glfw_window, 0) == GLFW_PRESS)
    {
      if (x != previous_mouse_x)
        fps_controller.IncrementYaw(-(float)((x - previous_mouse_x) * mouse_sensibility));
      if (y != previous_mouse_y)
        fps_controller.IncrementPitch(-(float)((y - previous_mouse_y) * mouse_sensibility));
    }
    previous_mouse_x = x;
    previous_mouse_y = y;
  }

}; // namespace chaos
