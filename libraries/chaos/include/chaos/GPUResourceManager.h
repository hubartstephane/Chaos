#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Texture.h>
#include <chaos/GPUProgram.h>
#include <chaos/RenderMaterial.h>
#include <chaos/FilePath.h>

namespace chaos
{
  class GPUResourceManager : public ReferencedObject
  {
  public:

    /** destructor */
    virtual ~GPUResourceManager();
	/** release the resources */
	virtual void Release();

	/** load a texture */
	Texture * LoadTexture(FilePath const & path);
	/** load a program */
	GPUProgram * LoadProgram(FilePath const & path);
	/** load a material */
	RenderMaterial * LoadRenderMaterial(FilePath const & path);

  public:

    /** the textures */
    std::vector<boost::intrusive_ptr<Texture>> textures;
	/** the programs */
	std::vector<boost::intrusive_ptr<GPUProgram>> programs;
	/** the render materials */
	std::vector<boost::intrusive_ptr<RenderMaterial>> render_materials;

  };


}; // namespace chaos
