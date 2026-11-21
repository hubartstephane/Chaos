namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ForceFeedbackMotorValues;
	class ForceFeedbackEffect;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ForceFeedbackMotorValues: get the motor values for forcefeedback effects
	 */

	class CHAOS_API ForceFeedbackMotorValues
	{
	public:

		/** the left engine of the gamepad ('shocks') */
		float left_value = 0.0f;
		/** the right engine of the gamepad ('vibration') */
		float right_value = 0.0f;
	};

	/**
	 * ForceFeedbackEffect : the feedback effects
	 */

	class CHAOS_API ForceFeedbackEffect : public Object, public SupervisableInterface
	{
		friend class Gamepad;

	public:

		/** destructor */
		~ForceFeedbackEffect();

		/** remove the forcefeedback from its gamepad */
		void RemoveFromGamepad();

	protected:

		/** override */
		virtual void RemoveFromSuperviser() override;

		/** returns the values for left and right motors. returns true whether the effect is to be finished */
		virtual bool TickAndGetMotorValues(float delta_time, ForceFeedbackMotorValues& out_motor_values);

	protected:

		/** the gamepad using this effect */
		Gamepad* gamepad = nullptr;
	};

#endif

}; // namespace chaos