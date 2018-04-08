#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/RenderMaterial.h>
#include <chaos/FilePath.h>
#include <chaos/ManagerResourceLoader.h>
#include <chaos/EmptyClass.h>


namespace chaos
{

	class RenderMaterialLoader : public ManagerResourceLoader<GPUFileResourceFriend, GPUResourceManager>
	{
	public:

		/** constructor */
		RenderMaterialLoader(GPUResourceManager * in_resource_manager);
		/** destructor */
		virtual ~RenderMaterialLoader() = default;

		/** Generate a render material from a json content */
		virtual RenderMaterial * GenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name) const;
		/** Generate a render material from an file */
		virtual RenderMaterial * GenRenderMaterialObject(FilePathParam const & path, std::string & parent_name) const;

	protected:

		/** initialize the program from its name */
		bool InitializeProgramFromName(RenderMaterial * render_material, char const * program_name) const;
		/** initialize the program from its path */
		bool InitializeProgramFromPath(RenderMaterial * render_material, FilePathParam const & path) const;

		/** add a uniform in the render material */
		bool AddUniformToRenderMaterial(RenderMaterial * render_material, char const * uniform_name, nlohmann::json const & json) const;

		/** get the program from JSON */
		bool InitializeProgramFromJSON(RenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const;
		/** get the textures from JSON */
		bool InitializeTexturesFromJSON(RenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const;
		/** get the uniforms from JSON */
		bool InitializeUniformsFromJSON(RenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const;

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(std::string const & in_name) const override;
	};

}; // namespace chaos
