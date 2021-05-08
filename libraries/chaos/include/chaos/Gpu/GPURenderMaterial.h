namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUProgramRenderMaterialProvider;
	class GPURenderMaterialInfoTraverseFunc;
	class GPURenderMaterialInfoEntry;
	class GPURenderMaterialInfo;
	class GPURenderMaterial;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUProgramRenderMaterialProvider : this is a variable provider dedicated for RenderMaterials
	*/

	class GPUProgramRenderMaterialProvider : public GPUProgramProvider
	{
	public:

		/** constructor */
		GPUProgramRenderMaterialProvider(class GPURenderMaterial const* in_render_material, GPUProgramProviderBase const* in_other_provider, GPURenderParams const* in_render_params) :
			render_material(in_render_material),
			other_provider(in_other_provider),
			render_params(in_render_params)
		{}

	protected:

		/** apply the actions */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;

	protected:

		/** the render material as base for the chain */
		GPURenderMaterial const* render_material = nullptr;
		/** another provider (use a non intrusive reference !!!) */
		GPUProgramProviderBase const* other_provider = nullptr;
		/** the render params used */
		GPURenderParams const* render_params = nullptr;
	};

	class GPURenderMaterialInfoTraverseFunc
	{
	public:

		virtual bool OnRenderMaterial(GPURenderMaterial const* render_material, GPURenderMaterialInfo const* material_info, char const* renderpass_name)
		{
			return false; // continue traversal
		}
	};

	/**
	* GPURenderMaterialInfoEntry : a 'pair' filter => material_info
	*/

	class GPURenderMaterialInfoEntry
	{

	public:

		/** filters for which this entry is valid */
		NameFilter filter;
		/** the material considerered */
		shared_ptr<GPURenderMaterialInfo> material_info;
	};

	/**
	* GPURenderMaterialInfo : the data for a material
	*/

	class GPURenderMaterialInfo : public Object
	{

	public:

		/** the program */
		shared_ptr<GPUProgram> program;
		/** parent material */
		shared_ptr<GPURenderMaterial> parent_material;
		/** some rendering states */
		GPUProgramProvider uniform_provider;

		/** some enable/disable behavior */
		NameFilter filter;
		/** whether the material is null (force to use no program => no rendering) */
		bool hidden = false;
		/** other renderpasses */
		std::vector<GPURenderMaterialInfoEntry> renderpasses;

		/** whether there was an explicit filter in the JSON file (or it is inherited from parent) */
		bool filter_specified = false;
		/** whether there was an explicit hidden in the JSON file (or it is inherited from parent) */
		bool hidden_specified = false;
	};

	/**
	* GPURenderMaterial : this is the combinaison of some uniforms and a program
	*/

	class GPURenderMaterial : public GPUFileResource
	{
		friend class GPUProgramData;
		friend class GPUProgramRenderMaterialProvider;
		friend class GPUResourceManager;
		friend class GPURenderMaterialLoader;
		friend class GPURenderMaterialLoaderReferenceSolver;

	public:

		/** constructor */
		GPURenderMaterial();
		/** destructor */
		virtual ~GPURenderMaterial();

		/** prepare the rendering (find the program, use it, fills its uniforms and returns the program) */
		GPUProgram const* UseMaterial(GPUProgramProviderBase const* in_uniform_provider, GPURenderParams const& render_params) const;

		/** set the program */
		bool SetProgram(GPUProgram* in_program);
		/** set the parent material */
		bool SetParentMaterial(GPURenderMaterial* in_parent);

		/** go through the hierarchy and search for the program */
		GPUProgram const* GetEffectiveProgram(GPURenderParams const& render_params) const;

		/** get the uniform provider */
		GPUProgramProvider& GetUniformProvider();
		/** get the uniform provider */
		GPUProgramProvider const& GetUniformProvider() const;

		/** traverse method entry point */
		bool Traverse(GPURenderMaterialInfoTraverseFunc& traverse_func, char const* renderpass_name) const;

		/** create a RenderMaterial from a simple program */
		static GPURenderMaterial* GenRenderMaterialObject(GPUProgram* program);

		/** override */
		virtual void Release() override;

	protected:

		/** traversal method implementation */
		static bool TraverseImpl(GPURenderMaterial const* render_material, GPURenderMaterialInfo const* material_info, GPURenderMaterialInfoTraverseFunc& traverse_func, char const* renderpass_name);
		/** search some cycles throught parent_material (returning true is an error) */
		static bool SearchRenderMaterialCycle(GPURenderMaterialInfo const* material_info, GPURenderMaterial const* searched_material);


	protected:

		/** all the information for the material */
		shared_ptr<GPURenderMaterialInfo> material_info;
	};

#endif

}; // namespace chaos