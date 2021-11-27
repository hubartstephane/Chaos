#include <chaos/Chaos.h>

namespace chaos
{

	// ===========================================================================
	// GPURenderMaterialLoaderReferenceSolver
	// ===========================================================================

	void GPURenderMaterialLoaderReferenceSolver::AddInheritance(GPURenderMaterial * material, GPURenderMaterialInfo * material_info, std::string parent_name)
	{
		assert(material != nullptr);
		assert(material_info != nullptr);
		assert(!parent_name.empty());

		GPURenderMaterialParentReference reference;
		reference.material = material;
		reference.material_info = material_info;
		reference.parent_name = std::move(parent_name);
		parent_references.push_back(std::move(reference));
	}

	bool GPURenderMaterialLoaderReferenceSolver::ResolveReferences(GPUResourceManager * resource_manager)
	{
		assert(resource_manager != nullptr);
		// resolve parenting
		for (GPURenderMaterialParentReference & ref : parent_references)
		{
			// reference already resolved
			if (ref.material_info->parent_material != nullptr) 
				continue;
			// find parent
			GPURenderMaterial * parent_material = resource_manager->FindRenderMaterial(ref.parent_name.c_str());
			if (parent_material == nullptr)
				continue;
			// search cycle
			if (GPURenderMaterial::SearchRenderMaterialCycle(parent_material->material_info.get(), ref.material.get()))
				return false;
			// set the parent
			ref.material_info->parent_material = parent_material;
		}
		// clear all
		parent_references.clear();

		return true;
	}

		// ===========================================================================
		// GPURenderMaterialLoader
		// ===========================================================================

	bool GPURenderMaterialLoader::InitializeProgramFromName(GPURenderMaterialInfo * material_info, char const * program_name) const
	{
		GPUProgram * program = manager->FindProgram(program_name);
		if (program == nullptr)
			return false;
		material_info->program = program;
		return true;
	}

	bool GPURenderMaterialLoader::InitializeProgramFromPath(GPURenderMaterialInfo * material_info, FilePathParam const & path) const
	{
		// take already loaded program, or try load it
		GPUProgram * program = manager->FindProgramByPath(path.GetResolvedPath());
		if (program == nullptr)
			program = manager->LoadProgram(path);
		// set the program
		if (program == nullptr)
			return false;
		material_info->program = program;
		return true;
	}
	
	bool GPURenderMaterialLoader::InitializeProgramFromJSON(GPURenderMaterialInfo * material_info, nlohmann::json const & json) const
	{
		// does the JSON have a "program" string ?
		std::string program_name;
		if (JSONTools::GetAttribute(json, "program", program_name))
			return InitializeProgramFromName(material_info, program_name.c_str());

		// if there is a program it has to be under the form of a structure
		nlohmann::json const * json_program = JSONTools::GetStructure(json, "program");
		if (json_program == nullptr || !json_program->is_object())
			return false;

		// does the object has a member : "name" ? (this can be an inplace program defined => only return if we found the corresponding resource)
		if (JSONTools::GetAttribute(*json_program, "name", program_name))
			if (InitializeProgramFromName(material_info, program_name.c_str()))
				return true;

		// does the object has a "path" member ?
		boost::filesystem::path program_path;
		if (JSONTools::GetAttribute(*json_program, "path", program_path))
			if (InitializeProgramFromPath(material_info, program_path))
				return true;

		// inplace declared program 
		GPUProgramLoader program_loader(manager);
		GPUProgram * program = program_loader.LoadObject(nullptr, *json_program);
		if (program == nullptr || program->GetProgramType() != GPUProgramType::RENDER)
			return false;

		// initialize the program
		material_info->program = program;

		return true;
	}

	bool GPURenderMaterialLoader::InitializeTextureFromName(GPURenderMaterialInfo * material_info, char const * uniform_name, char const * texture_name) const
	{
		GPUTexture * texture = manager->FindTexture(texture_name);
		if (texture == nullptr)
			return false;
		material_info->uniform_provider.AddTexture(uniform_name, texture);
		return true;
	}

	bool GPURenderMaterialLoader::InitializeTextureFromPath(GPURenderMaterialInfo * material_info, char const * uniform_name, FilePathParam const & path) const
	{
		// take already loaded texture, or try load it
		GPUTexture * texture = manager->FindTextureByPath(path.GetResolvedPath());
		if (texture == nullptr)
			texture = manager->LoadTexture(path);
		// set the texture
		if (texture == nullptr)
			return false;
		material_info->uniform_provider.AddTexture(uniform_name, texture);
		return true;
	}

