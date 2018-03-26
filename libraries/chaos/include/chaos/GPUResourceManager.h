#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Manager.h>
#include <chaos/Texture.h>
#include <chaos/GPUProgram.h>
#include <chaos/RenderMaterial.h>
#include <chaos/FilePath.h>

namespace chaos
{
  class GPUResourceManager : public Manager
  {
  public:

    /** destructor */
    virtual ~GPUResourceManager();
    /** release the resources */
    virtual void Release();

    /** load a texture */
    Texture * LoadTexture(FilePathParam const & path, char const * name = nullptr);
    /** load a program */
    GPUProgram * LoadProgram(FilePathParam const & path, char const * name = nullptr);
    /** load a material */
    RenderMaterial * LoadRenderMaterial(FilePathParam const & path, char const * name = nullptr);

    /** loading from a JSON object */
    virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);

  protected:

    /** load the textures from configuration */
    bool InitializeTexturesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);
    /** load the programs from configuration */
    bool InitializeProgramsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);
    /** load the materials from configuration */
    bool InitializeMaterialsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);

    /** add a texture from a JSON object */
    Texture * AddJSONTexture(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path);
    /** add a program from a JSON object */
    GPUProgram * AddJSONProgram(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path);
    /** add a material from a JSON object */
    RenderMaterial * AddJSONMaterial(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path);

  protected:

    /** the textures */
    std::vector<boost::intrusive_ptr<Texture>> textures;
    /** the programs */
    std::vector<boost::intrusive_ptr<GPUProgram>> programs;
    /** the render materials */
    std::vector<boost::intrusive_ptr<RenderMaterial>> render_materials;
  };

}; // namespace chaos
