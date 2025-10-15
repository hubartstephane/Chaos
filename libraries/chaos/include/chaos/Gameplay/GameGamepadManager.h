namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameGamepadManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GameGamepadManager : public GamepadManager
	{
	public:

		/** the constructor */
		GameGamepadManager(Game* in_game, float in_dead_zone = 0.4f, float in_max_zone = 0.9f): 
			GamepadManager(in_dead_zone, in_max_zone),
			game(in_game)
		{
			assert(in_game != nullptr);
		}

	protected:

		/** the gamepad manager */
		virtual bool DoPoolGamepad(PhysicalGamepad* physical_gamepad) override;

	protected:

		/** pointer on the game */
		Game* game = nullptr;
	};

#endif

}; // namespace chaos