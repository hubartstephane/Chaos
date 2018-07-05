#include <chaos/GPURenderMaterial.h>


namespace chaos
{
	bool GPUProgramRenderMaterialProvider::DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const
	{
		// use extra provider
		if (other_provider != nullptr)
			if (other_provider->DoProcessAction(name, action, other_provider))
				return true;

		// use variables inside this provider
		if (GPUProgramProvider::DoProcessAction(name, action, top_provider))
			return true;

		// use the provider of each materials
		GPURenderMaterial const * rm = render_material;
		while (rm != nullptr)
		{
			if (rm->uniform_provider.DoProcessAction(name, action, top_provider))
				return true;
			rm = rm->parent_material.get();
		}
		return false;
	}

	GPURenderMaterial::GPURenderMaterial()
	{

	}

	GPURenderMaterial::~GPURenderMaterial()
	{
		DoRelease();
	}

	bool GPURenderMaterial::DoRelease()
	{
		program = nullptr;
		parent_material = nullptr;
		uniform_provider.Clear();
		return true;
	}


	bool GPURenderMaterial::SetProgram(GPUProgram * in_program)
	{
		program = in_program;
		return true;
	}

	bool GPURenderMaterial::SetParentMaterial(GPURenderMaterial * in_parent)
	{
		// ensure no cycle parenting
		GPURenderMaterial * rm = in_parent;
		while (rm != nullptr)
		{
			if (rm == this)
				return false; // cycle detected
			rm = rm->parent_material.get();
		}
		parent_material = in_parent;
		return true;
	}

	GPUProgram const * GPURenderMaterial::GetEffectiveProgram() const
	{
		// go through the hierarchy until we get the program
		GPURenderMaterial const * rm = this;
		while (rm != nullptr && rm->program == nullptr)
			rm = rm->parent_material.get();
		if (rm == nullptr)
			return nullptr;
		return rm->program.get();
	}

	bool GPURenderMaterial::UseMaterial(GPUProgramProviderBase const * in_uniform_provider) const
	{
		// go through the hierarchy until we get the program
		GPUProgram const * effective_program = GetEffectiveProgram();
		if (effective_program == nullptr)
			return false;

		// use the program
		GPUProgramRenderMaterialProvider provider(this, in_uniform_provider);
		effective_program->UseProgram(&provider);

		return true;
	}

}; // namespace chaos
