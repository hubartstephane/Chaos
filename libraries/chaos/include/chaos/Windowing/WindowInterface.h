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
		WidgetPlacement viewport;
		/** the rendering full size */
		glm::ivec2 full_size = { 0, 0 };
	};

	// ========================================================
	// WindowInterface : a common interface for both window and window client
	// ========================================================

	class CHAOS_API WindowInterface : public TickableInterface, public InputEventReceiverInterface, public NamedInterface, public GPUProgramProviderInterface
	{
	protected:

		/** the drawing specialization method */
		virtual bool OnDraw(GPURenderer* renderer, WindowDrawParams const& draw_params, GPUProgramProviderInterface const* uniform_provider) { return true; }
	};

#endif

}; // namespace chaos