	bool GPURenderMaterialLoader::InitializeTexturesFromJSON(GPURenderMaterialInfo * material_info, nlohmann::json const & json) const
	{
		// search the texture object
		nlohmann::json const * json_textures = JSONTools::GetStructure(json, "textures");
		if (json_textures == nullptr || !json_textures->is_object())
			return false;
		// enumerate all textures
		for (nlohmann::json::const_iterator it = json_textures->begin(); it != json_textures->end(); ++it)
		{
			// get the name of the uniform
			std::string const & texture_uniform_name = it.key();
			if (texture_uniform_name.empty())
				continue;

			// is it directly a name "texture_uniform_name":"texture_name" ?
			if (it->is_string())
			{
				std::string texture_name = it->get<std::string>();
				if (!texture_name.empty())
					if (InitializeTextureFromName(material_info, texture_uniform_name.c_str(), texture_name.c_str()))
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
					if (InitializeTextureFromName(material_info, texture_uniform_name.c_str(), texture_name.c_str()))
						continue;
			}

			// does the object have a "Path" => try to find already loaded texture or load the texture
			boost::filesystem::path texture_path;
			if (JSONTools::GetAttribute(*it, "path", texture_path))
				if (InitializeTextureFromPath(material_info, texture_uniform_name.c_str(), texture_path))
					continue;

			// inplace declared texture 
			GPUTextureLoader texture_loader(manager);
			GPUTexture * texture = texture_loader.LoadObject(nullptr, *it);
			if (texture == nullptr)
				continue;
			material_info->uniform_provider.AddTexture(texture_uniform_name.c_str(), texture);
		}
		return true;
	}

	template<typename VECTOR_TYPE>
	static bool DoAddUniformVectorToRenderMaterial(GPURenderMaterialInfo * material_info, char const * uniform_name, nlohmann::json const & json, VECTOR_TYPE & value)
	{
		size_t count = json.size();
		for (size_t i = 0; i < count; ++i)
			value[i] = json[i].get<VECTOR_TYPE::value_type>();
		material_info->uniform_provider.AddVariable(uniform_name, value);
		return true;
	}

	template<typename SCALAR_TYPE>
	static bool AddUniformVectorToRenderMaterial(GPURenderMaterialInfo * material_info, char const * uniform_name, nlohmann::json const & json)
	{
		size_t count = json.size();
		if (count == 1)
		{
			material_info->uniform_provider.AddVariable(uniform_name, json[0].get<SCALAR_TYPE>());
			return true;
		}
		if (count == 2)
		{
			glm::tvec2<SCALAR_TYPE> value;
			return DoAddUniformVectorToRenderMaterial(material_info, uniform_name, json, value);
		}
		if (count == 3)
		{
			glm::tvec3<SCALAR_TYPE> value;
			return DoAddUniformVectorToRenderMaterial(material_info, uniform_name, json, value);
		}
		if (count == 4)
		{
			glm::tvec4<SCALAR_TYPE> value;
			return DoAddUniformVectorToRenderMaterial(material_info, uniform_name, json, value);
		}
		return false;
	}

	bool GPURenderMaterialLoader::AddUniformToRenderMaterial(GPURenderMaterialInfo * material_info, char const * uniform_name, nlohmann::json const & json) const
	{
		// is the uniform a integer ?
		if (json.is_number_integer())
		{
			material_info->uniform_provider.AddVariable(uniform_name, json.get<int>());
			return true;
		}
		// is the uniform a number ?
		if (json.is_number())
		{
			material_info->uniform_provider.AddVariable(uniform_name, json.get<float>());
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
				return AddUniformVectorToRenderMaterial<float>(material_info, uniform_name, json);
			if (integer_array)
				return AddUniformVectorToRenderMaterial<int>(material_info, uniform_name, json);
		}
		return false;
	}

	bool GPURenderMaterialLoader::InitializeUniformsFromJSON(GPURenderMaterialInfo * material_info, nlohmann::json const & json) const
	{
		// search the uniform object
		nlohmann::json const * json_uniforms = JSONTools::GetStructure(json, "uniforms");
		if (json_uniforms == nullptr || !json_uniforms->is_object())
			return false;
		// enumerate all uniforms
		for (nlohmann::json::const_iterator it = json_uniforms->begin(); it != json_uniforms->end(); ++it)
		{
			std::string const & uniform_name = it.key();
			if (!uniform_name.empty())
				AddUniformToRenderMaterial(material_info, uniform_name.c_str(), *it);
		}
		return true;
	}

