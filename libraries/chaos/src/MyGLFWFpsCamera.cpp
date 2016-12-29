#include <chaos/MyGLFWFpsCamera.h>
#include <chaos/LogTools.h>
#include <chaos/GLTools.h>

namespace chaos
{
	void MyGLFWFpsCamera::Tick(GLFWwindow * glfw_window, double delta_time)
	{
		// handle mouse movement
		if (mouse_captured)
		{
			if (glfwGetMouseButton(glfw_window, 0) == GLFW_RELEASE)
				mouse_captured = false;
			else
			{
				double new_mouse_x = 0.0;
				double new_mouse_y = 0.0;
				glfwGetCursorPos(glfw_window, &new_mouse_x, &new_mouse_y);
			
				if (new_mouse_x != previous_mouse_x)
					fps_controller.IncrementYaw(-(float)((new_mouse_x - previous_mouse_x) * mouse_sensibility));
				if (new_mouse_y != previous_mouse_y)
					fps_controller.IncrementPitch(-(float)((new_mouse_y - previous_mouse_y) * mouse_sensibility));

				previous_mouse_x = new_mouse_x;
				previous_mouse_y = new_mouse_y;
			}
		}
		else
		{
			if (glfwGetMouseButton(glfw_window, 0) == GLFW_PRESS)
			{
				mouse_captured = true;
				glfwGetCursorPos(glfw_window, &previous_mouse_x, &previous_mouse_y);				
			}						
		}

		// handles the keys for displacement
		if (glfwGetKey(glfw_window, GLFW_KEY_LEFT) == GLFW_PRESS)
			fps_controller.StrafeLeft(movement_speed.strafe * (float)delta_time);
		if (glfwGetKey(glfw_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			fps_controller.StrafeRight(movement_speed.strafe * (float)delta_time);
		if (glfwGetKey(glfw_window, GLFW_KEY_UP) == GLFW_PRESS)
			fps_controller.GoForward(movement_speed.forward * (float)delta_time);
		if (glfwGetKey(glfw_window, GLFW_KEY_DOWN) == GLFW_PRESS)
			fps_controller.GoBackward(movement_speed.back * (float)delta_time);
		if (glfwGetKey(glfw_window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
			fps_controller.position.y -= movement_speed.down * (float)delta_time;
		if (glfwGetKey(glfw_window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
			fps_controller.position.y += movement_speed.up * (float)delta_time;
	}

	void MyGLFWFpsCamera::OnMouseButton(GLFWwindow * glfw_window, int button, int action, int modifier)
	{
#if 0
		if (action == GLFW_RELEASE)
			mouse_captured = false;
		else if (action == GLFW_PRESS)
		{
			mouse_captured = true;
			glfwGetCursorPos(glfw_window, &previous_mouse_x, &previous_mouse_y);
		}
#endif
	}

	void MyGLFWFpsCamera::OnMouseMove(GLFWwindow * glfw_window, double x, double y)
	{
#if 0
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
#endif
	}

}; // namespace chaos
