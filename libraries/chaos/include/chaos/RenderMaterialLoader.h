#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/RenderMaterial.h>
#include <chaos/FilePath.h>

namespace chaos
{

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

		/** Generate a program from a json content */
		virtual RenderMaterial * GenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path) const;
		/** Generate a program from an file */
		virtual RenderMaterial * GenRenderMaterialObject(FilePathParam const & path) const;

	protected:

		/** the manager concerned by the loader */
		GPUResourceManager * resource_manager = nullptr;
	};

}; // namespace chaos
