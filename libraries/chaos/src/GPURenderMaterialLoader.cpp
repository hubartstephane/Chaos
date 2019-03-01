#include <chaos/GPURenderMaterialLoader.h>
#include <chaos/GPUResourceManagerProgramLoader.h>
#include <chaos/GPUResourceManagerTextureLoader.h>
#include <chaos/MathTools.h>
#include <chaos/BoostTools.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	bool GPURenderMaterialLoader::InitializeProgramFromName(GPURenderMaterial * render_material, char const * program_name) const
	{
		GPUProgram * program = manager->FindProgram(program_name);
		if (program == nullptr)
			return false;
		render_material->SetProgram(program);
		return true;
	}

	bool GPURenderMaterialLoader::InitializeProgramFromPath(GPURenderMaterial * render_material, FilePathParam const & path) const
	{
		// take already loaded program, or try load it
		GPUProgram * program = manager->FindProgramByPath(path.GetResolvedPath());
		if (program == nullptr)
			program = manager->LoadProgram(path);
		// set the program
		if (program != nullptr)
		{
			render_material->SetProgram(program);
			return true;
		}
		return false;
	}

	bool GPURenderMaterialLoader::InitializeProgramFromJSON(GPURenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{
		// does the JSON have a "program" string ?
		std::string program_name;
		if (JSONTools::GetAttribute(json, "program", program_name))
			return InitializeProgramFromName(render_material, program_name.c_str());

		// if there is a program it has to be under the form of a structure
		nlohmann::json const * json_program = JSONTools::GetStructure(json, "program");
		if (json_program == nullptr || !json_program->is_object())
			return false;

		// does the object has a member : "name" ? (this can be an inplace program defined => only return if we found the corresponding resource)
		if (JSONTools::GetAttribute(*json_program, "name", program_name))
			if (InitializeProgramFromName(render_material, program_name.c_str()))
				return true;

		// does the object has a "path" member ?
		std::string program_path;
		if (JSONTools::GetAttribute(*json_program, "path", program_path))
		{
			FilePathParam path(program_path, config_path);
			if (InitializeProgramFromPath(render_material, path))
				return true;
		}

		// inplace declared program 
		GPUResourceManagerProgramLoader program_loader(manager);
		GPUProgram * program = program_loader.LoadObject(nullptr, *json_program, program_name.c_str());

		//GPUProgram * program = manager->LoadProgram(nullptr, *json_program, program_name.c_str());
		if (program == nullptr)
			return false;
		render_material->SetProgram(program);
		return true;
	}

	bool GPURenderMaterialLoader::InitializeTextureFromName(GPURenderMaterial * render_material, char const * uniform_name, char const * texture_name) const
	{
		GPUTexture * texture = manager->FindTexture(texture_name);
		if (texture == nullptr)
			return false;
		render_material->GetUniformProvider().AddVariableTexture(uniform_name, texture);
		return true;
	}

	bool GPURenderMaterialLoader::InitializeTextureFromPath(GPURenderMaterial * render_material, char const * uniform_name, FilePathParam const & path) const
	{
		// take already loaded texture, or try load it
		GPUTexture * texture = manager->FindTextureByPath(path.GetResolvedPath());
		if (texture == nullptr)
			texture = manager->LoadTexture(path);
		// set the texture
		if (texture == nullptr)
			return false;
		render_material->GetUniformProvider().AddVariableTexture(uniform_name, texture);
		return true;
	}

	bool GPURenderMaterialLoader::InitializeTexturesFromJSON(GPURenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{
		// search the texture object
		nlohmann::json const * json_textures = JSONTools::GetStructure(json, "textures");
		if (json_textures == nullptr || !json_textures->is_object())
			return false;
		// enumerate all textures
		for (nlohmann::json::const_iterator it = json_textures->begin(); it != json_textures->end(); ++it)
		{
			// get the name of the uniform
			std::string texture_uniform_name = it.key();
			if (texture_uniform_name.empty())
				continue;

			// is it directly a name "texture_uniform_name":"texture_name" ?
			if (it->is_string())
			{
				std::string texture_name = it->get<std::string>();
				if (!texture_name.empty())
					if (InitializeTextureFromName(render_material, texture_uniform_name.c_str(), texture_name.c_str()))
						continue;
			}

			// to continue, we have to work with an object
			if (!it->is_object())
				continue;

			// does the object have a "Name" => try to find already loaded texture ?
			std::string texture_name;
			if (JSONTools::GetAttribute(*it, "name", texture_name))
			{
				if (!texture_name.empty())
					if (InitializeTextureFromName(render_material, texture_uniform_name.c_str(), texture_name.c_str()))
						continue;
			}

			// does the object have a "Path" => try to find already loaded texture or load the texture
			std::string texture_path;
			if (JSONTools::GetAttribute(*it, "path", texture_path))
			{
				FilePathParam path(texture_path, config_path);
				if (InitializeTextureFromPath(render_material, texture_uniform_name.c_str(), texture_path.c_str()))
					continue;
			}

			// inplace declared texture 
			GPUResourceManagerTextureLoader texture_loader(manager);
			GPUTexture * texture = texture_loader.LoadObject(nullptr, *it, texture_name.c_str());
			//GPUTexture * texture = manager->LoadTexture(nullptr, *it, texture_name.c_str());
			if (texture == nullptr)
				continue;
			render_material->GetUniformProvider().AddVariableTexture(texture_uniform_name.c_str(), texture);
		}
		return true;
	}

	template<typename VECTOR_TYPE>
	static bool DoAddUniformVectorToRenderMaterial(GPURenderMaterial * render_material, char const * uniform_name, nlohmann::json const & json, VECTOR_TYPE & value)
	{
		size_t count = json.size();
		for (size_t i = 0; i < count; ++i)
			value[i] = json[i].get<VECTOR_TYPE::value_type>();
		render_material->GetUniformProvider().AddVariableValue(uniform_name, value);
		return true;
	}

	template<typename SCALAR_TYPE>
	static bool AddUniformVectorToRenderMaterial(GPURenderMaterial * render_material, char const * uniform_name, nlohmann::json const & json)
	{
		size_t count = json.size();
		if (count == 1)
		{
			render_material->GetUniformProvider().AddVariableValue(uniform_name, json[0].get<SCALAR_TYPE>());
			return true;
		}
		if (count == 2)
		{
			glm::tvec2<SCALAR_TYPE> value;
			return DoAddUniformVectorToRenderMaterial(render_material, uniform_name, json, value);
		}
		if (count == 3)
		{
			glm::tvec3<SCALAR_TYPE> value;
			return DoAddUniformVectorToRenderMaterial(render_material, uniform_name, json, value);
		}
		if (count == 4)
		{
			glm::tvec4<SCALAR_TYPE> value;
			return DoAddUniformVectorToRenderMaterial(render_material, uniform_name, json, value);
		}
		return false;
	}

	bool GPURenderMaterialLoader::AddUniformToRenderMaterial(GPURenderMaterial * render_material, char const * uniform_name, nlohmann::json const & json) const
	{
		// is the uniform a integer ?
		if (json.is_number_integer())
		{
			render_material->GetUniformProvider().AddVariableValue(uniform_name, json.get<int>());
			return true;
		}
		// is the uniform a number ?
		if (json.is_number())
		{
			render_material->GetUniformProvider().AddVariableValue(uniform_name, json.get<float>());
			return true;
		}
		// is the uniform an array of numbers
		if (json.is_array())
		{
			// only accept array of numbers. Search the type
			size_t count = json.size();
			if (count == 0)
				return false;
			if (count > 4) // only vectors for moment
				return false;

			bool integer_array = false;
			bool real_array    = false;
			for (size_t i = 0; i < count; ++i)
			{
				// detect variable types
				bool integer = json[i].is_number_integer();
				bool real    = json[i].is_number_float();
				if (!integer && !real) // only types accepted
					return false;
				// promotion
				real_array    |= real;
				integer_array |= integer;
			}

			// create the array
			if (real_array)
				return AddUniformVectorToRenderMaterial<float>(render_material, uniform_name, json);
			if (integer_array)
				return AddUniformVectorToRenderMaterial<int>(render_material, uniform_name, json);
		}
		return false;
	}

	bool GPURenderMaterialLoader::InitializeUniformsFromJSON(GPURenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{
		// search the uniform object
		nlohmann::json const * json_uniforms = JSONTools::GetStructure(json, "uniforms");
		if (json_uniforms == nullptr || !json_uniforms->is_object())
			return false;
		// enumerate all uniforms
		for (nlohmann::json::const_iterator it = json_uniforms->begin(); it != json_uniforms->end(); ++it)
		{
			std::string uniform_name = it.key();
			if (!uniform_name.empty())
				AddUniformToRenderMaterial(render_material, uniform_name.c_str(), *it);
		}
		return true;
	}

	GPURenderMaterial * GPURenderMaterialLoader::LoadObject(nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name) const
	{
		// get the name, ensure no name collision
		if (!CheckResourceName(json))
			return nullptr;

		// indirect call
		std::string path;
		if (JSONTools::GetAttribute(json, "path", path))
			return LoadObject(path, parent_name);

		// create a new material
		GPURenderMaterial * result = new GPURenderMaterial;
		if (result == nullptr)
			return nullptr;

		// search material parent
		if (parent_name.empty())
			JSONTools::GetAttribute(json, "parent_material", parent_name);
		// search program
		InitializeProgramFromJSON(result, json, config_path);
		// look at textures
		InitializeTexturesFromJSON(result, json, config_path);
		// look at uniforms
		InitializeUniformsFromJSON(result, json, config_path);

		// finalize : give name / path to the new resource
		FinalizeLoadedResource(result);

		return result;
	}

	GPURenderMaterial * GPURenderMaterialLoader::LoadObject(FilePathParam const & path, std::string & parent_name) const
	{
		if (!CheckResourcePath(path))
			return nullptr;

		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json, true))
			return LoadObject(json, path.GetResolvedPath(), parent_name);
		return nullptr;
	}

	bool GPURenderMaterialLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager->FindRenderMaterialByPath(path) != nullptr);
	}

	bool GPURenderMaterialLoader::IsNameAlreadyUsedInManager(std::string const & in_name) const
	{
		return (manager->FindRenderMaterial(in_name.c_str()) != nullptr);
	}

	GPURenderMaterial * GPURenderMaterialLoader::LoadObject(char const * keyname, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{




		return nullptr;
	}






#if 0 // shuxxx MANAGER_LOADER

	/** add a material from a JSON object (return the name of the parent material if any) */
	virtual GPURenderMaterial * LoadRenderMaterial(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name);

	std::map<GPURenderMaterial *, std::string> parenting_map;

	[this, &parenting_map](char const * name, nlohmann::json const & obj_json, boost::filesystem::path const & path)
	{
		std::string parent_name;

		GPURenderMaterial * render_material = LoadRenderMaterial(name, obj_json, path, parent_name);
		if (render_material != nullptr && !parent_name.empty())
			parenting_map[render_material] = std::move(parent_name);
		return render_material;
	});
	// resolve the parenting
	if (result)
	{
		for (auto it : parenting_map)
			SetRenderMaterialParent(it.first, it.second);
	}
	return result;



	GPURenderMaterial * GPUResourceManager::LoadRenderMaterial(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name)
	{
		// ensure no name collision
		if (!CanAddRenderMaterial(name))
			return nullptr;
		// initialize the loader, so te name will be given to result at the end
		GPURenderMaterialLoader loader(this);
		loader.SetResultName(name);

		// load the resource
		GPURenderMaterial * render_material = loader.LoadObject(json, config_path, parent_name);
		if (render_material != nullptr)
			render_materials.push_back(render_material);
		return render_material;
	}

#endif

}; // namespace chaos
