#include <chaos/GPUProgram.h>
#include <chaos/GPUProgramVariableProvider.h>

namespace chaos
{

	GPUProgram::GPUProgram(GLuint in_id): 
		program_id(in_id)
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
		}  
	}

	bool GPUProgram::UseProgram(class GPUProgramVariableProvider const * uniform_provider, class GPUProgramVariableProvider * attribute_provider)
	{
		if (!IsValid())
			return false;
		glUseProgram(program_id);
		program_data.BindUniforms(uniform_provider);
		return true;
	}

}; // namespace chaos
