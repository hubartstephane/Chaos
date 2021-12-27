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

		/** the viewport */
		box2 viewport;
		/** the screen size */
		glm::ivec2 screen_size = glm::ivec2(0, 0);
		/** material provider */
		shared_ptr<GPUMaterialProvider> material_provider;
		/** some filters */
		shared_ptr<GPURenderableFilter> object_filter;


		// shu48

#if 0
		glm::mat4 projection_matrix;

		glm::mat4 local_to_world;

		glm::mat4 world_to_local;
#endif




		/** pointer on the current camera */
		Camera* camera = nullptr;





		/** material specialization */
		std::string renderpass_name;
		/** the instancing information */
		GPUInstancingInfo instancing;
	};

#endif

}; // namespace chaos