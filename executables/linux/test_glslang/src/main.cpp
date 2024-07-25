#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/Public/ResourceLimits.h"


int main(int argc, char ** argv)
{
	const char* EsslVersion = glslang::GetEsslVersionString();
	const char* GlslVersion = glslang::GetGlslVersionString();
	
	std::cout << "EsslVersion:" << EsslVersion << std::endl;
	std::cout << "GlslVersion:" << GlslVersion << std::endl;

	return 0;
}
 