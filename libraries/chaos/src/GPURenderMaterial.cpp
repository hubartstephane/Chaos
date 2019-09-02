#include <chaos/GPURenderMaterial.h>
#include <chaos/GPURenderParams.h>
#include <chaos/StringTools.h>

namespace chaos
{
	bool GPUProgramRenderMaterialProvider::DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const
	{



		 // shuyyy



		// use extra provider
		if (other_provider != nullptr)
			if (other_provider->DoProcessAction(name, action, top_provider))
				return true;

		// submaterials
		if (render_params != nullptr && !render_params->renderpass_name.empty())
		{
			GPURenderMaterial const * submaterial = render_material->FindSubMaterial(render_params->renderpass_name.c_str());
			if (submaterial != nullptr)
			{
				GPUProgramRenderMaterialProvider submaterial_provider(submaterial, nullptr, render_params); // no more other => it has already been called in this function
				if (submaterial_provider.DoProcessAction(name, action, top_provider))
					return true;
			}
		}
		// search in the materials uniforms
		if (render_material->uniform_provider.DoProcessAction(name, action, top_provider))
			return true;
		// use variables inside this provider (should be empty)
		if (GPUProgramProvider::DoProcessAction(name, action, top_provider))
			return true;
		// try parent 
		if (render_material->parent_material != nullptr)
		{
			GPUProgramRenderMaterialProvider parent_provider(render_material->parent_material.get(), nullptr, render_params); // no more other => it has already been called in this function
			if (parent_provider.DoProcessAction(name, action, top_provider))
				return true;

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

		// can access 'this' with parent about to be set ?
		if (in_parent != nullptr && in_parent->SearchRenderMaterialCycle(this))
			return false;
		// set the parent
		parent_material = in_parent;

		return true;
	}

	bool GPURenderMaterial::SearchRenderMaterialCycle(GPURenderMaterial const * searched_material) const
	{
		if (this == searched_material)
			return true;
		// recursion with parents
		if (parent_material != nullptr)
			if (parent_material->SearchRenderMaterialCycle(searched_material))
				return true;
		// recursion with sub materials
		for (auto const & sub : sub_materials)
			if (sub.second->SearchRenderMaterialCycle(searched_material))
				return true;
		return false;
	}

	bool GPURenderMaterial::SetSubMaterial(char const * submaterial_name, GPURenderMaterial * submaterial)
	{
		assert(submaterial != nullptr);
		assert(submaterial_name != nullptr);
		// can access 'this' with element about to be inserted ?
		if (submaterial->SearchRenderMaterialCycle(this))
			return false;
		// insertion as a sub material
		sub_materials.push_back(std::make_pair(submaterial_name, submaterial));
		return true;
	}

	GPURenderMaterial * GPURenderMaterial::FindSubMaterial(char const * submaterial_name)
	{
		for (auto & it : sub_materials)
			if (StringTools::Stricmp(it.first, submaterial_name) == 0)
				return it.second.get();
		return nullptr;
	}

	GPURenderMaterial const * GPURenderMaterial::FindSubMaterial(char const * submaterial_name) const
	{
		for (auto & it : sub_materials)
			if (StringTools::Stricmp(it.first, submaterial_name) == 0)
				return it.second.get();
		return nullptr;
	}

	GPUProgram const * GPURenderMaterial::DoGetEffectiveProgram(GPURenderParams const & render_params, bool submaterial_encoutered) const
	{
		// sub-materials
		if (!render_params.renderpass_name.empty())
		{
			GPURenderMaterial const * submaterial = FindSubMaterial(render_params.renderpass_name.c_str());
			if (submaterial != nullptr)
			{
				if (submaterial->hidden_material) // do not render with this material (do not test for this->program & parent_material->program)
					return nullptr;
				GPUProgram const * result = submaterial->DoGetEffectiveProgram(render_params, true);
				if (result != nullptr)
					return result;
			}
		}



		// shuyyy







		// our own program ?
		if (hidden_material) // do not render with this material (do not test for parent_material->program)
			return nullptr;
		if (submaterial_encoutered || !strict_submaterial || render_params.renderpass_name.empty())
			if (program != nullptr)
				return program.get();
		// go through the hierarchy until we get the program
		if (parent_material != nullptr)
			return parent_material->DoGetEffectiveProgram(render_params, submaterial_encoutered);
		// not found
		return nullptr;	
	}

	GPUProgram const * GPURenderMaterial::GetEffectiveProgram(GPURenderParams const & render_params) const
	{
		if (!IsMaterialEnabled(render_params))
			return nullptr;
		return DoGetEffectiveProgram(render_params, false);
	}

	bool GPURenderMaterial::IsMaterialEnabled(GPURenderParams const & render_params) const
	{	
		return true;
	}

	GPUProgram const * GPURenderMaterial::UseMaterial(GPUProgramProviderBase const * in_uniform_provider, GPURenderParams const & render_params) const
	{
		// go through the hierarchy until we get the program
		GPUProgram const * effective_program = GetEffectiveProgram(render_params);
		if (effective_program == nullptr)
			return nullptr;
		// use the program
		GPUProgramRenderMaterialProvider provider(this, in_uniform_provider, &render_params);
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
