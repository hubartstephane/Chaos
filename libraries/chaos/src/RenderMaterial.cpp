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
    variable_provider = nullptr;
    parent_material = nullptr;
  }

  void RenderMaterial::UseMaterial(GLProgramVariableProvider * uniform_provider)
  {
   // glUseProgram(program->GetResourceID());


  }
  

#if 0

  GLProgramVariableProviderChain * next_material

  chaos::GLProgramData const & program_data = program->GetProgramData();

  chaos::GLProgramVariableProviderChain uniform_provider;
  uniform_provider.AddVariableTexture("material", texture);
  uniform_provider.AddVariableValue("min_texture_coord", min_texture_coord);
  uniform_provider.AddVariableValue("max_texture_coord", max_texture_coord);
  uniform_provider.AddVariableValue("life_ratio", life_ratio);
  uniform_provider.AddVariableValue("level_ratio", level_ratio);

  program_data.BindUniforms(&uniform_provider);
#endif

}; // namespace chaos
