namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class FPSViewMovementSpeed;
	class FPSViewMovementKeyboardConfiguration;
	class FPSViewInputController;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* FPSViewMovementSpeed : a class that describes displacement speed for the FPS view
	*/

	class FPSViewMovementSpeed
	{
	public:

		/** mouse speed */
		float mouse_sensibility = 1.0f;

		/** some self descriptive speed */
		float strafe = 20.0f;
		/** some self descriptive speed */
		float forward = 20.0f;
		/** some self descriptive speed */
		float back = 20.0f;
		/** some self descriptive speed */
		float up = 20.0f;
		/** some self descriptive speed */
		float down = 20.0f;

		/** some self descriptive speed */
		float yaw = 90.0f;
		/** some self descriptive speed */
		float pitch = 90.0f;
	};

	class FPSViewMovementKeyboardConfiguration
	{
	public:

		/** self descriptive key */
		int key_left = GLFW_KEY_LEFT;
		/** self descriptive key */
		int key_right = GLFW_KEY_RIGHT;
		/** self descriptive key */
		int key_forward = GLFW_KEY_UP;
		/** self descriptive key */
		int key_backward = GLFW_KEY_DOWN;
		/** self descriptive key */
		int key_up = GLFW_KEY_PAGE_DOWN;
		/** self descriptive key */
		int key_down = GLFW_KEY_PAGE_UP;

		/** self descriptive key */
		int key_yaw_left = GLFW_KEY_KP_4;
		/** self descriptive key */
		int key_yaw_right = GLFW_KEY_KP_6;
		/** self descriptive key */
		int key_pitch_up = GLFW_KEY_KP_8;
		/** self descriptive key */
		int key_pitch_down = GLFW_KEY_KP_2;
	};

	/**
	* FPSViewInputController : an utility class to simply handle a FPS camera in a GLFW application. Handle keys ...
	*/

	class FPSViewInputController
	{
		static double constexpr INVALID_MOUSE_VALUE = std::numeric_limits<double>::max();

	public:

		/** the tick method */
		virtual void Tick(GLFWwindow* glfw_window, float delta_time);

		/** matrix getter */
		inline glm::mat4 GlobalToLocal() const { return fps_controller.GlobalToLocal(); }
		/** matrix getter */
		inline glm::mat4 LocalToGlobal() const { return fps_controller.LocalToGlobal(); }

		/** returns true whether mouse capture is enabled */
		bool IsMouseEnabled() const { return mouse_enabled; }
		/** change the mouse capture policy */
		void SetMouseEnabled(bool in_mouse_enabled);

	protected:

		/** handle the mouse displacement */
		void HandleMouseInputs(GLFWwindow* glfw_window, float delta_time);
		/** handle the keyboard inputs */
		void HandleKeyboardInputs(GLFWwindow* glfw_window, float delta_time);

		/** check whether keyboard input is down */
		bool CheckKeyInput(GLFWwindow* glfw_window, int key) const;

		/** whether the mouse is enabled or not */
		bool mouse_enabled = true;
		/** whether the mouse has been captured */
		bool mouse_captured = false;
		/** position of the mouse once captured */
		double previous_mouse_x = INVALID_MOUSE_VALUE;
		/** position of the mouse once captured */
		double previous_mouse_y = INVALID_MOUSE_VALUE;

	public:

		/** the fps matrix handler */
		FPSViewController fps_controller;
		/** the speed for the displacement */
		FPSViewMovementSpeed movement_speed;
		/** the mapping for the displacement */
		FPSViewMovementKeyboardConfiguration keyboard_config;

		/** whether we need to capture to move the camera */
		bool must_click_to_rotate = true;
	};

#endif

}; // namespace chaos