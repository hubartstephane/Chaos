namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * DefaultForceFeedbackEffect : a simple constant effect with a timer
	 */

	class CHAOS_API DefaultForceFeedbackEffect : public ForceFeedbackEffect
	{
		CHAOS_GAMEPAD_ALL_FRIENDS

	public:

		/** constructor */
		DefaultForceFeedbackEffect(float in_duration, ForceFeedbackMotorValues const & in_motor_values):
			duration(in_duration),
			motor_values(in_motor_values){}

	protected:

		/** override */
		virtual bool TickAndGetMotorValues(float delta_time, ForceFeedbackMotorValues & out_motor_values) override;

	protected:

		/** the duration of the effect */
		float duration = 0.0f;
		/** the remaining time */
		float timer = 0.0f;
		/** the motor values */
		ForceFeedbackMotorValues motor_values;
	};

#endif

}; // namespace chaos