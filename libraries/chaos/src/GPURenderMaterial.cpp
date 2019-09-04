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
		for (GPUSubMaterialEntry const & entry : sub_materials)
			if (entry.material->SearchRenderMaterialCycle(searched_material))
				return true;
		return false;
	}

	bool GPURenderMaterial::SetSubMaterial(NameFilter filter, GPURenderMaterial * submaterial)
	{
		assert(submaterial != nullptr);
		// can access 'this' with element about to be inserted ?
		if (submaterial->SearchRenderMaterialCycle(this))
			return false;
		// insertion as a sub material
		GPUSubMaterialEntry entry;
		entry.filter = std::move(filter);
		entry.material = submaterial;
		sub_materials.push_back(std::move(entry));
		return true;
	}

	GPURenderMaterial * GPURenderMaterial::FindSubMaterial(char const * submaterial_name)
	{
		for (GPUSubMaterialEntry const & entry : sub_materials)
			if (entry.filter.IsNameEnabled(submaterial_name))
				return entry.material.get();
		return nullptr;
	}

	GPURenderMaterial const * GPURenderMaterial::FindSubMaterial(char const * submaterial_name) const
	{
		for (GPUSubMaterialEntry const & entry : sub_materials)
			if (entry.filter.IsNameEnabled(submaterial_name))
				return entry.material.get();
		return nullptr;
	}





	GPURenderMaterial const * GPURenderMaterial::GetParentMaterialValidityLimit(GPURenderParams const & render_params) const
	{
		GPURenderMaterial const * result_hidden = nullptr; // to save whether theses values are specified and produce a visibility result
		GPURenderMaterial const * result_filter = nullptr;

		int result_hidden_counter = 0;
		int result_filter_counter = 0;
		int counter = 0;

		GPURenderMaterial const * material = this;
		while (material != nullptr)
		{
			++counter;

			// check HIDDEN state
			bool match_hidden = true;
			if (material->hidden_specified)
			{
				match_hidden = !material->hidden_material;
				if (match_hidden) // save the last material that was known has NOT HIDDEN
				{
					result_hidden = material;
					result_hidden_counter = counter;
				}
			}
			// check FILTER
			bool match_filter = true;
			if (material->filter_specified)
			{
				match_filter = material->filter.IsNameEnabled(render_params.renderpass_name.c_str());
				if (match_filter) // save the last material that match the FILTER criteria
				{
					result_filter = material;
					result_filter_counter = counter;
				}
			}
			// at this point one of the two tests is invalid, keep the very first on the chain
			if (!match_hidden || !match_filter) 
			{
				GPURenderMaterial const * result = (result_hidden_counter < result_filter_counter) ? result_hidden : result_filter;
				if (result != nullptr)
					return result->parent_material.get(); // the parent is the first failing material
				else
					return this; // the very first material of thee chain was invalid
			}
			// parent
			material = material->parent_material.get();
		}
		return nullptr; // all parents are visible
	}






	GPURenderMaterial const * GPURenderMaterial::GetEffectiveMaterial(GPURenderParams const & render_params) const
	{
		GPURenderMaterial const * validity_limit_material = GetParentMaterialValidityLimit(render_params);

		GPURenderMaterial const * material = this;
		while (material != validity_limit_material)
		{
			for (GPUSubMaterialEntry const & entry : material->sub_materials) // search in sub_material first
			{
				if (entry.filter.IsNameEnabled(render_params.renderpass_name.c_str()))
				{
					GPURenderMaterial const * result = entry.material->GetEffectiveMaterial(render_params);
					if (result != nullptr)
						return result;
				}
			}
			material = material->parent_material.get();
		}
		return (this != validity_limit_material)? this : nullptr;
	}


	GPUProgram const * GPURenderMaterial::GetEffectiveProgram(GPURenderParams const & render_params) const
	{
		GPURenderMaterial const * effective_material = GetEffectiveMaterial(render_params);
		while (effective_material != nullptr)
		{
			if (effective_material->program != nullptr)
				return effective_material->program.get();
			effective_material = effective_material->parent_material.get();
		}
		return nullptr;
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
