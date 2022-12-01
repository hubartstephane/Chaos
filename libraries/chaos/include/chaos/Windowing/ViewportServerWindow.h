namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportServerWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ViewportServerWindow : a window that may receive one viewport
	*/

	class CHAOS_API ViewportServerWindow : public Window, public ViewportServerInterface
	{
		CHAOS_DECLARE_OBJECT_CLASS(ViewportServerWindow, Window);

		friend class Viewport;

	public:

		/** get current viewport */
		AutoCastable<Viewport> GetViewport();
		/** get current viewport */
		AutoConstCastable<Viewport> GetViewport() const;
		/** set current viewport */
		void SetViewport(Viewport* in_viewport);

	protected:

		/** override */
		virtual bool OnWindowClosed() override;
		/** override */
		virtual void OnWindowResize(glm::ivec2 size) override;
		/** override */
		virtual void OnDropFile(int count, char const** paths) override;
		/** override */
		virtual void OnIconifiedStateChange(bool iconified) override;
		/** override */
		virtual void OnFocusStateChange(bool gain_focus) override;
		/** override */
		virtual bool OnDraw(GPURenderer* renderer, WindowDrawParams const& DrawParams, GPUProgramProviderInterface const* uniform_provider) override;

		/** override */
		virtual bool OnMouseMoveImpl(double x, double y) override;
		/** override */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** override */
		virtual bool OnMouseWheelImpl(double scroll_x, double scroll_y) override;
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const& event) override;
		/** override */
		virtual bool OnCharEventImpl(unsigned int c) override;

		/** override */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;

		/** override */
		virtual bool DoTick(float delta_time) override;

		/** update viewport size according the window size */
		void UpdateViewportPlacement();
	};

#endif

}; // namespace chaos