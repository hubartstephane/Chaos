#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GameApplication;

}; // namespace chaos

#else

namespace chaos
{
	class GameApplication : public WindowApplication
	{
	public:

		/** constructor */
		GameApplication(SubClassOf<Game> in_game_class, SubClassOf<Window> in_main_window_class, WindowParams const& in_window_params, WindowHints const in_window_hints);

	protected:

		/** override */
		virtual bool PreMainLoop() override;
		/** override */
		virtual void Tick(float delta_time) override;
		/** override */
		virtual void OnWindowCreated(Window* window) override;

	protected:

		/** the class for the game */
		SubClassOf<Game> game_class;
		/** pointer on the game */
		shared_ptr<Game> game;
	};

	template<typename GAME_TYPE, typename ...PARAMS>
	bool RunGame(int argc, char** argv, char** env, PARAMS... params)
	{
		return RunApplication<GameApplication>(argc, argv, env, GAME_TYPE::GetStaticClass(), GameWindow::GetStaticClass(), params...);
	}

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION