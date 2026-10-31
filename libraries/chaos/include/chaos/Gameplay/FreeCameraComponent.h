namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class FreeCameraComponent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =============================================
	// FreeCameraComponent
	// =============================================

	class CHAOS_API FreeCameraComponent : public CameraComponent
	{
		CHAOS_DECLARE_OBJECT_CLASS(FreeCameraComponent, CameraComponent);

	public:

		/** constructor */
		FreeCameraComponent(size_t in_player_index = 0) :
			player_index(in_player_index) {}

		/** returns the zoom value */
		float GetZoomValue() const { return zoom_value; }
		/** set the zoom value */
		void SetZoomValue(float in_zoom_value) { zoom_value = in_zoom_value; }

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual box2 ApplyModifier(box2 const& src) const override;
		/** override */
		virtual bool EnumerateInputActions(InputActionProcessor& in_action_processor, EnumerateInputActionContext in_context) override;

	protected:

		/** the player index to follow */
		size_t player_index = 0;
		/** the value of zoom to apply */
		float zoom_value = 1.0f;
		/** the camera move input value */
		glm::vec2 move_direction = {0.0f, 0.0f};
		/** the zoom in input value */
		float zoom_in = 0.0f;
		/** the zoom out input value */
		float zoom_out = 0.0f;
	};

#endif

}; // namespace chaos