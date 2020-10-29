#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GameGamepadManager;

}; // namespace chaos

#else

namespace chaos
{
	class GameGamepadManager : public chaos::MyGLFW::GamepadManager
	{
	public:

		/** the constructor */
		GameGamepadManager(Game * in_game) : game(in_game)
		{
			assert(in_game != nullptr);
		}

	protected:

		/** the gamepad manager */
		virtual bool DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad) override;

	protected:

		/** pointer on the game */
		Game * game = nullptr;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION