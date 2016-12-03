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

namespace chaos
{

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
    BitmapAtlas::TextureArrayAtlas * atlas{nullptr};
  };


  class SpriteManager
  {
  public:

    /** the source for the vertex shader */
    static char const * vertex_shader_source;
    /** the source for the pixel shader */
    static char const * pixel_shader_source;

    /** theses are the offset are the ones to apply to position, if position correspond to a given border */

    static int const HOTPOINT_LEFT = 1;
    static int const HOTPOINT_RIGHT = 2;
    static int const HOTPOINT_HMIDDLE = 0;

    static int const HOTPOINT_BOTTOM = 4;
    static int const HOTPOINT_TOP = 8;
    static int const HOTPOINT_VMIDDLE = 0;

    static int const HOTPOINT_BOTTOM_LEFT = HOTPOINT_BOTTOM | HOTPOINT_LEFT;
    static int const HOTPOINT_TOP_LEFT = HOTPOINT_TOP | HOTPOINT_LEFT;
    static int const HOTPOINT_BOTTOM_RIGHT = HOTPOINT_BOTTOM | HOTPOINT_RIGHT;
    static int const HOTPOINT_TOP_RIGHT = HOTPOINT_TOP | HOTPOINT_RIGHT;
    static int const HOTPOINT_CENTER = HOTPOINT_VMIDDLE | HOTPOINT_HMIDDLE;

    /** given a hotpoint, a sprite size, and the hotpoint_type, returns the BottomLeft hotpoint position */
    static glm::vec2 GetBottomLeftHotpointPosition(glm::vec2 const & hotpoint, glm::vec2 const & size, int hotpoint_type);
    /** given a hotpoint, a sprite size, and the hotpoint_type, returns the any other hotpoint_type position */
    static glm::vec2 GetHotpointPosition(glm::vec2 const & position, glm::vec2 const & size, int initial_hotpoint_type, int final_hotpoint_type);

    /** initialize the manager */
    bool Initialize(SpriteManagerInitParams & params);
    /** Finalize the manager */
    void Finalize();
    /** add a sprite to be rendered */
    void AddSprite(BitmapAtlas::BitmapEntry const * entry, glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type);
    /** add a sprite to be rendered */
    void AddSprite(BitmapAtlas::CharacterEntry const * entry, glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type, glm::vec3 const & color);
    /** Display the sprites */
    void Display();


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
    BitmapAtlas::TextureArrayAtlas * atlas{nullptr};
    /** the sprites */
    std::vector<SpriteVertex> sprites;
  };
};