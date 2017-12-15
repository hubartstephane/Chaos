#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/VertexArray.h>
#include <chaos/VertexBuffer.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/GLProgramVariableProvider.h>
#include <chaos/Hotpoint.h>
#include <chaos/SpriteManager.h>


namespace chaos
{

  /** the sprite structure */
  class SpriteVertex
  {
  public:

    glm::vec2 position;
    glm::vec3 texcoord;
    glm::vec3 color;
  };

  /** a structure to for manager initialization data */
  class SpriteManagerInitParams
  {
  public:

    /** the texture atlas */
    BitmapAtlas::TextureArrayAtlas * atlas = nullptr;
    /** the replacement program */
    boost::intrusive_ptr<GLProgram> program;
  };


  /** the manager for sprites */
  class SpriteManager : public ReferencedObject
  {
  public:

    /** the source for the vertex shader */
    static char const * vertex_shader_source;
    /** the source for the pixel shader */
    static char const * pixel_shader_source;

    /** initialize the manager */
    bool Initialize(SpriteManagerInitParams & params);
    /** Finalize the manager */
    void Finalize();
    /** add a sprite to be rendered */
    void AddSpriteBitmap(BitmapAtlas::BitmapEntry const * entry, glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type);
    /** add a sprite to be rendered */
    void AddSpriteCharacter(BitmapAtlas::CharacterEntry const * entry, glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type, glm::vec3 const & color);
    /** Display the sprites */
    void Display(GLProgramVariableProvider * uniform_provider);
    /** get the number of sprites in the buffer */
    int GetSpriteCount() const { return sprites.size() / 6; }
    /** remove the sprites (GPU buffer will be clean at next UpdateGPUVertexBuffer)*/
    void ClearSprites();
    /** get the program */
    GLProgram * GetProgram() { return program.get(); }

  protected:

    /** initialize the manager */
    bool DoInitialize(SpriteManagerInitParams & params);
    /** internal method to send data to GPU */
    void UpdateGPUVertexBuffer();
    /** internal method to insert a sprite in the buffer */
    void AddSpriteImpl(BitmapAtlas::BitmapEntry const * entry, glm::vec2 const & bottomleft_position, glm::vec2 const & size, glm::vec3 const & color);

  protected:

    /** the program to run */
    boost::intrusive_ptr<GLProgram> program;
    /** the vertex array */
    boost::intrusive_ptr<VertexArray> vertex_array;
    /** the vertex buffer */
    boost::intrusive_ptr<VertexBuffer> vertex_buffer;
    /** the declaration of the vertex buffer */
    VertexDeclaration declaration;
    /** the texture atlas */
    BitmapAtlas::TextureArrayAtlas * atlas = nullptr;
    /** the sprites */
    std::vector<SpriteVertex> sprites;
  };
};