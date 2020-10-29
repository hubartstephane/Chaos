#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ShakeCameraComponent;

}; // namespace chaos

#else

namespace chaos
{
	// =============================================
	// ShakeCameraComponent
	// =============================================

	class ShakeCameraComponent : public CameraComponent
	{
	public:

		/** constructor */
		ShakeCameraComponent(float in_modifier_duration, float in_modifier_range, float in_modifier_frequency, bool in_use_damping, bool in_zoom_effect):
			modifier_duration(in_modifier_duration),
			modifier_range(in_modifier_range),
			modifier_frequency(in_modifier_frequency),
			use_damping(in_use_damping),
			zoom_effect(in_zoom_effect){}

		/** override */
		virtual chaos::box2 ApplyModifier(chaos::box2 const & src) const override;

		/** the modifier is restarted */
		void RestartModifier();
		/** stop the modifier */
		void StopModifier();

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the duration of the shake effect (before the range fallbacks to 0) */
		float modifier_duration = 1.0f;
		/** the amplitude of the shake effet */
		float modifier_range = 1.0f;
		/** the frequency of the shake effect */
		float modifier_frequency = 0.1f;
		/** whether there is a damping effect */
		bool use_damping = true;
		/** whether to modify zoom instead of position */
		bool zoom_effect = true;

		/** the time of the effect */
		float current_time = -1.0f;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
