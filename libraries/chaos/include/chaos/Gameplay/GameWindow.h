#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GameWindow;

	template<typename GAME_TYPE>
	class TypedGameWindow;

}; // namespace chaos

#else

namespace chaos
{
	class GameWindow : public MyGLFW::Window
	{
		CHAOS_DECLARE_OBJECT_CLASS2(GameWindow, MyGLFW::Window);

	protected:

		/** game instance creation + initialization */
		virtual Game * CreateGame();
		/** game instance creation */
		virtual Game* DoCreateGame();

		/** override */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** override */
		virtual bool OnMouseMoveImpl(double x, double y) override;
		/** override */
		virtual bool OnCharEventImpl(unsigned int c) override;
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const & event) override;

		/** override */
		virtual bool OnDraw(GPURenderer * renderer, box2 const & viewport, glm::ivec2 window_size) override;
		/** override */
		virtual void Finalize() override;
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;
		/** override */
		virtual void TweakHints(MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override;
		/** override */
		virtual bool Tick(float delta_time) override;
		/** override */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode) override;
		/** override */
		virtual void OnIconifiedStateChange(bool iconified) override;
		/** override */
		virtual void OnFocusStateChange(bool gain_focus) override;
		/** override */
		virtual box2 GetRequiredViewport(glm::ivec2 const & size) const override;

	protected:

		/** pointer on the game */
		shared_ptr<Game> game;
	};


	template<typename GAME_TYPE>
	class TypedGameWindow : public GameWindow
	{
		CHAOS_DECLARE_TEMPLATE_OBJECT_CLASS2(TypedGameWindow, GameWindow);

	protected:

		/** override */
		virtual Game * DoCreateGame() override
		{
			return new GAME_TYPE();
		}
	};

}; // namespace chaos


#endif // CHAOS_FORWARD_DECLARATION