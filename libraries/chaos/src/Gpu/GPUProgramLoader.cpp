#include <chaos/Chaos.h>

namespace chaos
{

	GPUProgram * GPUProgramLoader::LoadObject(char const * name, JSONConfig const & config) const
	{
		GPUProgram* result = LoadObjectHelper(name, config, [this](JSONConfig const & config)
		{
			return GenProgramObject(config);
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

	GPUProgram * GPUProgramLoader::GenProgramObject(JSONConfig const & config) const
	{
		// can only work with json object
		if (!config.json.is_object())
			return nullptr;

		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(config.json, "path", p))
		{
			FilePathParam path(p, config.config_path);
			return GenProgramObject(path);
		}

		// the possible types of shaders
		static GLenum const shader_types[] = 
		{ 
			GL_FRAGMENT_SHADER, 
			GL_VERTEX_SHADER,
			GL_GEOMETRY_SHADER,
			GL_TESS_EVALUATION_SHADER,
			GL_TESS_CONTROL_SHADER
		};

		// the attribute names associated
		static char const * shader_json_names[] = // data is a single string
		{
			"pixel_source",
			"vertex_source",
			"geometry_source",
			"tesselation_evaluation_source",
			"tesselation_control_source"
		};
		static char const * shader_json_array_names[] = // data is an array of strings
		{
			"pixel_sources",
			"vertex_sources",
			"geometry_sources",
			"tesselation_evaluation_sources",
			"tesselation_control_sources"
		};

		size_t shader_type_count       = sizeof(shader_types) / sizeof(shader_types[0]);
		size_t shader_name_count       = sizeof(shader_json_names) / sizeof(shader_json_names[0]);
		size_t shader_array_name_count = sizeof(shader_json_array_names) / sizeof(shader_json_array_names[0]);
		assert(shader_type_count == shader_name_count);
		assert(shader_type_count == shader_array_name_count);

		GPUProgramGenerator program_generator;

		// iterate over every shader types (single string)
		for (size_t i = 0; i < shader_type_count; ++i)
		{
			std::string source_path;
			if (!JSONTools::GetAttribute(config.json, shader_json_names[i], source_path))
				continue;
			FilePathParam path(source_path, config.config_path);
			program_generator.AddShaderSourceFile(shader_types[i], path);
		}

		// iterate over every shader types for array
		for (size_t i = 0; i < shader_type_count; ++i)
		{
			nlohmann::json const * sources = JSONTools::GetStructure(config.json, shader_json_array_names[i]);
			if (sources == nullptr)
				continue;
			if (!sources->is_array())
				continue;

			// iterate over all sources for the given shader type
			for (size_t j = 0; j < sources->size(); ++j)
			{
				std::string source_path;
				if (!JSONTools::GetAttributeByIndex(*sources, j, source_path))
					continue;
				FilePathParam path(source_path, config.config_path);
				program_generator.AddShaderSourceFile(shader_types[i], path);
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
		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json, true))
			return GenProgramObject(JSONConfig(json, path.GetResolvedPath()));	
		return nullptr;
	}

}; // namespace chaos
