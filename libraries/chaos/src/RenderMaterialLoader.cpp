#include <chaos/RenderMaterialLoader.h>
#include <chaos/MathTools.h>
#include <chaos/BoostTools.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	RenderMaterialLoader::RenderMaterialLoader(GPUResourceManager * in_resource_manager):
		resource_manager(in_resource_manager)
	{
		assert(in_resource_manager != nullptr);
	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{

		return nullptr;
	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(FilePathParam const & path) const
	{

		return nullptr;
	}


}; // namespace chaos
