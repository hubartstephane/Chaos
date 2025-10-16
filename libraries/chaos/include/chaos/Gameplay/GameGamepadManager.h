namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameGamepadManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GameGamepadManager: specialization of GamepadManager that can poll input into game 
	 */

	class CHAOS_API GameGamepadManager : public GamepadManager
	{
	public:

		/** constructor */
		GameGamepadManager(Game* in_game, GamepadInputFilterSettings const& in_gamepad_filter_settings = {});

	protected:

		/** the gamepad manager */
		virtual bool DoPollGamepad(PhysicalGamepad* physical_gamepad) override;

	protected:

		/** pointer on the game */
		Game* game = nullptr;
	};

#endif

}; // namespace chaos