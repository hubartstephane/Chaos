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
		aabox2 viewport;
	};

	// ========================================================
	// WindowInterface : a common interface for both window and window client
	// ========================================================

	class CHAOS_API WindowInterface : public TickableInterface, public InputReceiverInterface, public GPUProgramProviderInterface
	{
	protected:

		/** the drawing specialization method */
		virtual bool OnDraw(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params) { return true; }
	};

#endif

}; // namespace chaos