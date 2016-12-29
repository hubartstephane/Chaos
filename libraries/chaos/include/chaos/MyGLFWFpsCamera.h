#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FPSViewController.h>

namespace chaos
{

	/**
	* FPSViewMovementSpeed : a class that describes displacement speed for the FPS view
	*/

	class FPSViewMovementSpeed
	{		
	public:

		/** some self descriptive speed */
		float strafe{20.0f};
		/** some self descriptive speed */
		float forward{20.0f};
		/** some self descriptive speed */
		float back{20.0f};
		/** some self descriptive speed */
		float up{20.0f};
		/** some self descriptive speed */
		float down{20.0f};	
	};

	/**
	* MyGLFWFpsCamera : an utility class to simply handle a FPS camera in a GLFW application. Handle keys ...
	*/

	class MyGLFWFpsCamera
	{
	public:

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
		bool mouse_captured{false};
		/** position of the mouse once captured */
		double previous_mouse_x{0.0};
		/** position of the mouse once captured */
		double previous_mouse_y{0.0};

	public:

		/** the fps matrix handler */
		FPSViewController fps_controller;
		/** the speed for the displacement */
		FPSViewMovementSpeed movement_speed;
		/** mouse speed */
		float mouse_sensibility{1.0f};
	};

}; // namespace chaos

