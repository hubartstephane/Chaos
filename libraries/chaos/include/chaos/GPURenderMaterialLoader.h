#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUClasses.h>

#include <chaos/GPURenderMaterial.h>
#include <chaos/FilePath.h>
#include <chaos/ResourceManagerLoader.h>
#include <chaos/FileResource.h>


namespace chaos
{
	// ===========================================================================
	// GPURenderMaterialLoaderReferenceSolver : some references may not be resolved when loading (register them to resolve them when possible)
	// ===========================================================================

	class GPURenderMaterialParentReference
	{
	public:

		/** the material we are searching for its parent */
		chaos::shared_ptr<GPURenderMaterial> render_material;
		/** the name of the parent */
		std::string parent_name;
	};

	class GPURenderMaterialSubMaterialReference
	{
	public:

		/** the material we are searching for its parent */
		chaos::shared_ptr<GPURenderMaterial> render_material;
		/** the filter used for this material */
		NameFilter filter;
		/** the name of the reference (name or path) */
		std::string reference_name;
		/** whether the reference is a name (other is path) */
		bool is_named_reference = true;
	};

	class GPURenderMaterialLoaderReferenceSolver
	{
	public:

		/** register a parenting */
		void AddInheritance(GPURenderMaterial * render_material, std::string parent_name);
		/** resolve all pending references */
		bool ResolveReferences(GPUResourceManager * resource_manager);
		/** add a sub material reference */
		void AddSubMaterialReference(GPURenderMaterial * render_material, NameFilter filter, std::string reference_name, bool is_named_reference);

	protected:

		/** the references for parents */
		std::vector<GPURenderMaterialParentReference> parent_references;
		/** the references for submaterials */
		std::vector<GPURenderMaterialSubMaterialReference> submaterials_references;

	};

	// ===========================================================================
	// GPURenderMaterialLoader
	// ===========================================================================

	class GPURenderMaterialLoader : public ResourceManagerLoader<GPURenderMaterial, ResourceFriend, GPUResourceManager>
	{
	public:

		/** constructor */
		GPURenderMaterialLoader(GPUResourceManager * in_resource_manager, GPURenderMaterialLoaderReferenceSolver * in_reference_solver) :
			ResourceManagerLoader<GPURenderMaterial, ResourceFriend, GPUResourceManager>(in_resource_manager),
			reference_solver(in_reference_solver)
		{
			assert(in_resource_manager != nullptr); // opposite to GPUTextureLoader and GPUProgramLoader, manager cannot be nullptr for RenderMaterial
		}

		/** load an object from JSON */
		virtual GPURenderMaterial * LoadObject(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path) const;
		/** Generate a render material from an file */
		virtual GPURenderMaterial * LoadObject(FilePathParam const & path, char const * name = nullptr) const;

	protected:

		/** initialize a texture from its name */
		bool InitializeTextureFromName(GPURenderMaterial * render_material, char const * uniform_name, char const * texture_name) const;
		/** initialize a texture from its path */
		bool InitializeTextureFromPath(GPURenderMaterial * render_material, char const * uniform_name, FilePathParam const & path) const;

		/** initialize the program from its name */
		bool InitializeProgramFromName(GPURenderMaterial * render_material, char const * program_name) const;
		/** initialize the program from its path */
		bool InitializeProgramFromPath(GPURenderMaterial * render_material, FilePathParam const & path) const;

		/** add a uniform in the render material */
		bool AddUniformToRenderMaterial(GPURenderMaterial * render_material, char const * uniform_name, nlohmann::json const & json) const;

		/** get the program from JSON */
		bool InitializeProgramFromJSON(GPURenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const;
		/** get the textures from JSON */
		bool InitializeTexturesFromJSON(GPURenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const;
		/** get the uniforms from JSON */
		bool InitializeUniformsFromJSON(GPURenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const;
		/** get the sub materials from JSON */
		bool InitializeSubMaterialsFromJSON(GPURenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const;
		

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(char const * in_name) const override;

	protected:

		/** the reference resolver */
		GPURenderMaterialLoaderReferenceSolver * reference_solver = nullptr;
		/** whether the render_material is to be inserted into the resource_manager (or is a submaterial) */
		bool insert_in_manager = true;
	};

}; // namespace chaos

