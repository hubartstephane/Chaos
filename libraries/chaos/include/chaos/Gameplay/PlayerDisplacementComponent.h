#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class PlayerDisplacementComponent;

}; // namespace chaos

#else

namespace chaos
{
	class PlayerDisplacementComponent : public chaos::Tickable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(PlayerDisplacementComponent, chaos::Tickable);

	public:

		DEATH_GAMEGETTERS_DECLARE();

		/** Initialization method */
		bool Initialize(Player* in_player);

		/** returns the player the component belongs to */
		chaos::AutoCastable<Player> GetPlayer() { return player; }
		/** returns the player the component belongs to */
		chaos::AutoConstCastable<Player> GetPlayer() const { return player; }

	protected:

		/** the player that owns this component */
		Player* player = nullptr;

	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION