#include <chaos/chaos.h>

// https://github.com/KhronosGroup/glslang/blob/master/README.md

#include "glslang/include/glslang_c_interface.h"
#include "glslang/public/ShaderLang.h"
#include "glslang/public/ResourceLimits.h"


//#include "./../glslang/Include/ShHandle.h"
//#include "./../glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/SPIRV/GLSL.std.450.h"
#include "glslang/SPIRV/doc.h"
#include "glslang/SPIRV/disassemble.h"


char const * pixel_shader_source= R"SHADER_SOURCE(

	#version 450 core
	#extension GL_GOOGLE_include_directive: enable
	#extension GL_GOOGLE_cpp_style_line_directive: enable
	#include "toto.h"
	#include <titi.h>

	layout(location = 0) out vec4 outColor;

	layout(binding=0) uniform MyUniforms
	{
		float gray;
	};

	uniform float alpha;

	void main()
	{
		outColor = vec4(gray, gray, gray, alpha);
	}

)SHADER_SOURCE";


char const* vertex_shader_source = R"SHADER_SOURCE(

	#version 450 core

	out gl_PerVertex
	{
		vec4 gl_Position;
	};

	layout(location = 0) in vec4 position;

	void main()
	{
		gl_Position = position;
		//gl_Position = vec4(1.0, 1.0, 1.0, 1.0);
	}

)SHADER_SOURCE";



// DirStackFileIncluder: public glslang::TShader::Includer

// #include "..."
//   1->call includeLocal
//   2->call includeSystem
// #include <...>
//   1->call includeSystem

class MyIncluder : public glslang::TShader::Includer
{
public:

	virtual IncludeResult* includeSystem(const char* headerName, const char* includerName, size_t inclusionDepth)
	{
		char const* src = "void f(){};";

		return new IncludeResult(
			headerName,
			strdup(src),
			strlen(src),
			nullptr);
	}

	virtual IncludeResult* includeLocal(const char* headerName, const char* includerName, size_t inclusionDepth)
	{
		char const* src = "void g(){};";

		return new IncludeResult(
			headerName,
			strdup(src),
			strlen(src),
			nullptr);
	}

	virtual void releaseInclude(IncludeResult * include_result)
	{
		if (include_result != nullptr)
		{
			delete include_result->headerData;
			delete(include_result);
		}
	}
};


glslang::TProgram* CreateProgram(std::map<EShLanguage, char const*> const& shaders)
{
	EShMessages messages = EShMessages(EShMsgRelaxedErrors | EShMsgAST | EShMsgSpvRules | EShMsgVulkanRules | EShMsgReadHlsl | EShMsgCascadingErrors | EShMsgDebugInfo | EShMsgEnhanced);

	auto OnFailure = [](glslang::TProgram* program, glslang::TShader* shader, char const * step_name)
	{
		if (shader != nullptr)
		{
			std::string shader_info = shader->getInfoLog();
			std::string shader_debug_info = shader->getInfoDebugLog();

			if (!shader_info.empty() || !shader_debug_info.empty())
				chaos::Log::Message("Failure: %s", step_name);
			if (!shader_info.empty())
				chaos::Log::Message("shader_info:\n%s", shader_info.c_str());
			if (!shader_debug_info.empty())
				chaos::Log::Message("shader_debug_info:\n%s", shader_debug_info.c_str());

			delete(shader);
		}

		if (program != nullptr)
		{
			std::string program_info = program->getInfoLog();
			std::string program_debug_info = program->getInfoDebugLog();

			if (!program_info.empty() || !program_debug_info.empty())
				chaos::Log::Message("Failure: %s", step_name);
			if (!program_info.empty())
				chaos::Log::Message("program_info:\n%s", program_info.c_str());
			if (!program_debug_info.empty())
				chaos::Log::Message("program_debug_info:\n%s", program_debug_info.c_str());

			delete(program);
		}
		return nullptr;
	};

	if (glslang::TProgram* program = new glslang::TProgram)
	{
		// insert the shaders
		for (auto it : shaders)
		{
			if (glslang::TShader* shader = new glslang::TShader(it.first))
			{
				char const* shader_strings[] = { it.second };
				int const shader_strings_length[] = { int(strlen(it.second)) };
				char const* shader_names[] = { "shader_source1" };

				shader->setStringsWithLengthsAndNames(shader_strings, shader_strings_length, shader_names, 1);



				//shader->setEnvInput(glslang::EShSourceGlsl, it.first, glslang::EShClientVulkan, 110); // important so that we know how to compile

				shader->setEnvInput(glslang::EShSourceGlsl, it.first, glslang::EShClientOpenGL, 450); // important so that we know how to compile





				shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0); // ??? we have source language below, and client language. what is this ?

				shader->setEntryPoint("main");
				shader->setDebugInfo(true);
				shader->setEnhancedMsgs();
				shader->setOverrideVersion(460);
				shader->setAutoMapBindings(true);
				shader->setAutoMapLocations(true);

				TBuiltInResource const* resources = GetDefaultResources(); // there is a function in ShaderLang to avoid some default settings

				bool forward_compatible = false;
				int  default_version = 110;
				bool force_default_version_and_profile = false;

				EProfile profile = ECoreProfile;

				MyIncluder includer;

				std::string output;
				if (shader->preprocess(resources, default_version, profile, force_default_version_and_profile, forward_compatible, messages, &output, includer))
				{
					chaos::Log::Message("%s", output.c_str());

					if (shader->parse(resources, default_version, forward_compatible, messages, includer)) // optional includer
					{
						program->addShader(shader);
					}
					else
					{
						return OnFailure(program, shader, "parse shader");
					}
				}
				else
				{
					return OnFailure(program, shader, "preprocess shader");
				}
			}
			else
			{
				return OnFailure(program, nullptr, "new shader");
			}
		}
		// build the program
		if (program->link(messages))
		{
			if (program->mapIO())
			{
				program->buildReflection(EShReflectionAllIOVariables);
				return program;
			}
			else
			{
				return OnFailure(program, nullptr, "program mapIO");
			}
		}
		else
		{
			return OnFailure(program, nullptr, "program link");
		}
		return nullptr;
	}
	return OnFailure(nullptr, nullptr, "new program");
}

