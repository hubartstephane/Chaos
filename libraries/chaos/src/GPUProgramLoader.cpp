#include <chaos/GPUProgramLoader.h>
#include <chaos/GLTools.h>
#include <chaos/GPUProgramGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/JSONTools.h>

namespace chaos
{

	GPUProgram * GPUProgramLoader::GenProgramObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options) const
	{
		// can only work with json object
		if (!json.is_object())
			return nullptr;

		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(json, "path", p))
		{
			FilePathParam path(p, config_path);
			return GenProgramObject(path, cache_options);
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
			if (!JSONTools::GetAttribute(json, shader_json_names[i], source_path))
				continue;
			FilePathParam path(source_path, config_path);
			program_generator.AddShaderSourceFile(shader_types[i], path);
		}

		// iterate over every shader types for array
		for (size_t i = 0; i < shader_type_count; ++i)
		{
			nlohmann::json const * sources = JSONTools::GetStructure(json, shader_json_array_names[i]);
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
				FilePathParam path(source_path, config_path);
				program_generator.AddShaderSourceFile(shader_types[i], path);
			}
		}

		// generate the program
		return program_generator.GenProgramObject();
	}
	
	GPUProgram * GPUProgramLoader::GenProgramObject(FilePathParam const & path, GPUProgramLoaderCacheOptions const & cache_options) const
	{
		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json, true))
			return GenProgramObject(json, path.GetResolvedPath(), cache_options);	
		return nullptr;
	}

#if 0


#endif

}; // namespace chaos
