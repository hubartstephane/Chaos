namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WindowDrawParams;
	class WindowInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ========================================================
	// WindowDrawParams : some data for the window rendering
	// ========================================================

	class CHAOS_API WindowDrawParams
	{
	public:
		/** the viewport */
		ViewportPlacement viewport;
		/** the rendering full size */
		glm::ivec2 full_size = { 0, 0 };
	};

	// ========================================================
	// WindowInterface : a common interface for both window and window client
	// ========================================================

	class CHAOS_API WindowInterface : public TickableInterface, public InputEventReceiverInterface, public NamedInterface, public GPUProgramProviderInterface
	{
	protected:

		/** called whenever the user try to close window */
		virtual bool OnWindowClosed() { return true; }
		/** called whenever the window is resized */
		virtual void OnWindowResize(glm::ivec2 size) {}
		/** called whenever a file is dropped */
		virtual void OnDropFile(int count, char const** paths) {}
		/** called whenever the window becomes iconified or is restored */
		virtual void OnIconifiedStateChange(bool iconified) {}
		/** called whenever the window gain or loose focus */
		virtual void OnFocusStateChange(bool gain_focus) {}
		/** the drawing specialization method */
		virtual bool OnDraw(GPURenderer* renderer, WindowDrawParams const& DrawParams, GPUProgramProviderInterface const* uniform_provider) { return true; }
	};

#endif

}; // namespace chaos