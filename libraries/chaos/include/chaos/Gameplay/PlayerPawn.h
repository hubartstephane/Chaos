#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class PlayerPawn;

}; // namespace chaos

#else

namespace chaos
{

	// =============================================
	// PlayerPawn
	// =============================================

	class PlayerPawn : public GameEntity
	{

		DEATH_GAMEFRAMEWORK_ALLFRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(PlayerPawn, GameEntity);

	public:

		DEATH_GAMEGETTERS_DECLARE();

		/** constructor */
		PlayerPawn();

		/** initialization method */
		bool Initialize(Player* in_player);

		/** returns the player the pawn belongs to */
		chaos::AutoCastable<Player> GetPlayer() { return player; }
		/** returns the player the pawn belongs to */
		chaos::AutoConstCastable<Player> GetPlayer() const { return player; }

	protected:

		/** the player that owns this pawn */
		Player* player = nullptr;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
