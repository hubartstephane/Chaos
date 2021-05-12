namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameGamepadManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class GameGamepadManager : public GamepadManager
	{
	public:

		/** the constructor */
		GameGamepadManager(Game* in_game) : game(in_game)
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