	bool GPURenderMaterialLoader::InitializeRenderPassesFromJSON(GPURenderMaterial * render_material, GPURenderMaterialInfo * material_info, nlohmann::json const & json) const
	{
		// iterate over all properties
		for (nlohmann::json::const_iterator it = json.begin(); it != json.end(); it++)
		{
			// a renderpass must be an object
			if (!it->is_object())
				continue;

			// search whether property is a renderpass
			std::string const & property_name = it.key();
			if (StringTools::Strncmp(property_name, "renderpass:", 11) != 0)
				continue;

			// create a new material info
			GPURenderMaterialInfo * other_material_info = new GPURenderMaterialInfo;
			if (other_material_info == nullptr)
				continue;
			if (!InitializeMaterialInfoFromJSON(render_material, other_material_info, it.value()))
			{
				delete (other_material_info);
				continue;
			}

			// register the child material info
			NameFilter name_filter;
			name_filter.AddEnabledNames(property_name.c_str() + 11);

			GPURenderMaterialInfoEntry entry;
			entry.filter = std::move(name_filter);
			entry.material_info = other_material_info;
			material_info->renderpasses.push_back(std::move(entry));
		}
		return true;
	}

	bool GPURenderMaterialLoader::InitializeMaterialInfoFromJSON(GPURenderMaterial * render_material, GPURenderMaterialInfo * material_info, nlohmann::json const & json) const
	{
		assert(render_material != nullptr);
		assert(material_info != nullptr);

		if (!json.is_object())
			return false;

		// search material parent
		std::string parent_name;
		if (reference_solver != nullptr &&  JSONTools::GetAttribute(json, "parent_material", parent_name) && !parent_name.empty())
			reference_solver->AddInheritance(render_material, material_info, std::move(parent_name));
		// read filter names
		material_info->filter_specified = JSONTools::GetAttribute(json, "filter", material_info->filter);
		// search whether the material is hidden
		material_info->hidden_specified = JSONTools::GetAttribute(json, "hidden", material_info->hidden, false);
		// search program
		InitializeProgramFromJSON(material_info, json);
		// look at textures
		InitializeTexturesFromJSON(material_info, json);
		// look at uniforms
		InitializeUniformsFromJSON(material_info, json);
		// look at renderpasses
		InitializeRenderPassesFromJSON(render_material, material_info, json);

		return true;
	}

	bool GPURenderMaterialLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager != nullptr && manager->FindRenderMaterialByPath(path) != nullptr);
	}

	bool GPURenderMaterialLoader::IsNameAlreadyUsedInManager(ObjectRequest request) const
	{
		return (manager != nullptr && manager->FindRenderMaterial(request) != nullptr);
	}

	GPURenderMaterial * GPURenderMaterialLoader::LoadObject(char const * name, nlohmann::json const & json) const
	{
		// check for name
		if (!CheckResourceName(nullptr, name, &json))
			return nullptr;

		// indirect call
		std::string path;
		if (JSONTools::GetAttribute(json, "path", path))
			return LoadObject(path);

		// create a new material
		GPURenderMaterial * result = new GPURenderMaterial;
		if (result == nullptr)
			return nullptr;

		// Initialize the material_info from JSON
		InitializeMaterialInfoFromJSON(result, result->material_info.get(), json);

		// finalize : give name / path to the new resource
		ApplyNameToLoadedResource(result);
		ApplyPathToLoadedResource(result);
		if (manager != nullptr)
			if (!StringTools::IsEmpty(result->GetName()))
				manager->render_materials.push_back(result);

		return result;
	}

	GPURenderMaterial * GPURenderMaterialLoader::LoadObject(FilePathParam const & path, char const * name) const
	{
		// check for path
		if (!CheckResourcePath(path))
			return nullptr;
		// check for name
		if (!CheckResourceName(&path.GetResolvedPath(), name, nullptr))
			return nullptr;
		// the file for material is in JSON format
		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json, true))
			return LoadObject(nullptr, json);
		return nullptr;
	}

}; // namespace chaos
