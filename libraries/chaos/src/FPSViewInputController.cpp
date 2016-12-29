#include <chaos/FPSViewInputController.h>
#include <chaos/LogTools.h>
#include <chaos/GLTools.h>

namespace chaos
{
	void FPSViewInputController::Tick(GLFWwindow * glfw_window, double delta_time)
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




}; // namespace chaos
