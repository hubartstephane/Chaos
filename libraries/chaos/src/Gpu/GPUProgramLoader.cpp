#include <chaos/Chaos.h>

namespace chaos
{

	GPUProgram * GPUProgramLoader::LoadObject(char const * name, nlohmann::json const & json) const
	{
		GPUProgram* result = LoadObjectHelper(name, json, [this](nlohmann::json const & json)
		{
			return GenProgramObject(json);
		});
		if (result != nullptr)
		{
			if (manager != nullptr)
				if (!StringTools::IsEmpty(result->GetName())) // would like to insert the resource in manager, but name is empty
					manager->programs.push_back(result);
		}
		return result;
	}

	GPUProgram * GPUProgramLoader::LoadObject(FilePathParam const & path, char const * name) const
	{
		GPUProgram* result = LoadObjectHelper(path, name, [this](FilePathParam const& path)
		{
			return GenProgramObject(path);
		});
		if (result != nullptr)
		{
			if (manager != nullptr)
				if (!StringTools::IsEmpty(result->GetName())) // would like to insert the resource in manager, but name is empty
					manager->programs.push_back(result);
		}
		return result;
	}

	bool GPUProgramLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager != nullptr && manager->FindProgramByPath(path) != nullptr);
	}

	bool GPUProgramLoader::IsNameAlreadyUsedInManager(ObjectRequest request) const
	{ 
		return (manager != nullptr && manager->FindProgram(request) != nullptr);
	}

	GPUProgram * GPUProgramLoader::GenProgramObject(nlohmann::json const & json) const
	{
		// can only work with json object
		if (!json.is_object())
			return nullptr;

		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(json, "path", p))
		{
			FilePathParam path(p);
			return GenProgramObject(path);
		}

		// gather all shaders
		GPUProgramGenerator program_generator;
		
		for (auto it = json.begin(); it != json.end(); ++it) // search of all keys 
		{
			std::string const & property_name = it.key();

			ShaderType shader_type;
			if (StringToEnum(property_name.c_str(), shader_type)) // if the key does correspond to a shader type
			{
				std::vector<boost::filesystem::path> shader_paths;
				if (LoadFromJSON(*it, shader_paths))
					for (auto const& p : shader_paths)
						program_generator.AddShaderSourceFile(shader_type, p); // add the shader
			}
		}
		// generate the program
		return program_generator.GenProgramObject();
	}

	GPUProgram * GPUProgramLoader::GenProgramObject(FilePathParam const & path) const
	{
		// check for path
		if (!CheckResourcePath(path))
			return nullptr;
		// load the file
		boost::filesystem::file_status status = boost::filesystem::status(path.GetResolvedPath());
		if (status.type() == boost::filesystem::file_type::directory_file)
		{
			int i = 0;
			++i;

		}
		else if (status.type() == boost::filesystem::file_type::regular_file)
		{
			nlohmann::json json;
			if (JSONTools::LoadJSONFile(path, json, true))
				return GenProgramObject(json);	
		}
		return nullptr;
	}

}; // namespace chaos
