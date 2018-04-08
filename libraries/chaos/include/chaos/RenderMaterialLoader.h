#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/RenderMaterial.h>
#include <chaos/FilePath.h>
#include <chaos/ManagerResourceLoader.h>
#include <chaos/EmptyClass.h>


namespace chaos
{

	class RenderMaterialLoader : public ManagerResourceLoader<EmptyClass, GPUResourceManager>
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

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(std::string const & in_name) const override;
	};

#if 0

	/**
	* RenderMaterialLoader
	*/

	class RenderMaterialLoader : protected GPUFileResourceFriend
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

		/** internal generate a render material from a json content */
		virtual RenderMaterial * DoGenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name) const;
		/** internal generate a render material from an file */
		virtual RenderMaterial * DoGenRenderMaterialObject(FilePathParam const & path, std::string & parent_name) const;

	protected:

		/** the manager concerned by the loader */
		GPUResourceManager * resource_manager = nullptr;
		/** indicates whether the resource path is already detected */
		mutable bool material_path_resolved = false;
	};

#endif

}; // namespace chaos
