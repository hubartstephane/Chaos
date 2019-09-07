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
#if 0
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

#endif

		// search in the materials uniforms
		if (render_material->material_info.uniform_provider.DoProcessAction(name, action, top_provider))
			return true;
		// use variables inside this provider (should be empty)
		if (GPUProgramProvider::DoProcessAction(name, action, top_provider))
			return true;
		// try parent 
		if (render_material->material_info.parent_material != nullptr)
		{
			GPUProgramRenderMaterialProvider parent_provider(render_material->material_info.parent_material.get(), nullptr, render_params); // no more other => it has already been called in this function
			if (parent_provider.DoProcessAction(name, action, top_provider))
				return true;

		}
		return false;
	}


	GPURenderMaterialInfoEntry::~GPURenderMaterialInfoEntry() 
	{ 
		if (material_info != nullptr)
			delete(material_info);
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
		material_info.program = nullptr;
		material_info.parent_material = nullptr;
		material_info.uniform_provider.Clear();
		return true;
	}


	bool GPURenderMaterial::SetProgram(GPUProgram * in_program)
	{
		material_info.program = in_program;
		return true;
	}

	bool GPURenderMaterial::SetParentMaterial(GPURenderMaterial * in_parent)
	{
		if (in_parent != nullptr && SearchRenderMaterialCycle(&in_parent->material_info, this))
			return false;
		material_info.parent_material = in_parent;
		return true;
	}

	bool GPURenderMaterial::SearchRenderMaterialCycle(GPURenderMaterialInfo const * material_info, GPURenderMaterial const * searched_material)
	{
		// throught parent
		GPURenderMaterial const * parent = material_info->parent_material.get();
		if (parent != nullptr)
		{
			if (parent == searched_material)
				return true;
			if (SearchRenderMaterialCycle(&parent->material_info, searched_material))
				return true;
		}
		// renderpasses
		for (GPURenderMaterialInfoEntry const & entry : material_info->renderpasses)
			if (SearchRenderMaterialCycle(entry.material_info, searched_material))
				return true;
		return false;
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
			if (material->material_info.hidden_specified)
			{
				match_hidden = !material->material_info.hidden;
				if (match_hidden) // save the last material that was known has NOT HIDDEN
				{
					result_hidden = material;
					result_hidden_counter = counter;
				}
			}
			// check FILTER
			bool match_filter = true;
			if (material->material_info.filter_specified)
			{
				match_filter = material->material_info.filter.IsNameEnabled(render_params.renderpass_name.c_str());
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
					return result->material_info.parent_material.get(); // the parent is the first failing material
				else
					return this; // the very first material of thee chain was invalid
			}
			// parent
			material = material->material_info.parent_material.get();
		}
		return nullptr; // all parents are visible
	}






	GPURenderMaterial const * GPURenderMaterial::GetEffectiveMaterial(GPURenderParams const & render_params) const
	{
		GPURenderMaterial const * validity_limit_material = GetParentMaterialValidityLimit(render_params);




		// shuyyy 

#if 0



		GPURenderMaterial const * material = this;
		while (material != validity_limit_material)
		{
			for (GPUSubMaterialEntry const & entry : material->material_info.sub_materials) // search in sub_material first
			{
				if (entry.filter.IsNameEnabled(render_params.renderpass_name.c_str()))
				{
					GPURenderMaterial const * result = entry.material->GetEffectiveMaterial(render_params);
					if (result != nullptr)
						return result;
				}
			}
			material = material->material_info.parent_material.get();
		}
#endif

		return (this != validity_limit_material)? this : nullptr;
	}
	
	GPUProgramProvider & GPURenderMaterial::GetUniformProvider() 
	{ 
		return material_info.uniform_provider;
	}
	
	GPUProgramProvider const & GPURenderMaterial::GetUniformProvider() const 
	{ 
		return material_info.uniform_provider;
	}

	GPUProgram const * GPURenderMaterial::GetEffectiveProgram(GPURenderParams const & render_params) const
	{
		GPURenderMaterial const * material = this;
		while (material != nullptr)
		{
			if (material->material_info.program != nullptr)
				return material->material_info.program.get();
			material = material->material_info.parent_material.get();
		}
		return nullptr;
	}

	GPUProgram const * GPURenderMaterial::UseMaterial(GPUProgramProviderBase const * in_uniform_provider, GPURenderParams const & render_params) const
	{
		// find the real material that is use throught sub_materials
		GPURenderMaterial const * effective_material = GetEffectiveMaterial(render_params);
		if (effective_material == nullptr)
			return nullptr;
		// go through the hierarchy until we get the program
		GPUProgram const * effective_program = effective_material->GetEffectiveProgram(render_params);
		if (effective_program == nullptr)
			return nullptr;
		// use the program
		GPUProgramRenderMaterialProvider provider(effective_material, in_uniform_provider, &render_params);
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
