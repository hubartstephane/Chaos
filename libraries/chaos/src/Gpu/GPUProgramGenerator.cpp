#include <chaos/Chaos.h>

namespace chaos
{
	void GPUProgramGenerator::AddFrameworkSources(GLenum shader_type, std::vector<char const*> & sources, std::vector<Buffer<char>> & buffers) const
	{
		// XXX : this must be the very first line of the program. 
		//       do it here and not in files because it would be difficult to insert macro just after elsewhere    
		sources.push_back("#version 450\n");

		// some flags for each vertex of a particle. Usefull for Half pixel correction 
		sources.push_back(R"SHAREDSHADERCODE(
		const int BOTTOM_LEFT  = 1;
		const int BOTTOM_RIGHT = 2;
		const int TOP_LEFT     = 3;
		const int TOP_RIGHT    = 4;		
		const int CORNER_MASK  = 7;
		const int HEIGHT_BITS_MODE = (1 << 4);			
		)SHAREDSHADERCODE");

		// some functions to extract flags from the Z component of texcoord and apply half pixel correction
		sources.push_back(R"SHAREDSHADERCODE(
		vec2 GetHalfPixelCorrectionOffset(int flags)
		{
			// only values 1, 2, 3 and 4 are valid corners
			vec2 offsets[8] = 
			{
				vec2( 0.0,  0.0),
				vec2(+1.0, +1.0), 
				vec2(-1.0, +1.0),
				vec2(+1.0, -1.0),
				vec2(-1.0, -1.0),
				vec2( 0.0,  0.0),
				vec2( 0.0,  0.0),
				vec2( 0.0,  0.0)
			};
			return offsets[flags & CORNER_MASK]; 
		}
		vec3 HalfPixelCorrection(vec3 texcoord, int flags, sampler2DArray material)
		{
			vec3 result = texcoord;
			result.xy += 0.5 * GetHalfPixelCorrectionOffset(flags) / textureSize(material, 0).xy;
			return result;
		}
		int ExtractFragmentFlags(int flags)
		{
			return (flags & HEIGHT_BITS_MODE); // remove all other flags (keep only 8bit)
		}
		)SHAREDSHADERCODE");
	}

	GLuint GPUProgramGenerator::DoGenerateShader(GLenum shader_type, std::vector<char const *> const & sources) const
	{
		// create a shader
		GLuint result = glCreateShader(shader_type);
		if (result == 0)
		{
			Log::Error("glCreateShader failed");
			return 0;
		}
		// fill with sources
		glShaderSource(result, (int)sources.size(), &sources[0], nullptr);
		// compile the shader
		glCompileShader(result);
		// test whether the compilation is successfull
		GLint compilation_result = 0;
		glGetShaderiv(result, GL_COMPILE_STATUS, &compilation_result);
		if (compilation_result)
			return result;
		// log error message and destroy GL failed resource
		GLint log_len = 0;
		glGetShaderiv(result, GL_INFO_LOG_LENGTH, &log_len);
		if (log_len > 0)
		{
			GLchar log_buffer[4096];
			glGetShaderInfoLog(result, sizeof(log_buffer) - 1, nullptr, log_buffer);

			Log::Error("Shader compilation failure : %s", log_buffer);
		}
		glDeleteShader(result);
		
		return 0;
	}

	GLuint GPUProgramGenerator::DoGenerateShader(GLenum shader_type, GeneratorSet const & generators, DefinitionSet const & definitions, std::string const & definitions_string) const
	{
		bool success = false;

		// shared generators 
		GeneratorSet const * global_generators = nullptr;

		std::map<GLenum, GeneratorSet>::const_iterator global_generators_it = shaders.find(GL_NONE);
		if (global_generators_it != shaders.cend())
			global_generators = &global_generators_it->second;

		// store the 'strings' in one array 
		// store the 'buffers' in a second one
		//  => we do not want the generated strings to becomes invalid due to buffer destruction
		//     the second buffer helps us keep the string valid.

		std::vector<char const *> sources;
		std::vector<Buffer<char>> buffers; // this is important !!!! the GenerateSource(...) function returns Buffer<> whose lifetime is assured because of that


		// extra sources
		AddFrameworkSources(shader_type, sources, buffers);

		// add the definitions
		if (!definitions_string.empty())
			sources.push_back(definitions_string.c_str());

		// shared sources
		if (global_generators != nullptr)
		{
			for (size_t i = 0; i < global_generators->size(); ++i)
			{
				Buffer<char> buffer = global_generators->operator [] (i)->GenerateSource(definitions);
				if (buffer == nullptr || buffer.size() == 0)
					continue;
				buffers.push_back(buffer);
				sources.push_back(buffer.data);
			}
		}

		// standard sources
		size_t previous_source_size = sources.size();

		for (size_t i = 0; i < generators.size(); ++i)
		{
			Buffer<char> buffer = generators[i]->GenerateSource(definitions);
			if (buffer == nullptr || buffer.size() == 0)
				continue;
			buffers.push_back(buffer);
			sources.push_back(buffer.data);
		}



		// no explicit source => early exit
		if (previous_source_size == sources.size())
			return 0;

		// tweak the pixel shader source
		if (shader_type == GL_FRAGMENT_SHADER)
		{



		}

		return DoGenerateShader(shader_type, sources);
	}

	GLuint GPUProgramGenerator::GenerateShader(GLuint program, GLenum shader_type, GeneratorSet const & generators, DefinitionSet const & definitions, std::string const & definitions_string) const
	{
		GLuint result = DoGenerateShader(shader_type, generators, definitions, definitions_string);
		if (result != 0)
		{
			// give program the responsability of shader lifetime
			glAttachShader(program, result);
			glDeleteShader(result);
		}
		return result;
	}

	bool GPUProgramGenerator::PreLinkProgram(GLuint program) const
	{
		// frag data location
		GLuint color_number = 0;
		glBindFragDataLocation(program, color_number, "output_color");

		// XXX : we could force attribute location with glBindAttribLocation(.. name ..) before the link
		//       but we cannot use introspection methods in GPUProgramData::GetData(...) before link
		//       => 2 possibilities :
		//              - we guess the name to give to glBindAttribLocation(...)
		//              - we use a double linkage method
#if 0
		GPUProgramData data = GPUProgramData::GetData(program);
		for (auto & attrib : data.attributes)
		{
			GLint location = GLTools::GetDefaultAttribLocationByName(attrib.name.c_str());
			if (location >= 0)
				glBindAttribLocation(program, location, attrib.name.c_str());
		}
#endif
		return true;
	}

	std::string GPUProgramGenerator::DefinitionsToString(DefinitionSet const & definitions)
	{
		std::string result;

		// reserve a raisonable amount of memory 
		size_t size = 0;
		for (auto const & d : definitions)
			size += d.first.length();
		size += definitions.size() * 15; // for "#define ...\n"
		result.reserve(size);

		// do the concatenation
		for (auto const & d : definitions)
			result = result + StringTools::Printf("#define %s %d\n", d.first.c_str(), d.second);
		return result;
	}

	GLuint GPUProgramGenerator::GenProgram(DefinitionSet const & definitions) const
	{
		GLuint result = glCreateProgram();
		if (result == 0)
		{
			Log::Error("glCreateProgram failed");
			return 0;
		}

		// create a string to contains all definitions
		std::string definitions_string = DefinitionsToString(definitions);

		// create all shaders
		bool success = true;

		bool has_vertex_shader = false;
		for (auto const & shader_generators : shaders)
		{
			GLenum shader_type = shader_generators.first;
			// this type is a joker and does not deserve to generate a shader
			if (shader_type == GL_NONE)
				continue;
			// keep trace whether a vertex shader is provided
			if (shader_type == GL_VERTEX_SHADER)
				has_vertex_shader = true;
			// generate the shader for this TYPE
			GLuint shader_id = GenerateShader(result, shader_type, shader_generators.second, definitions, definitions_string);
			if (shader_id == 0)
			{
				success = false;
				break;
			}
		}


		// complete the program with default vertex shader if not provided
		if (success && !has_vertex_shader)
		{




		


		}

		// link program
		if (success)
		{
			success = PreLinkProgram(result);
			if (success)
			{
				glLinkProgram(result);
				success = (GLShaderTools::CheckProgramStatus(result, GL_LINK_STATUS, "Program link failure : %s") == GL_TRUE);

#if 0 && _DEBUG
				if (success)
					GLShaderTools::DisplayProgramDiagnostic(result);
#endif
			}
		}

		// delete resources in case of error
		if (!success)
		{
			glDeleteProgram(result);
			result = 0;
		}
		return result;
	}

	GPUProgram * GPUProgramGenerator::GenProgramObject(DefinitionSet const & definitions) const
	{
		GLuint program_id = GenProgram(definitions);
		return (program_id == 0) ? nullptr : new GPUProgram(program_id);
	}

	void GPUProgramGenerator::Reset()
	{
		shaders.clear();
	}

	bool GPUProgramGenerator::IsShaderTypeValid(GLenum shader_type)
	{
		return
			(shader_type == GL_NONE) ||  // this is a special value that serves as a joker
			(shader_type == GL_VERTEX_SHADER) ||
			(shader_type == GL_FRAGMENT_SHADER) ||
			(shader_type == GL_GEOMETRY_SHADER) ||
			(shader_type == GL_TESS_EVALUATION_SHADER) ||
			(shader_type == GL_TESS_CONTROL_SHADER) ||
			(shader_type == GL_COMPUTE_SHADER);
	}

	bool GPUProgramGenerator::AddSourceGenerator(GLenum shader_type, GPUProgramSourceGenerator * generator)
	{
		assert(generator != nullptr);
		if (!IsShaderTypeValid(shader_type))
			return false;
		shaders[shader_type].push_back(generator);
		return true;
	}

	bool GPUProgramGenerator::AddShaderSource(GLenum shader_type, Buffer<char> buffer)
	{
		if (!IsShaderTypeValid(shader_type))
			return false;
		return AddSourceGenerator(shader_type, new GPUProgramStringSourceGenerator(buffer));
	}

	bool GPUProgramGenerator::AddShaderSource(GLenum shader_type, char const * src)
	{
		if (!IsShaderTypeValid(shader_type))
			return false;
		return AddSourceGenerator(shader_type, new GPUProgramStringSourceGenerator(src));
	}

	bool GPUProgramGenerator::AddShaderSourceFile(GLenum shader_type, FilePathParam const & path)
	{
		if (!IsShaderTypeValid(shader_type))
			return false;
		return AddSourceGenerator(shader_type, new GPUProgramFileSourceGenerator(path));
	}


}; // namespace chaos
