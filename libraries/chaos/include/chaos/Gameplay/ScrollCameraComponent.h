#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ScrollCameraComponent;

}; // namespace chaos

#else

namespace chaos
{

	// =============================================
	// ScrollCameraComponent
	// =============================================

	class ScrollCameraComponent : public CameraComponent
	{
		CHAOS_DECLARE_OBJECT_CLASS2(ScrollCameraComponent, CameraComponent);

	public:

		/** constructor */
		ScrollCameraComponent() = default;
		/** constructor */
		ScrollCameraComponent(float in_scroll_speed, Axis in_axis):
			scroll_speed(in_scroll_speed),
			axis(in_axis){}

		/** get the scroll speed */
		float GetScrollSpeed() const { return scroll_speed;	}
		/** set the scroll speed */
		void SetScrollSpeed(float in_scroll_speed) { scroll_speed = in_scroll_speed; }
		/** get the axis */
		Axis GetAxis() const { return axis; }

		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const;
		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json_entry);

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the displacement speed */
		float scroll_speed = 0.0f;
		/** the axis of displacement */
		Axis axis = Axis::AXIS_X;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
