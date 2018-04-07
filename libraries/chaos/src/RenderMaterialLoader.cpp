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


#if 0




	std::string material_path;
	if (JSONTools::GetAttribute(json, "path", material_path))
	{

		material_path = material_path;
	}

	nlohmann::json const * program = JSONTools::GetStructure(json, "program");
	if (program != nullptr)
	{

		program = program;
	}

	std::string parent_material;
	if (JSONTools::GetAttribute(json, "parent_material", parent_material))
	{

		parent_material = parent_material;
	}

	nlohmann::json const * textures = JSONTools::GetStructure(json, "textures");
	if (textures != nullptr)
	{

		textures = textures;
	}

	nlohmann::json const * uniforms = JSONTools::GetStructure(json, "uniforms");
	if (uniforms != nullptr)
	{


		uniforms = uniforms;
	}




#endif

}; // namespace chaos
