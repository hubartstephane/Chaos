#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class FollowPlayerCameraComponent;

}; // namespace chaos

#else

namespace chaos
{

	// =============================================
	// FollowPlayerCameraComponent
	// =============================================

	class FollowPlayerCameraComponent : public CameraComponent
	{
		CHAOS_DECLARE_OBJECT_CLASS2(FollowPlayerCameraComponent, CameraComponent);

	public:

		/** constructor */
		FollowPlayerCameraComponent(size_t in_player_index = 0) :
			player_index(in_player_index) {}

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the player index to follow */
		size_t player_index = 0;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION