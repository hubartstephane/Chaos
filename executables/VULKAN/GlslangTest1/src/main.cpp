#include <chaos/chaos.h>

// https://github.com/KhronosGroup/glslang/blob/master/README.md

#include "glslang/include/glslang_c_interface.h"
#include "glslang/public/ShaderLang.h"
#include "glslang/public/ResourceLimits.h"

char const * pixel_shader_source= R"SHADER_SOURCE(

	#version 450
	
	layout(location = 0) out vec4 outColor;

	void main()
	{
		outColor = vec4(1.0, 0.0, 0.0, 1.0);
	}

)SHADER_SOURCE";


int main(int argc, char ** argv, char ** env)
{
	const char* EsslVersion = glslang::GetEsslVersionString();
	const char* GlslVersion = glslang::GetGlslVersionString();

	glslang_initialize_process();


	glslang_stage_t shader_stage = glslang_stage_t::GLSLANG_STAGE_FRAGMENT;

	const glslang_input_t input =
	{
		.language = GLSLANG_SOURCE_GLSL,
		.stage = shader_stage,


		//.client = GLSLANG_CLIENT_VULKAN,
		//.client_version = GLSLANG_TARGET_VULKAN_1_2,

		.client = GLSLANG_CLIENT_OPENGL,
		
		.client_version = GLSLANG_TARGET_OPENGL_450,



		.target_language = GLSLANG_TARGET_SPV,
		.target_language_version = GLSLANG_TARGET_SPV_1_5,
		.code = pixel_shader_source,
		.default_version = 100,
		.default_profile = GLSLANG_NO_PROFILE,
		.force_default_version_and_profile = false,
		.forward_compatible = false,
		.messages = GLSLANG_MSG_DEFAULT_BIT,
		.resource = reinterpret_cast<const glslang_resource_t*>(GetDefaultResources())
	};

	if (glslang_shader_t* shader = glslang_shader_create(&input))
	{

		if (!glslang_shader_preprocess(shader, &input))
		{
			printf("GLSL preprocessing failed\n");


			auto * aaa = glslang_shader_get_info_log(shader);
			auto * bbb = glslang_shader_get_info_debug_log(shader);

			printf("%s\n", glslang_shader_get_info_log(shader));
			printf("%s\n", glslang_shader_get_info_debug_log(shader));
			printf("%s\n", input.code);
			glslang_shader_delete(shader);
			return 0;
		}

		if (!glslang_shader_parse(shader, &input))
		{
			printf("GLSL parsing failed\n");
			printf("%s\n", glslang_shader_get_info_log(shader));
			printf("%s\n", glslang_shader_get_info_debug_log(shader));
			printf("%s\n", glslang_shader_get_preprocessed_code(shader));
			glslang_shader_delete(shader);
			return 0;
		}


		glslang_program_t* program = glslang_program_create();
		glslang_program_add_shader(program, shader);

		if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
		{
			printf("GLSL linking failed\n");
			printf("%s\n", glslang_program_get_info_log(program));
			printf("%s\n", glslang_program_get_info_debug_log(program));
			glslang_program_delete(program);
			glslang_shader_delete(shader);
			return 0;
		}

		glslang_program_SPIRV_generate(program, shader_stage);


		std::vector<uint32_t> outShaderModule(glslang_program_SPIRV_get_size(program));
		glslang_program_SPIRV_get(program, outShaderModule.data());

		auto* aaa = glslang_program_get_info_log(program);
		auto* bbb = glslang_program_get_info_debug_log(program);


		
		if (char const * spirv_messages = glslang_program_SPIRV_get_messages(program))
		{
			spirv_messages = spirv_messages;

		}

		glslang_program_delete(program);
		glslang_shader_delete(shader);

		shader = shader;
	}






	glslang_finalize_process();

    return 0;
}