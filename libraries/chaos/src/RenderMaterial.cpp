#include <chaos/RenderMaterial.h>


namespace chaos
{
  bool GPUProgramVariableRenderMaterialProviderChain::DoProcessAction(char const * name, GPUProgramVariableAction & action, GPUProgramVariableProvider const * top_provider) const
  {
    if (other_provider != nullptr)
      if (other_provider->DoProcessAction(name, action, other_provider))
        return true;

    if (GPUProgramVariableProviderChain::DoProcessAction(name, action, top_provider))
      return true;

    RenderMaterial const * rm = render_material;
    while (rm != nullptr)
    {
      if (rm->uniform_provider != nullptr)
        if (rm->uniform_provider->DoProcessAction(name, action, top_provider))
          return true;
      rm = rm->parent_material.get();
    }
    return false;
  }

  RenderMaterial::RenderMaterial()
  {

  }

  RenderMaterial::~RenderMaterial()
  {
    Release();
  }

  void RenderMaterial::Release()
  {
    program = nullptr;
    uniform_provider = nullptr;
    parent_material = nullptr;
  }


  bool RenderMaterial::SetProgram(GPUProgram * in_program)
  {
    program = in_program;
    return true;
  }

  bool RenderMaterial::SetParentMaterial(RenderMaterial * in_parent)
  {
    // ensure no cycle parenting
    RenderMaterial * rm = in_parent;
    while (rm != nullptr)
    {
      if (rm == this)
        return false; // cycle detected
      rm = rm->parent_material.get();
    }
    parent_material = in_parent;
    return true;
  }

  GPUProgram const * RenderMaterial::GetEffectiveProgram() const
  {
    // go through the hierarchy until we get the program
    RenderMaterial const * rm = this;
    while (rm != nullptr && rm->program == nullptr)
      rm = rm->parent_material.get();
    if (rm == nullptr)
      return nullptr;
    return rm->program.get();
  }

  bool RenderMaterial::UseMaterial(GPUProgramVariableProvider const * in_uniform_provider) const
  {
    // go through the hierarchy until we get the program
    GPUProgram const * effective_program = GetEffectiveProgram();
    if (effective_program == nullptr)
      return false;

    // use the program
    GPUProgramVariableRenderMaterialProviderChain provider(this, in_uniform_provider);
    effective_program->UseProgram(&provider, nullptr);

    return true;
  }
  
}; // namespace chaos
