namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameApplication;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GameApplication : public WindowApplication
	{
	public:

		/** constructor */
		GameApplication(SubClassOf<Game> in_game_class, SubClassOf<Window> in_main_window_class, WindowParams const& in_window_params = {}, GLFWWindowHints const& in_window_hints = {});

		/** gets the game */
		Game* GetGame() { return game.get(); }
		/** gets the game */
		Game const* GetGame() const { return game.get(); }

	protected:

		/** override */
		virtual bool PreMessageLoop() override;
		/** override */
		virtual void Tick(float delta_time) override;
		/** override */
		virtual void OnWindowCreated(Window* window) override;
		/** override */
		virtual bool FillAtlasGeneratorInput(BitmapAtlas::AtlasInput& input) override;

	protected:

		/** the class for the game */
		SubClassOf<Game> game_class;
		/** pointer on the game */
		shared_ptr<Game> game;
	};

	template<typename GAME_TYPE, typename ...PARAMS>
	/*CHAOS_API*/ bool RunGame(int argc, char** argv, char** env, PARAMS && ...params)
	{
		return RunApplication<GameApplication>(argc, argv, env, GAME_TYPE::GetStaticClass(), GameWindow::GetStaticClass(), std::forward<PARAMS>(params)...);
	}

#endif

}; // namespace chaos