namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class ViewportWindow : public Window
	{
		CHAOS_DECLARE_OBJECT_CLASS(ViewportWindow, Window);

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

		/** change the viewport layout */
		void SetViewportLayout(ViewportLayout* in_viewport_layout);

		/** insert a viewport inside the window */
		void AddViewport(Viewport* viewport, bool compute_viewport_surfaces = true);
		/** remove a viewport inside the window */
		void RemoveViewport(Viewport* viewport, bool compute_viewport_surfaces = true, bool keep_empty_place = false);

		/** update the surface allocation for all viewports */
		void ComputeViewportPlacements(glm::ivec2 size);

	protected:

		/** override */
		virtual bool OnDraw(GPURenderer* renderer, box2 const& viewport, glm::ivec2 window_size, GPUProgramProviderInterface const * uniform_provider) override;
		/** override */
		virtual void OnWindowResize(glm::ivec2 size) override;

	protected:

		/** the viewports */
		std::vector<shared_ptr<Viewport>> viewports;
		/** the viewport layout */
		shared_ptr<ViewportLayout> viewport_layout;

	};

#endif

}; // namespace chaos