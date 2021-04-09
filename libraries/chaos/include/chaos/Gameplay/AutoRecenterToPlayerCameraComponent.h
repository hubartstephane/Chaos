#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class AutoRecenterToPlayerCameraComponent;

}; // namespace chaos

#else

namespace chaos
{

	// =============================================
	// AutoRecenterToPlayerCameraComponent
	// =============================================

	class AutoRecenterToPlayerCameraComponent : public CameraComponent
	{
		CHAOS_DECLARE_OBJECT_CLASS2(AutoRecenterToPlayerCameraComponent, CameraComponent);

	public:

		/** constructor */
		AutoRecenterToPlayerCameraComponent(size_t in_player_index = 0) :
			player_index(in_player_index) {}
		AutoRecenterToPlayerCameraComponent(size_t in_player_index, float in_recenter_speed, float in_idle_delay) :
			player_index(in_player_index),
			recenter_speed(in_recenter_speed),
			idle_delay(in_idle_delay){}

		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const;
		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json_entry);

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the player index to follow */
		size_t player_index = 0;
		/** the maximum speed for recentering to player */
		float recenter_speed = 5.0f;
		/** the delay of idle to recenter the camera to player */
		float idle_delay = 5.0f;

		/** current idle timer value */
		float idle_timer = 0.0f;

	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION