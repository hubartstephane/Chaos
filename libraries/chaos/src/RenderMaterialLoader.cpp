#include <chaos/RenderMaterialLoader.h>
#include <chaos/MathTools.h>
#include <chaos/BoostTools.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{


	RenderMaterialLoader::RenderMaterialLoader(GPUResourceManager * in_resource_manager) :
		ManagerResourceLoader<GPUFileResourceFriend, GPUResourceManager>(in_resource_manager)
	{

	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name) const
	{
		// get the name, ensure no name collision
		if (!CheckResourceName(json))
			return nullptr;

		// indirect call
		std::string path;
		if (JSONTools::GetAttribute(json, "path", path))
			return GenRenderMaterialObject(path, parent_name);

		// create a new material
		RenderMaterial * result = new RenderMaterial;
		if (result == nullptr)
			return nullptr;

		// search material parent
		if (parent_name.empty())
			JSONTools::GetAttribute(json, "parent_material", parent_name);

		// search program
		nlohmann::json const * program = JSONTools::GetStructure(json, "program");
		if (program != nullptr)
		{

			program = program;
		}

		// look at textures
		nlohmann::json const * textures = JSONTools::GetStructure(json, "textures");
		if (textures != nullptr)
		{

			textures = textures;
		}

		// look at uniforms
		nlohmann::json const * uniforms = JSONTools::GetStructure(json, "uniforms");
		if (uniforms != nullptr)
		{


			uniforms = uniforms;
		}

		FinalizeLoadedResource(result);

		return result;
	}

	RenderMaterial * RenderMaterialLoader::GenRenderMaterialObject(FilePathParam const & path, std::string & parent_name) const
	{
		if (!CheckResourcePath(path))
			return nullptr;

		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json, true))
			return GenRenderMaterialObject(json, path.GetResolvedPath(), parent_name);
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

}; // namespace chaos
