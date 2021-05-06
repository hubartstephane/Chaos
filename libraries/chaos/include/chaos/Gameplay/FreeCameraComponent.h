#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class FreeCameraComponent;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{

	// =============================================
	// FreeCameraComponent
	// =============================================

	class FreeCameraComponent : public CameraComponent
	{
		CHAOS_DECLARE_OBJECT_CLASS2(FreeCameraComponent, CameraComponent);

	public:

		/** constructor */
		FreeCameraComponent(size_t in_player_index = 0) :
			player_index(in_player_index) {}

		/** returns the zoom value */
		float GetZoomValue() const { return zoom_value; }
		/** set the zoom value */
		void SetZoomValue(float in_zoom_value){ zoom_value = in_zoom_value; }

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual box2 ApplyModifier(box2 const & src) const override;

	protected:

		/** the player index to follow */
		size_t player_index = 0;
		/** the value of zoom to apply */
		float zoom_value = 1.0f;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION