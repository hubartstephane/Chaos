#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumMetaData<ShaderType> const ShaderType_metadata =
	{
		{ ShaderType::ANY, "any" },
		{ ShaderType::VERTEX, "vertex" },
		{ ShaderType::FRAGMENT, "fragment" },
		{ ShaderType::GEOMETRY, "geometry" },
		{ ShaderType::TESS_EVALUATION, "tess_evaluation" },
		{ ShaderType::TESS_CONTROL, "tess_control" },
		{ ShaderType::COMPUTE, "compute" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(ShaderType, &ShaderType_metadata, CHAOS_API);


	void GPUProgramGenerator::AddFrameworkSources(ShaderType shader_type, std::vector<char const*> & sources, std::vector<Buffer<char>> & buffers) const
	{
		// XXX : this must be the very first line of the program.
		//       do it here and not in files because it would be difficult to insert macro just after elsewhere
		sources.push_back("#version 450\n");

		// some flags for each vertex of a particle. Usefull for Half pixel correction
		sources.push_back(R"SHARED_SHADER(
		const int BOTTOM_LEFT  = 1;
		const int BOTTOM_RIGHT = 2;
		const int TOP_LEFT     = 3;
		const int TOP_RIGHT    = 4;
		const int CORNER_MASK  = 7;
		const int EIGHT_BITS_MODE = (1 << 4);
		)SHARED_SHADER");

		// some functions to extract flags from the Z component of texcoord and apply half pixel correction
		sources.push_back(R"SHARED_SHADER(
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
			return (flags & EIGHT_BITS_MODE); // remove all other flags (keep only 8bit)
		}
		)SHARED_SHADER");
	}

	GLuint GPUProgramGenerator::DoGenerateShader(ShaderType shader_type, std::vector<char const *> const & sources) const
	{
		// create a shader
		GLuint result = glCreateShader((GLenum)shader_type);
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

	GLuint GPUProgramGenerator::DoGenerateShader(ShaderType shader_type, GeneratorSet const & generators, DefinitionSet const & definitions, std::string const & definitions_string) const
	{
		bool success = false;

		// shared generators
		GeneratorSet const * global_generators = nullptr;

		std::map<ShaderType, GeneratorSet>::const_iterator global_generators_it = shaders.find(ShaderType::ANY);
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
			for (auto const & generator : *global_generators)
			{
				Buffer<char> buffer = generator->GenerateSource(definitions);
				if (buffer == nullptr || buffer.size() == 0)
					continue;
				buffers.push_back(buffer);
				sources.push_back(buffer.data);
			}
		}

		// standard sources
		for (auto const & generator : generators)
		{
			Buffer<char> buffer = generator->GenerateSource(definitions);
			if (buffer == nullptr || buffer.size() == 0)
				continue;
			buffers.push_back(buffer);
			sources.push_back(buffer.data);
		}

		// tweak the pixel shader source
		if (shader_type == ShaderType::FRAGMENT)
		{



		}

		return DoGenerateShader(shader_type, sources);
	}

	GLuint GPUProgramGenerator::GenerateShader(GLuint program, ShaderType shader_type, GeneratorSet const & generators, DefinitionSet const & definitions, std::string const & definitions_string) const
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
		// frag data location (not valid for compute program)
		if (HasRenderShaderSources())
		{
			GLuint color_number = 0;
			glBindFragDataLocation(program, color_number, "output_color");
		}

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
		// early exit
		if (!has_compute_shader && !has_render_shader)
		{
			Log::Error("GPUProgramGenerator::GenProgram(...) no COMPUTE shader nor RENDER shader");
			return 0;
		}
		if (has_compute_shader && has_render_shader)
		{
			Log::Error("GPUProgramGenerator::GenProgram(...) cannot create a program with both COMPUTE shader and both RENDER shader");
			return 0;
		}
		// create openGL program
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
			ShaderType shader_type = shader_generators.first;
			// this type is a joker and does not deserve to generate a shader
			if (shader_type == ShaderType::ANY)
				continue;
			// keep trace whether a vertex shader is provided
			if (shader_type == ShaderType::VERTEX)
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
		// a rendering program requires at least a vertex shader (for Transform & Feedback)
		if (success && !has_vertex_shader && !has_compute_shader)
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
		if (GLuint program_id = GenProgram(definitions))
		{
			GPUProgramType program_type = (HasComputeShaderSources()) ? GPUProgramType::COMPUTE : GPUProgramType::RENDER;
			if (GPUProgram* result = new GPUProgram(program_id, program_type))
			{
				if (program_type == GPUProgramType::RENDER)
					result->default_material = GPURenderMaterial::GenRenderMaterialObject(result, true);
				return result;
			}
		}
		return nullptr;
	}

	void GPUProgramGenerator::Reset()
	{
		shaders.clear();
		has_render_shader = has_compute_shader = false;
	}

	bool GPUProgramGenerator::AddSourceGenerator(ShaderType shader_type, GPUProgramSourceGenerator * generator)
	{
		assert(generator != nullptr);
		// insert the shader (even in case of error -> this will produce an error at program creation)
		shaders[shader_type].push_back(generator);
		// check whether there is both a compute and a rendering shader
		has_compute_shader |= (shader_type == ShaderType::COMPUTE);
		has_render_shader  |= (shader_type != ShaderType::COMPUTE && shader_type != ShaderType::ANY);
		if (has_compute_shader && has_render_shader)
		{
			Log::Warning("GPUProgramGenerator::AddSourceGenerator(...) cannot have both compute and rendering shaders");
			return false;
		}
		return true;
	}

	bool GPUProgramGenerator::AddShaderSource(ShaderType shader_type, Buffer<char> buffer)
	{
		return AddSourceGenerator(shader_type, new GPUProgramStringSourceGenerator(buffer));
	}

	bool GPUProgramGenerator::AddShaderSource(ShaderType shader_type, char const * src)
	{
		return AddSourceGenerator(shader_type, new GPUProgramStringSourceGenerator(src));
	}

	bool GPUProgramGenerator::AddShaderSourceFile(ShaderType shader_type, FilePathParam const & path)
	{
		return AddSourceGenerator(shader_type, new GPUProgramFileSourceGenerator(path));
	}


}; // namespace chaos
