#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class PlayerPawn;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{

	// =============================================
	// PlayerPawn
	// =============================================

	class PlayerPawn : public GameEntity
	{

		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(PlayerPawn, GameEntity);

	public:

		CHAOS_DECLARE_GAMEPLAY_GETTERS();

		/** constructor */
		PlayerPawn();

		/** initialization method */
		bool Initialize(Player* in_player);

		/** returns the player the pawn belongs to */
		AutoCastable<Player> GetPlayer() { return player; }
		/** returns the player the pawn belongs to */
		AutoConstCastable<Player> GetPlayer() const { return player; }

	protected:

		/** the player that owns this pawn */
		Player* player = nullptr;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
