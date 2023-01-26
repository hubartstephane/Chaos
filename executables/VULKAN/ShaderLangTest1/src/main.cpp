#include <chaos/chaos.h>

// https://github.com/KhronosGroup/glslang/blob/master/README.md

#include "glslang/include/glslang_c_interface.h"
#include "glslang/public/ShaderLang.h"
#include "glslang/public/ResourceLimits.h"


char const * pixel_shader_source= R"SHADER_SOURCE(

	#version 450
	#include "toto.h"
	#include <titi.h>
	
	layout(location = 0) out vec4 outColor;

	void main()
	{
		outColor = vec4(1.0, 0.0, 0.0, 1.0);
	}

)SHADER_SOURCE";


// DirStackFileIncluder: public glslang::TShader::Includer


class MyIncluder : public glslang::TShader::Includer
{
public:

	virtual IncludeResult* includeSystem(const char* headerName, const char* includerName, size_t inclusionDepth)
	{
		return nullptr;
	}

	virtual IncludeResult* includeLocal(const char* headerName, const char* includerName, size_t inclusionDepth)
	{
		return nullptr;
	}

	virtual void releaseInclude(IncludeResult * include_result)
	{


		include_result = include_result;
	}
};



int main(int argc, char ** argv, char ** env)
{
	ShInitialize();


	ShHandle compiler = ShConstructCompiler(EShLangFragment, 0);
	std::string log = ShGetInfoLog(compiler);



	ShDestruct(compiler);

	if (glslang::TProgram* program = new glslang::TProgram)
	{
		EShLanguage shader_stage = EShLangFragment;

		if (glslang::TShader* shader = new glslang::TShader(shader_stage))
		{
			char const* shader_strings[] = { pixel_shader_source };
			shader->setStrings(shader_strings, 1);

			shader->setSourceEntryPoint("main");
			shader->setDebugInfo(true);
			shader->setEnhancedMsgs();
			shader->setEnvInput(glslang::EShSourceGlsl, shader_stage, glslang::EShClientVulkan, 110); // important so that we know how to compile
			shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0); // ??? we have source language below, and client language. what is this ?
			shader->setOverrideVersion(460);

			TBuiltInResource const * resources = GetDefaultResources(); // there is a function in ShaderLang to avoid some default settings

			bool forward_compatible = false;
			int default_version = 110;
			bool force_default_version_and_profile = false;
			EShMessages messages = EShMessages(EShMsgRelaxedErrors | EShMsgAST | EShMsgSpvRules | EShMsgVulkanRules | EShMsgReadHlsl | EShMsgCascadingErrors | EShMsgDebugInfo | EShMsgEnhanced);

			EProfile profile = ECoreProfile;

			std::string output;

			//glslang::TShader::ForbidIncluder includer;
			MyIncluder includer;

			if (shader->preprocess(resources, default_version, profile, force_default_version_and_profile, forward_compatible, messages, &output, includer))
			{
				if (shader->parse(resources, default_version, forward_compatible, messages)) // optional includer
				{

					program->addShader(shader);

					if (program->mapIO())
					{
						program = program;

					}

					std::string p1 = program->getInfoLog();
					std::string p2 = program->getInfoDebugLog();

					program->buildReflection(EShReflectionDefault);
					program->dumpReflection();
				}
			}



			std::string s1 = shader->getInfoLog();
			std::string s2 = shader->getInfoDebugLog();


			delete(shader);
		}
		delete(program);
	}
	ShFinalize();

    return 0;
}