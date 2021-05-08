namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPURenderMaterialParentReference;
	class GPURenderMaterialLoaderReferenceSolver;
	class GPURenderMaterialLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ===========================================================================
	// GPURenderMaterialLoaderReferenceSolver : some references may not be resolved when loading (register them to resolve them when possible)
	// ===========================================================================

	class GPURenderMaterialParentReference
	{
	public:

		/** the material of concern (used to detected cyclic inheritance) */
		shared_ptr<GPURenderMaterial> material;
		/** the material_info to resolve */
		GPURenderMaterialInfo* material_info = nullptr;
		/** the name of the parent */
		std::string parent_name;
	};

	class GPURenderMaterialLoaderReferenceSolver
	{
	public:

		/** register a parenting */
		void AddInheritance(GPURenderMaterial* material, GPURenderMaterialInfo* material_info, std::string parent_name);
		/** resolve all pending references */
		bool ResolveReferences(GPUResourceManager* resource_manager);

	protected:

		/** the references for parents */
		std::vector<GPURenderMaterialParentReference> parent_references;
	};

	// ===========================================================================
	// GPURenderMaterialLoader
	// ===========================================================================

	class GPURenderMaterialLoader : public ResourceManagerLoader<GPURenderMaterial, GPUResourceManager>
	{
	public:

		/** constructor */
		GPURenderMaterialLoader(GPUResourceManager* in_resource_manager, GPURenderMaterialLoaderReferenceSolver* in_reference_solver) :
			ResourceManagerLoader<GPURenderMaterial, GPUResourceManager>(in_resource_manager),
			reference_solver(in_reference_solver)
		{
			assert(in_resource_manager != nullptr); // opposite to GPUTextureLoader and GPUProgramLoader, manager cannot be nullptr for RenderMaterial
		}

		/** load an object from JSON */
		virtual GPURenderMaterial* LoadObject(char const* name, nlohmann::json const& json, boost::filesystem::path const& config_path) const;
		/** Generate a render material from an file */
		virtual GPURenderMaterial* LoadObject(FilePathParam const& path, char const* name = nullptr) const;

	protected:

		/** initialize the material_info */
		bool InitializeMaterialInfoFromJSON(GPURenderMaterial* render_material, GPURenderMaterialInfo* material_info, nlohmann::json const& json, boost::filesystem::path const& config_path) const;
		/** initialize a texture from its name */
		bool InitializeTextureFromName(GPURenderMaterialInfo* material_info, char const* uniform_name, char const* texture_name) const;
		/** initialize a texture from its path */
		bool InitializeTextureFromPath(GPURenderMaterialInfo* material_info, char const* uniform_name, FilePathParam const& path) const;

		/** initialize the program from its name */
		bool InitializeProgramFromName(GPURenderMaterialInfo* material_info, char const* program_name) const;
		/** initialize the program from its path */
		bool InitializeProgramFromPath(GPURenderMaterialInfo* material_info, FilePathParam const& path) const;

		/** add a uniform in the render material */
		bool AddUniformToRenderMaterial(GPURenderMaterialInfo* material_info, char const* uniform_name, nlohmann::json const& json) const;

		/** get the program from JSON */
		bool InitializeProgramFromJSON(GPURenderMaterialInfo* material_info, nlohmann::json const& json, boost::filesystem::path const& config_path) const;
		/** get the textures from JSON */
		bool InitializeTexturesFromJSON(GPURenderMaterialInfo* material_info, nlohmann::json const& json, boost::filesystem::path const& config_path) const;
		/** get the uniforms from JSON */
		bool InitializeUniformsFromJSON(GPURenderMaterialInfo* material_info, nlohmann::json const& json, boost::filesystem::path const& config_path) const;
		/** get the renderpasses from JSON */
		bool InitializeRenderPassesFromJSON(GPURenderMaterial* render_material, GPURenderMaterialInfo* material_info, nlohmann::json const& json, boost::filesystem::path const& config_path) const;

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const& path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(ObjectRequest request) const override;

	protected:

		/** the reference resolver */
		GPURenderMaterialLoaderReferenceSolver* reference_solver = nullptr;
	};

#endif

}; // namespace chaos