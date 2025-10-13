namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class FPSViewControllerConfiguration;
	class FPSViewControllerInputConfiguration;
	class FPSViewController;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* FPSViewControllerConfiguration: a class that describes displacement speed for the FPS view
	*/

	class CHAOS_API FPSViewControllerConfiguration
	{
	public:

		/** mouse speed */
		float mouse_sensibility = 1.0f;

		/** some self descriptive speed */
		float strafe_speed = 20.0f;
		/** some self descriptive speed */
		float forward_speed = 20.0f;
		/** some self descriptive speed */
		float back_speed = 20.0f;
		/** some self descriptive speed */
		float up_speed = 20.0f;
		/** some self descriptive speed */
		float down_speed = 20.0f;

		/** some self descriptive speed */
		float yaw_speed = 90.0f;
		/** some self descriptive speed */
		float pitch_speed = 90.0f;

		/** whether we need to capture to move the camera */
		bool must_click_to_rotate = true;
	};

	/**
	* FPSViewControllerInputConfiguration: a class that describes which keys are used for the inputs
	*/

	class CHAOS_API FPSViewControllerInputConfiguration
	{
	public:

		/** self descriptive key */
		Key left_button = Key::LEFT;
		/** self descriptive key */
		Key right_button = Key::RIGHT;
		/** self descriptive key */
		Key forward_button = Key::UP;
		/** self descriptive key */
		Key backward_button = Key::DOWN;
		/** self descriptive key */
		Key up_button = Key::PAGE_UP;
		/** self descriptive key */
		Key down_button = Key::PAGE_DOWN;

		/** self descriptive key */
		Key yaw_left_button = Key::KP_4;
		/** self descriptive key */
		Key yaw_right_button = Key::KP_6;
		/** self descriptive key */
		Key pitch_up_button = Key::KP_8;
		/** self descriptive key */
		Key pitch_down_button = Key::KP_2;

		/** self descriptive key */
		Key rotation_button = Key::MOUSE_BUTTON_1;
	};

	/**
	* FPSViewController : an utility class to simply handle a FPS camera in a GLFW application. Handle keys ...
	*/

	class CHAOS_API FPSViewController : public InputReceiverInterface
	{
	public:

		/** matrix getter */
		inline glm::mat4 GlobalToLocal() const { return fps_view.GlobalToLocal(); }
		/** matrix getter */
		inline glm::mat4 LocalToGlobal() const { return fps_view.LocalToGlobal(); }

		/** returns true whether mouse capture is enabled */
		bool IsMouseEnabled() const { return mouse_enabled; }
		/** change the mouse capture policy */
		void SetMouseEnabled(bool in_mouse_enabled);

		/** override */
		virtual bool EnumerateInputActions(InputActionEnumerator & in_action_enumerator, EnumerateInputActionContext in_context) override;

	protected:

		/** override */
		virtual bool OnMouseMoveImpl(glm::vec2 const& delta) override;

	public:

		/** the fps matrix handler */
		FPSView fps_view;
		/** the speed for the displacement */
		FPSViewControllerConfiguration config;
		/** the mapping for the displacement */
		FPSViewControllerInputConfiguration input_config;

	protected:

		/** whether the mouse is enabled or not */
		bool mouse_enabled = true;
		/** whether the mouse has been captured */
		bool mouse_captured = false;
	};

#endif

}; // namespace chaos