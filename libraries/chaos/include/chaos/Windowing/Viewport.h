namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Viewport;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class Viewport : public Object, public NamedObject
	{
		friend class ViewportWindow;

	public:

		/** gets the window owning his viewport */
		ViewportWindow* GetWindow() { return window; }
		/** gets the window owning his viewport */
		ViewportWindow const * GetWindow() const { return window; }

		/** get the placement */
		ViewportPlacement const& GetViewportPlacement() const { return placement; }
		/** set the placement */
		void SetViewportPlacement(ViewportPlacement const& in_placement) { placement = in_placement; }

	protected:

		/** the viewport window that owns this viewport */
		ViewportWindow* window = nullptr;
		/** the placement of the viewport */
		ViewportPlacement placement;
	};

#endif

}; // namespace chaos