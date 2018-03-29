#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Manager.h>
#include <chaos/Texture.h>
#include <chaos/GPUProgram.h>
#include <chaos/RenderMaterial.h>
#include <chaos/GPUFileResource.h>
#include <chaos/FilePath.h>

namespace chaos
{
  class GPUResourceManager : public Manager, protected GPUFileResourceFriend
  {
  public:

    /** destructor */
    virtual ~GPUResourceManager();
    /** release the resources */
    virtual void Release();

    /** load a texture (named deduced from path) */
    Texture * LoadTexture(FilePathParam const & path);
    /** load a texture */
    Texture * LoadTexture(FilePathParam const & path, char const * name);
    /** load a program (named deduced from path) */
    GPUProgram * LoadProgram(FilePathParam const & path);
    /** load a program */
    GPUProgram * LoadProgram(FilePathParam const & path, char const * name);
    /** load a material (named deduced from path) */
    RenderMaterial * LoadRenderMaterial(FilePathParam const & path);
    /** load a material */
    RenderMaterial * LoadRenderMaterial(FilePathParam const & path, char const * name);

    /** find a texture by its name */
    Texture * FindTexture(char const * name);
    /** find a texture by its name */
    Texture const * FindTexture(char const * name) const;
    /** find a texture by its path */
    Texture * FindTextureByPath(FilePathParam const & path);
    /** find a texture by its path */
    Texture const * FindTextureByPath(FilePathParam const & path) const;

    /** find a program by its name */
    GPUProgram * FindProgram(char const * name);
    /** find a program by its name */
    GPUProgram const * FindProgram(char const * name) const;
    /** find a program by its path */
    GPUProgram * FindProgramByPath(FilePathParam const & path);
    /** find a program by its path */
    GPUProgram const * FindProgramByPath(FilePathParam const & path) const;

    /** find a render material by its name */
    RenderMaterial * FindRenderMaterial(char const * name);
    /** find a render material by its name */
    RenderMaterial const * FindRenderMaterial(char const * name) const;
    /** find a render material by its path */
    RenderMaterial * FindRenderMaterialByPath(FilePathParam const & path);
    /** find a render material by its path */
    RenderMaterial const * FindRenderMaterialByPath(FilePathParam const & path) const;

    /** loading from a JSON object */
    virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);

  protected:

    /** returns whether a texture with given name can be inserted */
    bool CanAddTexture(char const * name) const;
    /** returns whether a program with given name can be inserted */
    bool CanAddProgram(char const * name) const;
    /** returns whether a render material with given name can be inserted */
    bool CanAddRenderMaterial(char const * name) const;

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
