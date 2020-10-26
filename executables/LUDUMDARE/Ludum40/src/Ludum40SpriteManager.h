#pragma once

#include <chaos/Chaos.h>


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
    chaos::shared_ptr<GPUProgram> program;
  };


  /** the manager for sprites */
  class SpriteManager : public Object
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
    void AddSpriteBitmap(BitmapAtlas::BitmapInfo const * info, glm::vec2 const & position, glm::vec2 const & size, Hotpoint hotpoint);
    /** add a sprite to be rendered */
    void AddSpriteCharacter(BitmapAtlas::CharacterInfo const * info, glm::vec2 const & position, glm::vec2 const & size, Hotpoint hotpoint, glm::vec3 const & color);
    /** Display the sprites */
    void Display(GPUProgramProviderBase * uniform_provider);
    /** get the number of sprites in the buffer */
    size_t GetSpriteCount() const { return sprites.size() / 6; }
    /** remove the sprites (GPU buffer will be clean at next UpdateGPUBuffer)*/
    void ClearSprites();
    /** get the program */
    GPUProgram * GetProgram() { return program.get(); }

	ParticleCorners GetParticleCorners(glm::vec2 const& position, glm::vec2 const& size, Hotpoint hotpoint) const;

  protected:

    /** initialize the manager */
    bool DoInitialize(SpriteManagerInitParams & params);
    /** internal method to send data to GPU */
    void UpdateGPUBuffer();
    /** internal method to insert a sprite in the buffer */
    void AddSpriteImpl(BitmapAtlas::BitmapLayout const * layout, ParticleCorners const & corners, glm::vec3 const & color);

  protected:

    /** the program to run */
    chaos::shared_ptr<GPUProgram> program;
    /** the vertex array */
    chaos::shared_ptr<GPUVertexArray> vertex_array;
    /** the vertex buffer */
    chaos::shared_ptr<GPUBuffer> vertex_buffer;
    /** the declaration of the vertex buffer */
    GPUVertexDeclaration declaration;
    /** the texture atlas */
    BitmapAtlas::TextureArrayAtlas * atlas = nullptr;
    /** the sprites */
    std::vector<SpriteVertex> sprites;
  };
}; // namespace chaos