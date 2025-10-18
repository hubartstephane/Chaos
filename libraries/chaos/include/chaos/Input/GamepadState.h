namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GamepadState;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GamepadState: the gamepad buttons and axes states
	*/

	class CHAOS_API GamepadState : public InputDeviceInterface
	{
		/** number of axes in a gamepad */
		static constexpr size_t AXIS_COUNT = sizeof(meta::FakeInstance<GLFWgamepadstate>().axes) / sizeof(meta::FakeInstance<GLFWgamepadstate>().axes[0]);
		/** number of buttons in a gamepad (beware this does not take TRIGGER_LEFT/TRIGGER_RIGHT virtual buttons into account) */
		static constexpr size_t BUTTON_COUNT = sizeof(meta::FakeInstance<GLFWgamepadstate>().buttons) / sizeof(meta::FakeInstance<GLFWgamepadstate>().buttons[0]);
		/** number of sticks in gamepad */
		static constexpr size_t STICK_COUNT = 2; // see Input2D

	public:

		/** returns the number of buttons */
		size_t GetButtonCount() const;
		/** returns the number of axis */
		size_t GetAxisCount() const;

		/** update all the values for the axis and buttons */
		void UpdateAxisAndButtons(int stick_index, GamepadInputFilterSettings const& in_filter_settings);
		/** reset the content of the object */
		void Clear();

	protected:

		/** override */
		virtual KeyState const * DoGetInputState(Key input) const override;
		/** override */
		virtual Input1DState const * DoGetInputState(Input1D input) const override;
		/** override */
		virtual Input2DState const * DoGetInputState(Input2D input) const override;

		/** override */
		virtual bool DoForAllKeys(ForAllKeysFunction func) const override;
		/** override */
		virtual bool DoForAllInput1D(ForAllInput1DFunction func) const override;
		/** override */
		virtual bool DoForAllInput2D(ForAllInput2DFunction func) const override;

		/** apply dead_zone and max_zone. renormalize input */
		float ClampAndNormalizeInput1D(float value, GamepadInputFilterSettings const& in_filter_settings) const;
		/** apply dead_zone and max_zone. renormalize input */
		glm::vec2 ClampAndNormalizeInput2D(glm::vec2 value, GamepadInputFilterSettings const& in_filter_settings) const;
		/** try to snap stick angle to some sector boundaries (X & Y directions). Angle must be in [0, 2.pi] */
		float SnapInput2DAngleToSectorBoundaries(float angle, GamepadInputFilterSettings const& in_filter_settings) const;

	protected:

		/** the state for axis */
		std::array<Input1DState, AXIS_COUNT> axes;
		/** the state for buttons */
		std::array<KeyState, BUTTON_COUNT + 2> buttons; // +2 for LEFT_TRIGGER/RIGHT_TRIGGER virtual buttons
		/** the state for sticks */
		std::array<Input2DState, STICK_COUNT> sticks;
	};

#endif

}; // namespace chaos