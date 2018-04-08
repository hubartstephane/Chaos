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

	bool RenderMaterialLoader::InitializeProgramFromName(RenderMaterial * render_material, char const * program_name) const
	{
		GPUProgram * program = manager->FindProgram(program_name);
		if (program == nullptr)
			return false;
		render_material->SetProgram(program);
		return true;
	}

	bool RenderMaterialLoader::InitializeProgramFromPath(RenderMaterial * render_material, FilePathParam const & path) const
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

	bool RenderMaterialLoader::InitializeProgramFromJSON(RenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const
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
			return InitializeProgramFromPath(render_material, path);
		}

		// inplace declared program 
		GPUProgram * program = manager->LoadProgram(*json_program, program_name.c_str());
		if (program != nullptr)
		{
			render_material->SetProgram(program);
			return true;
		}
		return false;
	}

	bool RenderMaterialLoader::InitializeTexturesFromJSON(RenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{
		// search the texture object
		nlohmann::json const * json_textures = JSONTools::GetStructure(json, "textures");
		if (json_textures == nullptr || !json_textures->is_object())
			return false;
		// enumerate all textures
		for (nlohmann::json::const_iterator it = json_textures->begin(); it != json_textures->end(); ++it)
		{
			std::string texture_name = it.key();

			render_material = render_material;
		}
		return true;
	}

	template<typename VECTOR_TYPE>
	static bool DoAddUniformVectorToRenderMaterial(RenderMaterial * render_material, char const * uniform_name, nlohmann::json const & json, VECTOR_TYPE & value)
	{
		render_material->GetUniformProvider().AddVariableValue("uniform_name", glm::bvec2());
		render_material->GetUniformProvider().AddVariableValue("uniform_name", glm::bvec3());
		render_material->GetUniformProvider().AddVariableValue("uniform_name", glm::bvec4());

		size_t count = json.size();
		for (size_t i = 0; i < count; ++i)
			value[i] = json[i].get<VECTOR_TYPE::value_type>();
	//	render_material->GetUniformProvider().AddVariableValue(uniform_name, value);
		return true;
	}

	template<typename SCALAR_TYPE>
	static bool AddUniformVectorToRenderMaterial(RenderMaterial * render_material, char const * uniform_name, nlohmann::json const & json)
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

	bool RenderMaterialLoader::AddUniformToRenderMaterial(RenderMaterial * render_material, char const * uniform_name, nlohmann::json const & json) const
	{
		// is the uniform a boolean ?
		if (json.is_boolean())
		{
			render_material->GetUniformProvider().AddVariableValue(uniform_name, json.get<bool>());
			return true;
		}
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

			bool boolean_array = false;
			bool integer_array = false;
			bool real_array    = false;
			for (size_t i = 0; i < count; ++i)
			{
				// detect variable types
				bool boolean = json[i].is_boolean();
				bool integer = json[i].is_number_integer();
				bool real    = json[i].is_number_float();
				if (!boolean && !integer && !real) // only types accepted
					return false;
				// promotion
				real_array    |= real;
				integer_array |= integer;
				boolean_array |= boolean;
			}

			// create the array
			if (real_array)
				return AddUniformVectorToRenderMaterial<float>(render_material, uniform_name, json);
			if (integer_array)
				return AddUniformVectorToRenderMaterial<int>(render_material, uniform_name, json);
			if (boolean_array)
				return AddUniformVectorToRenderMaterial<bool>(render_material, uniform_name, json);
		}
		return false;
	}
	
	bool RenderMaterialLoader::InitializeUniformsFromJSON(RenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const
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
		InitializeProgramFromJSON(result, json, config_path);
		// look at textures
		InitializeTexturesFromJSON(result, json, config_path);
		// look at uniforms
		InitializeUniformsFromJSON(result, json, config_path);

		// finalize : give name / path to the new resource
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
