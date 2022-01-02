namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class ViewportWindow : public Window
	{
	public:

		/** gets the number of viewport */
		size_t GetViewportCount() const;
		/** gets a viewport by its index */
		AutoCastable<Viewport> GetViewport(size_t index);
		/** gets a viewport by its index */
		AutoConstCastable<Viewport> GetViewport(size_t index) const;
		/** find viewport by name */
		AutoCastable<Viewport> FindViewport(ObjectRequest request);
		/** find viewport by name */
		AutoConstCastable<Viewport> FindViewport(ObjectRequest request) const;

		/** gets the viewport layout */
		ViewportLayout* GetViewportLayout();
		/** gets the viewport layout */
		ViewportLayout const * GetViewportLayout() const;

	protected:

		/** the viewports */
		std::vector<shared_ptr<Viewport>> viewports;
		/** the viewport layout */
		shared_ptr<ViewportLayout> viewport_layout;

	};

#endif

}; // namespace chaos