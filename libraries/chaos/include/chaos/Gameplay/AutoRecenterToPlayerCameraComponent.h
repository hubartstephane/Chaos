namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class AutoRecenterToPlayerCameraComponent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =============================================
	// AutoRecenterToPlayerCameraComponent
	// =============================================

	class CHAOS_API AutoRecenterToPlayerCameraComponent : public CameraComponent
	{
		CHAOS_DECLARE_OBJECT_CLASS(AutoRecenterToPlayerCameraComponent, CameraComponent);

	public:

		/** constructor */
		AutoRecenterToPlayerCameraComponent(size_t in_player_index = 0) :
			player_index(in_player_index) {}

		/** gets the target the component wants to focus on */
		virtual box2 GetTargetBox() const;

		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const;
		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const * json);

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the player index to follow */
		size_t player_index = 0;

		/** the safe zone (inside the camera safe_zone) where the pawn must be restricted in low speed */
		SafeZone slow_safe_zone = { {0.3f, 0.3f}, {0.7f, 0.7f} };

		/** the speed at which limit updates (safe_zone size as a unit) */
		float limit_speed = 0.5f;
		/** the speed at which the camera is manually moved (safe_zone size as a unit) */
		float manual_limit_speed = 1.0f;
		/** the speed at which the camera is recentered (safe_zone size as a unit) */
		float recenter_limit_speed = 0.2f;

		/** the speed above which the speed is considered high */
		float fast_pawn_speed = 300.0f;
		/** the speed under which the speed is considered idle */
		float idle_pawn_speed = 50.0f;

		/** the delay of idle to recenter the camera to player */
		float idle_delay = 5.0f;

		/** the previous pawn position */
		glm::vec2 pawn_previous_position = { std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN() };

		/** current ratio of dynamic limits (min limits, left and down)*/
		glm::vec2 min_dynamic_safe_zone = { 0.0f, 0.0f };
		/** current ratio of dynamic limits (max limits, right and up) */
		glm::vec2 max_dynamic_safe_zone = { 0.0f, 0.0f };

		/** current idle timer value */
		float idle_timer = 0.0f;
	};

#endif

}; // namespace chaos