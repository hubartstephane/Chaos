#pragma once

#include "Ludum40PCH.h"

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
    chaos::BitmapAtlas::GPUAtlas* atlas = nullptr;
    /** the replacement program */
    chaos::shared_ptr<chaos::GPUProgram> program;
};


/** the manager for sprites */
class SpriteManager : public chaos::Object
{
    friend class Game;

public:

    /** the source for the vertex shader */
    static char const* vertex_shader_source;
    /** the source for the pixel shader */
    static char const* fragment_shader_source;

    /** initialize the manager */
    bool Initialize(Game* in_game, SpriteManagerInitParams& params);
    /** Finalize the manager */
    void Finalize();
    /** add a sprite to be rendered */
    void AddSpriteBitmap(chaos::BitmapAtlas::BitmapInfo const* info, glm::vec2 const& position, glm::vec2 const& size, chaos::Hotpoint hotpoint);
    /** add a sprite to be rendered */
    void AddSpriteCharacter(chaos::BitmapAtlas::CharacterInfo const* info, glm::vec2 const& position, glm::vec2 const& size, chaos::Hotpoint hotpoint, glm::vec3 const& color);
    /** Display the sprites */
    void Display(chaos::GPUProgramProviderBase* uniform_provider);
    /** get the number of sprites in the buffer */
    size_t GetSpriteCount() const { return sprites.size() / 6; }
    /** remove the sprites (GPU buffer will be clean at next UpdateGPUBuffer)*/
    void ClearSprites();
    /** get the program */
    chaos::GPUProgram* GetProgram() { return program.get(); }

    chaos::ParticleCorners GetParticleCorners(glm::vec2 const& position, glm::vec2 const& size, chaos::Hotpoint hotpoint) const;

protected:

    /** initialize the manager */
    bool DoInitialize(SpriteManagerInitParams& params);
    /** internal method to send data to GPU */
    void UpdateGPUBuffer();
    /** internal method to insert a sprite in the buffer */
    void AddSpriteImpl(chaos::BitmapAtlas::BitmapLayout const* layout, chaos::ParticleCorners const& corners, glm::vec3 const& color);

protected:

    Game* game = nullptr;

    /** the program to run */
    chaos::shared_ptr<chaos::GPUProgram> program;
    /** the vertex array */
    chaos::shared_ptr<chaos::GPUVertexArray> vertex_array;
    /** the vertex buffer */
    chaos::shared_ptr<chaos::GPUBuffer> vertex_buffer;
    /** the declaration of the vertex buffer */
    chaos::GPUVertexDeclaration declaration;
    /** the texture atlas */
    chaos::BitmapAtlas::GPUAtlas* atlas = nullptr;
    /** the sprites */
    std::vector<SpriteVertex> sprites;
};
