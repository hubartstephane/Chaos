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

	GPURenderMaterial * GPURenderMaterial::FindSubMaterial(char const * submaterial_name)
	{
		for (auto & it : sub_materials)
			if (_stricmp(it.first.c_str(), submaterial_name) == 0)
				return it.second.get();
		return nullptr;
	}

	GPURenderMaterial const * GPURenderMaterial::FindSubMaterial(char const * submaterial_name) const
	{
		for (auto & it : sub_materials)
			if (_stricmp(it.first.c_str(), submaterial_name) == 0)
				return it.second.get();
		return nullptr;
	}

	GPUProgram const * GPURenderMaterial::GetEffectiveProgram(RenderParams const & render_params) const
	{
		// sub-materials
		if (render_params.submaterial_name.empty())
		{
			GPURenderMaterial const * submaterial = FindSubMaterial(render_params.submaterial_name.c_str());
			if (submaterial != nullptr)
			{
				GPUProgram const * result = submaterial->GetEffectiveProgram(render_params);
				if (result != nullptr)
					return result;
			}
		}
		// our own program ?
		if (program != nullptr)
			return program.get();
		// go through the hierarchy until we get the program
		if (parent_material != nullptr)
			return parent_material->GetEffectiveProgram(render_params);
		// not found
		return nullptr;
	}

	GPUProgram const * GPURenderMaterial::UseMaterial(GPUProgramProviderBase const * in_uniform_provider, RenderParams const & render_params) const
	{
		// go through the hierarchy until we get the program
		GPUProgram const * effective_program = GetEffectiveProgram(render_params);
		if (effective_program == nullptr)
			return nullptr;
		// use the program
		GPUProgramRenderMaterialProvider provider(this, in_uniform_provider);
		effective_program->UseProgram(&provider);

		return effective_program;
	}

	GPURenderMaterial * GPURenderMaterial::GenRenderMaterialObject(GPUProgram * program)
	{
		if (program == nullptr)
			return nullptr;
		// create the material
		GPURenderMaterial * result = new GPURenderMaterial();
		if (result == nullptr)
			return nullptr;
		// initialize the program
		result->SetProgram(program);
		return result;
	}

}; // namespace chaos
