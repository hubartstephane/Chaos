namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class GameWindow : public Window
	{
		friend class GameApplication;

		CHAOS_DECLARE_OBJECT_CLASS(GameWindow, Window);

	protected:

		/** initialize game member */
		void SetGame(Game* in_game);

		/** override */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** override */
		virtual bool OnMouseMoveImpl(double x, double y) override;
		/** override */
		virtual bool OnCharEventImpl(unsigned int c) override;
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const& event) override;

		/** override */
		virtual bool OnDraw(GPURenderer* renderer, box2 const& viewport, glm::ivec2 window_size) override;
		/** override */
		virtual void Finalize() override;
		/** override */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode) override;
		/** override */
		virtual void OnIconifiedStateChange(bool iconified) override;
		/** override */
		virtual void OnFocusStateChange(bool gain_focus) override;
		/** override */
		virtual box2 GetRequiredViewport(glm::ivec2 const& size) const override;


	protected:

		/** pointer on the game */
		shared_ptr<Game> game;
	};

#endif

}; // namespace chaos