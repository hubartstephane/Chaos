#include <chaos/MathTools.h>
#include <chaos/BoostTools.h>
#include <chaos/GPUTextureLoader.h>
#include <chaos/GPURenderMaterialLoader.h>
#include <chaos/GPUProgramLoader.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{

	// ===========================================================================
	// GPURenderMaterialLoaderReferenceSolver
	// ===========================================================================

	void GPURenderMaterialLoaderReferenceSolver::AddInheritance(GPURenderMaterial * render_material, std::string parent_name)
	{
		assert(render_material != nullptr);
		assert(!parent_name.empty());

		GPURenderMaterialParentReference reference;
		reference.render_material = render_material;
		reference.parent_name = std::move(parent_name);
		parent_references.push_back(std::move(reference));
	}

	bool GPURenderMaterialLoaderReferenceSolver::ResolveReferences(GPUResourceManager * resource_manager)
	{
		assert(resource_manager != nullptr);
		// resolve parenting
		for (GPURenderMaterialParentReference & ref : parent_references)
			if (ref.render_material->parent_material == nullptr)
				resource_manager->SetRenderMaterialParent(ref.render_material.get(), ref.parent_name.c_str());
		// resolve sub materials
		for (GPURenderMaterialSubMaterialReference & ref : submaterials_references)
		{
			if (ref.is_named_reference)
				resource_manager->SetRenderMaterialSubMaterial(ref.render_material.get(), std::move(ref.filter), ref.reference_name.c_str());
			else
				resource_manager->SetRenderMaterialSubMaterialByPath(ref.render_material.get(), std::move(ref.filter), ref.reference_name.c_str());
		}
		// clear all
		parent_references.clear();
		submaterials_references.clear();

		return true;
	}

	void GPURenderMaterialLoaderReferenceSolver::AddSubMaterialReference(GPURenderMaterial * render_material, NameFilter filter, std::string reference_name, bool is_named_reference)
	{
		assert(render_material != nullptr);
		assert(!reference_name.empty());

		GPURenderMaterialSubMaterialReference reference;
		reference.render_material = render_material;
		reference.filter = std::move(filter);
		reference.reference_name = std::move(reference_name);
		reference.is_named_reference = is_named_reference;
		submaterials_references.push_back(std::move(reference));
	}

		// ===========================================================================
		// GPURenderMaterialLoader
		// ===========================================================================

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
		GPUProgramLoader program_loader(manager);
		GPUProgram * program = program_loader.LoadObject(nullptr, *json_program, program_name.c_str());
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
			GPUTextureLoader texture_loader(manager);
			GPUTexture * texture = texture_loader.LoadObject(nullptr, *it, texture_name.c_str());
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

	bool GPURenderMaterialLoader::InitializeSubMaterialsFromJSON(GPURenderMaterial * render_material, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{
		// search the uniform object
		nlohmann::json const * json_submaterials = JSONTools::GetStructure(json, "sub_materials");
		if (json_submaterials == nullptr)
			return false;

		// 2 cases : 
		//   -array  => a 'filter' property is expected (but not mandatory)
		//   -object => the key correspond to enable filter list

		bool is_array  = json_submaterials->is_array();
		bool is_object = json_submaterials->is_object();
		if (!is_array && !is_object)
			return false;

		// enumerate all sub materials
		for (nlohmann::json::const_iterator it = json_submaterials->begin(); it != json_submaterials->end(); ++it)
		{	
			nlohmann::json const & json_submat = it.value();

			// each element of the structure has to be an object
			if (!json_submat.is_object())
				continue;

			// search the filters
			NameFilter filter;

			if (is_object) // the key for an object is the enable list
			{
				std::string filter_string = it.key();
				filter.AddEnabledNames(filter_string.c_str());
			}
			else if (is_array)
			{
				JSONTools::GetAttribute(json_submat, "filter", filter); // try to read filter as a property
			}

			if (manager != nullptr && reference_solver != nullptr)
			{
				// try to find by path
				std::string sub_path;
				if (JSONTools::GetAttribute(json_submat, "path", sub_path))
				{
					reference_solver->AddSubMaterialReference(render_material, std::move(filter), std::move(sub_path), false); // false => by path
					continue;
				}
				// try to find by name
				std::string sub_name;
				if (JSONTools::GetAttribute(json_submat, "name", sub_name))
				{
					reference_solver->AddSubMaterialReference(render_material, std::move(filter), std::move(sub_name), true); // true => by name
					continue;
				}
			}

			// XXX : 
			//       - GPUSubMaterialEntry has a 'filter' property
			//       - GPURenderMaterial   has a 'filter' property too
			//
			//  there must not be any confusion about that

			// independant material
			GPURenderMaterialLoader other_loader = *this;
			other_loader.insert_in_manager = false;

			shared_ptr<GPURenderMaterial> submaterial = other_loader.LoadObject(nullptr, json_submat, config_path); // may be an error during SetSubMaterial(...) due to reference cycle
			if (submaterial != nullptr)
			{
				if (is_object)
					render_material->SetSubMaterial(std::move(filter), submaterial.get()); // the filter of GPUSubMaterialEntry was given by the KEY
				else if (is_array)
					render_material->SetSubMaterial(NameFilter(), submaterial.get());      // the filter we may have encoutered previously is not for GPUSubMaterialEntry, but is read by the new independant GPURenderMaterial
			}
		}
		return true;
	}

	bool GPURenderMaterialLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager->FindRenderMaterialByPath(path) != nullptr);
	}

	bool GPURenderMaterialLoader::IsNameAlreadyUsedInManager(char const * in_name) const
	{
		return (manager->FindRenderMaterial(in_name) != nullptr);
	}
	GPURenderMaterial * GPURenderMaterialLoader::LoadObject(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path) const
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
		if (result != nullptr)
		{
			// search material parent
			std::string parent_name;
			if (reference_solver != nullptr &&  JSONTools::GetAttribute(json, "parent_material", parent_name) && !parent_name.empty())
				reference_solver->AddInheritance(result, std::move(parent_name));
			
			// read filter names
			result->filter_specified = JSONTools::GetAttribute(json, "filter", result->filter);
			// search whether the material is hidden
			result->hidden_specified = JSONTools::GetAttribute(json, "hidden", result->hidden);
			if (!result->hidden_specified)
				result->hidden = false; // default value
			// search program
			InitializeProgramFromJSON(result, json, config_path);
			// look at textures
			InitializeTexturesFromJSON(result, json, config_path);
			// look at uniforms
			InitializeUniformsFromJSON(result, json, config_path);
			// initialize sub materials
			InitializeSubMaterialsFromJSON(result, json, config_path);

			// finalize : give name / path to the new resource
			ApplyNameToLoadedResource(result);
			ApplyPathToLoadedResource(result);
			if (manager != nullptr && insert_in_manager)
				manager->render_materials.push_back(result);
		}
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
			return LoadObject(nullptr, json, path.GetResolvedPath());
		return nullptr;
	}

}; // namespace chaos