void DumpProgramInternals(glslang::TProgram * program)
{
	program->dumpReflection();

	int uniform_count = program->getNumLiveUniformVariables();
	for (int i = 0; i < uniform_count; ++i)
	{
		glslang::TObjectReflection const& uniform = program->getUniform(i);

		glslang::TType const* type = uniform.getType();
		auto p = uniform.getBinding();

		chaos::Log::Message("uniform: %d", i);
		chaos::Log::Message("    name:                %s", uniform.name.c_str());
		chaos::Log::Message("    offset:              %d", uniform.offset);
		chaos::Log::Message("    glDefineType:        %d", uniform.glDefineType);
		chaos::Log::Message("    size:                %d", uniform.size);
		chaos::Log::Message("    index:               %d", uniform.index);
		chaos::Log::Message("    counterIndex:        %d", uniform.counterIndex);
		chaos::Log::Message("    numMembers:          %d", uniform.numMembers);
		chaos::Log::Message("    arrayStride:         %d", uniform.arrayStride);
		chaos::Log::Message("    topLevelArraySize:   %d", uniform.topLevelArraySize);
		chaos::Log::Message("    topLevelArrayStride: %d", uniform.topLevelArrayStride);
	}

	int uniform_block_count = program->getNumLiveUniformBlocks();
	for (int i = 0; i < uniform_block_count; ++i)
	{
		glslang::TObjectReflection const& uniform = program->getUniformBlock(i);




	}





	int attribute_count = program->getNumLiveAttributes();
	for (int i = 0; i < attribute_count; ++i)
	{
		char const* attribute_name = program->getAttributeName(i);
		int attribute_type = program->getAttributeType(i);

		i = i;
	}





}










int main(int argc, char ** argv, char ** env)
{
	chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	glslang::InitializeProcess();

	std::map<EShLanguage, char const*> shaders =
	{
		{EShLangFragment, pixel_shader_source},
		{EShLangVertex, vertex_shader_source}
	};

	if (glslang::TProgram* program = CreateProgram(shaders))
	{
		std::vector<unsigned int> spirv;
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;

		spvOptions.stripDebugInfo = true;
		spvOptions.disableOptimizer = true;
		spvOptions.optimizeSize = true;
		spvOptions.disassemble = true;
		spvOptions.validate = true;
		glslang::GlslangToSpv(*program->getIntermediate((EShLanguage)EShLangVertex), spirv, &logger, &spvOptions);


		glslang::TIntermediate * inter = program->getIntermediate((EShLanguage)EShLangVertex);

		DumpProgramInternals(program);



		delete (program);
	}

	glslang::FinalizeProcess();

    return 0;
}