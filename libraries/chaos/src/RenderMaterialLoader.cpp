#include <chaos/RenderMaterialLoader.h>
#include <chaos/MathTools.h>
#include <chaos/BoostTools.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{


	RenderMaterialLoader::RenderMaterialLoader(GPUResourceManager * in_resource_manager) :
		ManagerResourceLoader<EmptyClass, GPUResourceManager>(in_resource_manager)
	{

	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name) const
	{







		return nullptr;
	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(FilePathParam const & path, std::string & parent_name) const
	{







		return nullptr;
	}

	bool RenderMaterialLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager->FindRenderMaterialByPath(path) != nullptr);
	}

	bool RenderMaterialLoader::IsNameAlreadyUsedInManager(std::string const & in_name) const
	{
		return (manager->FindRenderMaterial(in_name.c_str()) != nullptr);
	}











#if 0

	RenderMaterialLoader::RenderMaterialLoader(GPUResourceManager * in_resource_manager) :
		resource_manager(in_resource_manager)
	{
		assert(in_resource_manager != nullptr);
	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name) const
	{
		material_path_resolved = false;
		return DoGenRenderMaterialObject(json, config_path, parent_name);
	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(FilePathParam const & path, std::string & parent_name) const
	{
		if (resource_manager->FindRenderMaterialByPath(path) != nullptr)  // ensure the Manager does not already have a RenderMaterial with the same path
			return nullptr;
		material_path_resolved = false;
		return DoGenRenderMaterialObject(path, parent_name);
	}







	RenderMaterial * RenderMaterialLoader::DoGenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name) const
	{
		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(json, "path", p))
		{
			FilePathParam path(p, config_path);
			return DoGenRenderMaterialObject(path, parent_name);
		}

		JSONTools::GetAttribute(json, "parent_material", parent_name);

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

	RenderMaterial * RenderMaterialLoader::DoGenRenderMaterialObject(FilePathParam const & path, std::string & parent_name) const
	{
		if (resource_manager->FindRenderMaterialByPath(path) != nullptr)  // ensure the Manager does not already have a RenderMaterial with the same path
			return nullptr;

		RenderMaterial * result = nullptr;

		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json, true))
		{
			result = DoGenRenderMaterialObject(json, path.GetResolvedPath(), parent_name);
			if (result != nullptr)
				SetResourcePath(result, path.GetResolvedPath()); // update the path
		}
		return result;
	}

#endif

}; // namespace chaos
