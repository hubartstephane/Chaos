#include <chaos/Chaos.h>

namespace chaos
{
	GPUProgram * GPUProgramLoader::LoadObject(char const * name, nlohmann::json const & json) const
	{
		return LoadObjectHelper(name, json, [this](nlohmann::json const & json)
		{
			return GenProgramObject(json);
		},
		[this](GPUProgram* program)
		{
			manager->programs.push_back(program);
		});
	}

	GPUProgram * GPUProgramLoader::LoadObject(FilePathParam const & path, char const * name) const
	{
		return LoadObjectHelper(path, name, [this](FilePathParam const& path)
		{
			return GenProgramObject(path);
		},
		[this](GPUProgram * program)
		{
			manager->programs.push_back(program);
		});
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
			return GenProgramObjectFromDirectory(path.GetResolvedPath());
		}
		else if (status.type() == boost::filesystem::file_type::regular_file)
		{
			nlohmann::json json;
			if (JSONTools::LoadJSONFile(path, json, true))
				return GenProgramObject(json);	
		}
		return nullptr;
	}

	GPUProgram* GPUProgramLoader::GenProgramObjectFromDirectory(boost::filesystem::path const & p) const
	{
		// search whether a .pgm files does exists (with the same name than the directory)
		boost::filesystem::path pgm_filename = p / p.filename().replace_extension("pgm");

		nlohmann::json json;
		if (JSONTools::LoadJSONFile(pgm_filename, json, true))
			return GenProgramObject(json);	

		// search all files in the directory
		static std::map<char const*, ShaderType, StringTools::RawStringILess> extension_map =
		{
			{".vsh", ShaderType::VERTEX},
			{".fsh", ShaderType::FRAGMENT},
			{".gsh", ShaderType::GEOMETRY},
			{".tcsh", ShaderType::TESS_CONTROL},
			{".tesh", ShaderType::TESS_EVALUATION},
			{".tcsh", ShaderType::ANY},
			{".csh", ShaderType::COMPUTE}
		};

		GPUProgramGenerator program_generator;

		for (auto it = boost::filesystem::directory_iterator(p); it != boost::filesystem::directory_iterator(); ++it)
		{
			if (it->status().type() == boost::filesystem::file_type::regular_file)
			{
				boost::filesystem::path file_path = it->path();
				std::string ext = file_path.extension().string();

				auto ext_it = extension_map.find(ext.c_str());
				if (ext_it != extension_map.end())
				{
					// add source to the generator
					ShaderType shader_type = ext_it->second;
					if (!program_generator.AddShaderSourceFile(shader_type, file_path))
					{
						Log::Error("GPUProgramLoader::GenProgramObjectFromDirectory(...) fails to add source [%s]", p.c_str());
						return nullptr;
					}
				}
			}
		}
		return program_generator.GenProgramObject();
	}

}; // namespace chaos
