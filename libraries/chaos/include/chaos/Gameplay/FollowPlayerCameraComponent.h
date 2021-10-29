namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class FollowPlayerCameraComponent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =============================================
	// FollowPlayerCameraComponent
	// =============================================

	class FollowPlayerCameraComponent : public CameraComponent
	{
		CHAOS_DECLARE_OBJECT_CLASS(FollowPlayerCameraComponent, CameraComponent);

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

#endif

}; // namespace chaos