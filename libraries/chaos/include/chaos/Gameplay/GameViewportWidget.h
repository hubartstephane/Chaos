namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameViewportWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API PlaceHolderWidget : public Widget
	{
		CHAOS_DECLARE_OBJECT_CLASS(PlaceHolderWidget, Widget);

	public:


		PlaceHolderWidget();

		/** override */
		virtual bool OnDraw(GPURenderer* renderer, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params) override;


		glm::vec4 clear_color;

	};





	class CHAOS_API GameViewportWidget : public Widget
	{
		friend class GameApplication;

		CHAOS_DECLARE_OBJECT_CLASS(GameViewportWidget, Widget);

	public:

		/** initialize game member */
		void SetGame(Game* in_game);

	protected:

		/** override */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** override */
		virtual bool OnMouseMoveImpl(glm::vec2 const & delta) override;
		/** override */
		virtual bool OnCharEventImpl(unsigned int c) override;
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const& key_event) override;

		/** override */
		virtual bool OnDraw(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, WindowDrawParams const& draw_params) override;

		/** override */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode) override;


#if 0

		/** override */
		virtual void OnIconifiedStateChange(bool iconified) override;
		/** override */
		virtual void OnFocusStateChange(bool gain_focus) override;
#endif

#if 0
		/** override */
		virtual aabox2 GetRequiredViewport(glm::ivec2 const& size) const override;
#endif


	protected:

		/** pointer on the game */
		shared_ptr<Game> game;
	};

#endif

}; // namespace chaos