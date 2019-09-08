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
		if (render_material->material_info->uniform_provider.DoProcessAction(name, action, top_provider))
			return true;
		// use variables inside this provider (should be empty)
		if (GPUProgramProvider::DoProcessAction(name, action, top_provider))
			return true;
		// try parent 
		if (render_material->material_info->parent_material != nullptr)
		{
			GPUProgramRenderMaterialProvider parent_provider(render_material->material_info->parent_material.get(), nullptr, render_params); // no more other => it has already been called in this function
			if (parent_provider.DoProcessAction(name, action, top_provider))
				return true;

		}
		return false;
	}













	GPURenderMaterial::GPURenderMaterial()
	{
		material_info = new GPURenderMaterialInfo;
	}

	GPURenderMaterial::~GPURenderMaterial()
	{
		DoRelease();
	}

	bool GPURenderMaterial::DoRelease()
	{
		material_info->program = nullptr;
		material_info->parent_material = nullptr;
		material_info->uniform_provider.Clear();
		material_info->renderpasses.clear();
		return true;
	}


	bool GPURenderMaterial::SetProgram(GPUProgram * in_program)
	{
		material_info->program = in_program;
		return true;
	}

	bool GPURenderMaterial::SetParentMaterial(GPURenderMaterial * in_parent)
	{
		if (in_parent != nullptr && SearchRenderMaterialCycle(in_parent->material_info.get(), this))
			return false;
		material_info->parent_material = in_parent;
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
			if (SearchRenderMaterialCycle(parent->material_info.get(), searched_material))
				return true;
		}
		// renderpasses
		for (GPURenderMaterialInfoEntry const & entry : material_info->renderpasses)
			if (SearchRenderMaterialCycle(entry.material_info.get(), searched_material))
				return true;
		return false;
	}

	GPUProgramProvider & GPURenderMaterial::GetUniformProvider() 
	{ 
		return material_info->uniform_provider;
	}
	
	GPUProgramProvider const & GPURenderMaterial::GetUniformProvider() const 
	{ 
		return material_info->uniform_provider;
	}

#if 0
	GPUProgram const * GPURenderMaterial::GetEffectiveProgram(GPURenderParams const & render_params) const
	{
		GPURenderMaterial const * material = this;
		while (material != nullptr)
		{
			if (material->material_info->program != nullptr)
				return material->material_info->program.get();
			material = material->material_info->parent_material.get();
		}
		return nullptr;
	}
#endif

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


	bool GPURenderMaterial::Traverse(GPURenderMaterialInfoTraverseFunc & traverse_func, char const * renderpass_name) const
	{
		return TraverseImpl(this, material_info.get(), traverse_func, renderpass_name);
	}

	bool GPURenderMaterial::TraverseImpl(GPURenderMaterial const * render_material, GPURenderMaterialInfo const * material_info, GPURenderMaterialInfoTraverseFunc & traverse_func, char const * renderpass_name)
	{
		assert(render_material != nullptr);
		assert(material_info != nullptr);

		// traverse renderpasses first (deep first)
		for (GPURenderMaterialInfoEntry const & entry : material_info->renderpasses)
		{
			if (entry.filter.IsNameEnabled(renderpass_name))
				if (TraverseImpl(render_material, entry.material_info.get(), traverse_func, renderpass_name))
					return true;
		}
		// test current 'node'
		if (traverse_func.OnRenderMaterial(render_material, material_info, renderpass_name))
			return true;
		// traverse parent
		if (material_info->parent_material != nullptr)
			if (TraverseImpl(material_info->parent_material.get(), material_info->parent_material->material_info.get(), traverse_func, renderpass_name))
				return true;
		return false; // continue recursion
	}

	GPUProgram const * GPURenderMaterial::GetEffectiveProgram(GPURenderParams const & render_params) const
	{
		class GPURenderMaterialInfoGetProgramTraverseFunc : public GPURenderMaterialInfoTraverseFunc
		{
		public:

			virtual bool OnRenderMaterial(GPURenderMaterial const * render_material, GPURenderMaterialInfo const * material_info, char const * renderpass_name) override
			{
				// use the traversal as an opportunity to know whether the object is visible (HIDDEN flag)
				if (check_hidden_flag && material_info->hidden_specified)
				{
					check_hidden_flag = false; // no need to search HIDDEN anymore
					if (material_info->hidden)
					{
						program = nullptr; // considere as if there is no program => invisible
						return true;       // stop traversal
					}
					if (program != nullptr && !check_filter) // we already know the program => can stop traversal
						return true;
				}

				// use the traversal as an opportunity to know whether the object is visible (FILTER data)
				if (check_filter && material_info->filter_specified)
				{
					check_filter = false; // no need to search FILTER anymore
					if (!material_info->filter.IsNameEnabled(renderpass_name))
					{
						program = nullptr; // considere as if there is no program => invisible
						return true;       // stop traversal
					}
					if (program != nullptr && !check_hidden_flag) // we already know the program => can stop traversal
						return true;
				}
				// search program
				if (program == nullptr && material_info->program != nullptr)
				{
					program = material_info->program.get();
					if (!check_hidden_flag && !check_filter)
						return true; // Already know all what we need => stop traversal
				}
				return false; // continue traversal
			}

			/** result program */
			shared_ptr<GPUProgram> program;
			/** search whether the hidden state is known */
			bool check_hidden_flag = true;
			/** search whether we encoutered a filter */
			bool check_filter = true;
		};

		GPURenderMaterialInfoGetProgramTraverseFunc traversal_func;
		Traverse(traversal_func, render_params.renderpass_name.c_str()); // this may return TRUE or FALSE depending on the fact that HIDDEN may be specified or NOT
		return traversal_func.program.get();
	}


}; // namespace chaos
