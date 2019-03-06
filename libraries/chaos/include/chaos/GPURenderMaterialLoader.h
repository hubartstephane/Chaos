#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/FilePath.h>
#include <chaos/ResourceManagerLoader.h>
#include <chaos/EmptyClass.h>


namespace chaos
{

	class GPURenderMaterialLoader : public ResourceManagerLoader<GPURenderMaterial, GPUFileResourceFriend, GPUResourceManager>
	{
	public:

		/** constructor */
		GPURenderMaterialLoader(GPUResourceManager * in_resource_manager) :
			ResourceManagerLoader<GPURenderMaterial, GPUFileResourceFriend, GPUResourceManager>(in_resource_manager)
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

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(char const * in_name) const override;
	};

}; // namespace chaos
