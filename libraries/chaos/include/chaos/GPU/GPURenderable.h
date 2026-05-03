namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPURenderable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ========================================================
	// GPURenderable : base class for all object that can be rendered
	// ========================================================

	class CHAOS_API GPURenderable : public Tickable
	{

		CHAOS_DECLARE_OBJECT_CLASS(GPURenderable, Tickable);

	public:

		/** public method to render the object (Display = PrepareDisplay + DoDisplay) */
		int Display(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, GPURenderParams const& render_params);

		/** show or hide the object */
		void Show(bool in_visible = true);
		/** returns whether the object is visible or not */
		bool IsVisible() const;

		/** whether the object can be ticked if hidden */
		bool CanTickIfHidden() const;
		/** change whether the object can be ticked if hidden flag */
		void SetCanTickIfHidden(bool in_tick_hidden);

		/** add names in the enabled list */
		void AddEnabledRenderPasses(char const* renderpass_names);
		/** add names in the disabled list */
		void AddDisabledRenderPasses(char const* renderpass_names);
		/** remove names from the enabled list */
		void RemoveEnabledRenderPasses(char const* renderpass_names);
		/** remove names from the disabled list */
		void RemoveDisabledRenderPasses(char const* renderpass_names);

		/** check whether the renderable can be displayed by the name */
		bool IsRenderPassEnabled(char const* renderpass_name) const;

	protected:

		/** update resources if necessary */
		bool UpdateGPUResources(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, GPURenderParams const& render_params);
		/** check whether the object is to be displayed */
		bool ShouldDisplayObject(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, GPURenderParams const& render_params) const;

		/** the user defined method to display the object (this method is already integrated into Display method) */
		virtual int DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, GPURenderParams const& render_params);

		/** override */
		virtual bool CanTick() override;
		/** called to update the graphic resource */
		virtual bool DoUpdateGPUResources(GPURenderContext* render_context);
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

#endif

}; // namespace chaos