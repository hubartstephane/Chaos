#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPURenderable;

}; // namespace chaos

#else 

namespace chaos
{

	// ========================================================
	// GPURenderable : base class for all object that can be rendered
	// ========================================================

	class GPURenderable : public Tickable
	{

		CHAOS_OBJECT_DECLARE_CLASS2(GPURenderable, Tickable);

	public:

		/** public method to render the object */
		int Display(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params);

		/** show or hide the object */
		void Show(bool in_visible = true);
		/** returns whether the object is visible or not */
		bool IsVisible() const;

		/** whether the object can be ticked if hidden */
		bool CanTickIfHidden() const;
		/** change whether the object can be ticked if hidden flag */
		void SetCanTickIfHidden(bool in_tick_hidden);

		/** add names in the enabled list */
		void AddEnabledRenderPasses(char const * renderpass_names);
		/** add names in the disabled list */
		void AddDisabledRenderPasses(char const * renderpass_names);
		/** remove names from the enabled list */
		void RemoveEnabledRenderPasses(char const * renderpass_names);
		/** remove names from the disabled list */
		void RemoveDisabledRenderPasses(char const * renderpass_names);

		/** check whether the renderable can be displayed by the name */
		bool IsRenderPassEnabled(char const * renderpass_name) const;

	protected:

		/** override */
		virtual bool CanTick() override;
		/** the user defined method to display the object */
		virtual int DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params);
		/** called to update the graphic resource */
		virtual bool DoUpdateGPUResources(GPURenderer * renderer);
		/** called whenever object visibility has been changed */
		virtual void OnVisibilityChanged(bool in_visible);

	protected:

		/** whether the object is hidden or visible */
		bool visible = true;
		/** whether the object must be ticked if hidden */
		bool tick_hidden = false;
		/** the last time the resource has been updated */
		uint64_t update_timestamp = 0;

		/** the list of enabled renderpass names */
		NameFilter renderpass_filter;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

