#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FPSViewController.h>

namespace chaos
{

  /**
   * MyGLFWFpsCamera : an utility class to simply handle a FPS camera in a GLFW application. Handle keys ...
   */

class MyGLFWFpsCamera
{
public:

  /** constructor */
  MyGLFWFpsCamera();

  /** the tick method */
  virtual void Tick(GLFWwindow * glfw_window, double delta_time);
  /** the mouse button method */
  virtual void OnMouseButton(GLFWwindow * glfw_window, int button, int action, int modifier);
  /** the mouse button method */
  virtual void OnMouseMove(GLFWwindow * glfw_window, double x, double y);

  /** matrix getter */
  inline glm::mat4 GlobalToLocal() const { return fps_controller.GlobalToLocal(); }
  /** matrix getter */
  inline glm::mat4 LocalToGlobal() const { return fps_controller.LocalToGlobal(); }

protected:

  /** whether the mouse has been captured */
  bool mouse_captured;
  /** position of the mouse once captured */
  double previous_mouse_x;
  /** position of the mouse once captured */
  double previous_mouse_y;

public:

  /** the fps matrix handler */
  chaos::FPSViewController fps_controller;

  /** some self descriptive speed */
  float strafe_speed;
  /** some self descriptive speed */
  float goforward_speed;
  /** some self descriptive speed */
  float goback_speed;
  /** some self descriptive speed */
  float goup_speed;
  /** some self descriptive speed */
  float godown_speed;

  /** mouse speed */
  float mouse_sensibility;
};

}; // namespace chaos

