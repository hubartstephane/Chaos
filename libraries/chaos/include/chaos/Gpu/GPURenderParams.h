namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPURenderParams;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ========================================================
	// GPURenderParams : some data for the rendering
	// ========================================================

	class GPURenderParams
	{

	public:

		/** get the material */
		GPURenderMaterial const* GetMaterial(GPURenderable const* renderable, GPURenderMaterial const* default_material) const;

	public:

		/** the used viewport */
		ViewportPlacement viewport;
		/** the rendering full size */
		glm::ivec2 full_size = { 0, 0 };

		/** material provider */
		shared_ptr<GPUMaterialProvider> material_provider;
		/** some filters */
		shared_ptr<GPURenderableFilter> object_filter;
		/** material specialization */
		std::string renderpass_name;
		/** the instancing information */
		GPUInstancingInfo instancing;
	};

#endif

}; // namespace chaos