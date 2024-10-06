namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ShakeCameraComponent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =============================================
	// ShakeCameraComponent
	// =============================================

	class CHAOS_API ShakeCameraComponent : public CameraComponent
	{
		CHAOS_DECLARE_OBJECT_CLASS(ShakeCameraComponent, CameraComponent);

	public:

		/** constructor */
		ShakeCameraComponent() = default;
		/** constructor */
		ShakeCameraComponent(float in_modifier_duration, float in_modifier_range, float in_modifier_frequency, bool in_use_damping, bool in_zoom_effect) :
			modifier_duration(in_modifier_duration),
			modifier_range(in_modifier_range),
			modifier_frequency(in_modifier_frequency),
			use_damping(in_use_damping),
			zoom_effect(in_zoom_effect) {}

		/** the modifier is restarted */
		void RestartModifier();
		/** stop the modifier */
		void StopModifier();

		/** override */
		virtual box2 ApplyModifier(box2 const& src) const override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const;
		/** override */
		virtual bool SerializeFromJSON(JSONReadConfiguration config);

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

#endif

}; // namespace chaos