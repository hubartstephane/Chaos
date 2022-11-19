namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameWindowClient;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GameWindowClient : public WindowClient
	{
		friend class GameApplication;

		CHAOS_DECLARE_OBJECT_CLASS(GameWindowClient, WindowClient);

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
		virtual bool OnDraw(GPURenderer* renderer, WindowDrawParams const & draw_params, GPUProgramProviderInterface const * uniform_provider) override;

		/** override */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode) override;
		/** override */
		virtual void OnIconifiedStateChange(bool iconified) override;
		/** override */
		virtual void OnFocusStateChange(bool gain_focus) override;

#if 0
		/** override */
		virtual ViewportPlacement GetRequiredViewport(glm::ivec2 const& size) const override;
#endif


	protected:

		/** pointer on the game */
		shared_ptr<Game> game;
	};

#endif

}; // namespace chaos