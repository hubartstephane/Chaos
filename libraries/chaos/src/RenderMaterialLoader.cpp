#include <chaos/RenderMaterialLoader.h>
#include <chaos/MathTools.h>
#include <chaos/BoostTools.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	RenderMaterialLoader::RenderMaterialLoader(GPUResourceManager * in_resource_manager) :
		resource_manager(in_resource_manager)
	{
		assert(in_resource_manager != nullptr);
	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{
		material_path_resolved = false;
		return DoGenRenderMaterialObject(json, config_path);
	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(FilePathParam const & path) const
	{
		if (resource_manager->FindRenderMaterialByPath(path) != nullptr)  // ensure the Manager does not already have a RenderMaterial with the same path
			return nullptr;
		material_path_resolved = false;
		return DoGenRenderMaterialObject(path);
	}







	RenderMaterial * RenderMaterialLoader::DoGenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{
		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(json, "path", p))
		{
			FilePathParam path(p, config_path);
			return DoGenRenderMaterialObject(path);
		}



#if 0







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




		return nullptr;
	}

	RenderMaterial * RenderMaterialLoader::DoGenRenderMaterialObject(FilePathParam const & path) const
	{
		if (resource_manager->FindRenderMaterialByPath(path) != nullptr)  // ensure the Manager does not already have a RenderMaterial with the same path
			return nullptr;

		RenderMaterial * result = nullptr;

		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json, true))
		{
			result = DoGenRenderMaterialObject(json, path.GetResolvedPath());
			if (result != nullptr)
				SetResourcePath(result, path.GetResolvedPath()); // update the path
		}
		return result;
	}


#if 0


	std::string material_path;
	if (JSONTools::GetAttribute(json, "path", material_path))
	{

		material_path = material_path;
	}





#endif

}; // namespace chaos
