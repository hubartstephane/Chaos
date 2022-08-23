#include "chaos/Chaos.h"

namespace chaos
{

	GPUProgram::GPUProgram(GLuint in_id, GPUProgramType in_type): 
		program_id(in_id),
		type(in_type)
	{
		if (in_id != 0)
			program_data = GPUProgramData::GetData(in_id);
	}

	GPUProgram::~GPUProgram()
	{
		Release();
	}

	void GPUProgram::Release()
	{
		if (program_id != 0)
		{
			glDeleteProgram(program_id);
			program_id = 0;
			program_data.Clear();
			default_material = nullptr;
		}
	}

	bool GPUProgram::UseProgram(class GPUProgramProviderInterface const * uniform_provider) const
	{
		if (!IsValid())
			return false;
		glUseProgram(program_id);
		program_data.BindUniforms(uniform_provider);
		return true;
	}

	GPURenderMaterial* GPUProgram::GetDefaultMaterial() const
	{ 
		return default_material.get();
	}

}; // namespace chaos
