#include <chaos/RenderMaterial.h>


namespace chaos
{

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

  void RenderMaterial::UseMaterial(GPUProgramVariableProvider const * in_uniform_provider) const
  {
    // go through the hierarchy until we get the program
    RenderMaterial const * rm = this;
    while (rm != nullptr && rm->program == nullptr)
      rm = rm->parent_material.get();
    if (rm == nullptr)
      return;

    // use the program
    rm->program->UseProgram(nullptr, nullptr);

    GPUProgramData const & program_data = rm->program->GetProgramData();
    program_data.BindUniforms(this, in_uniform_provider);
  }
  
}; // namespace chaos
