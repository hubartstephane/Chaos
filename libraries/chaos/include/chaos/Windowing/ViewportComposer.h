namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportComposer;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ViewportComposer : 
	*/

	class CHAOS_API ViewportComposer : public Viewport
	{
	public:

		/** gets the viewport layout */
		AutoCastable<ViewportLayout> GetViewportLayout();
		/** gets the viewport layout */
		AutoConstCastable<ViewportLayout> GetViewportLayout() const;

		/** change the viewport layout */
		void SetViewportLayout(ViewportLayout* in_viewport_layout);

		/** update the surface allocation for all viewports */
		void ComputeViewportPlacements(glm::ivec2 size);

	protected:

		/** the viewports */
		std::vector<shared_ptr<Viewport>> viewports;
		/** the viewport layout */
		shared_ptr<ViewportLayout> viewport_layout;
	};



#endif

}; // namespace chaos