#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramVariableProvider.h>
#include <chaos/SoundManager.h>
#include <json.hpp>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/SpriteManager.h>

// ======================================================================================

class Particle
{
public:

	glm::vec2 position;
	glm::vec2 half_size;
	glm::vec2 velocity;
	float life_time;
};


// ======================================================================================

class ObjectDefinition
{

public:

	bool LoadFromJSON(nlohmann::json const & json_entry);

public:

	int id = 0;
	int layer = 0;
	float scale = 1.0f;
	boost::filesystem::path bitmap_path;
};

// ======================================================================================

class SpriteLayer
{
public:

	void Draw(chaos::GLProgramVariableProviderChain & uniform_provider);

	void Tick(double delta_time, chaos::box2 const * clip_rect);

protected:

	void UpdateParticleLifetime(double delta_time);
	void UpdateParticleVelocity(double delta_time);
	void DestroyParticleByClipRect(chaos::box2 const * clip_rect);
	void UpdateGPUBuffer();

public:

	boost::intrusive_ptr<chaos::SpriteManager> sprite_manager;

	std::vector<Particle> particles;

	int layer;
};

// ======================================================================================

class Game : public chaos::ReferencedObject
{
public:

	void Tick(double delta_time, chaos::box2 const * clip_rect);

	bool Initialize(boost::filesystem::path const & path);

	void Finalize();

	void Display(glm::ivec2 size);

protected:

	bool LoadObjectDefinition(boost::filesystem::path const & path);

	bool LoadObjectDefinition(nlohmann::json const & json_entry);

	bool GenerateAtlas(boost::filesystem::path const & path);

	bool GenerateSpriteGPUProgram(boost::filesystem::path const & path);

	bool GenerateSpriteLayers();

	SpriteLayer * FindSpriteLayer(int layer);



protected:

	std::vector<SpriteLayer> sprite_layers;

	chaos::BitmapAtlas::TextureArrayAtlas texture_atlas;

	std::vector<ObjectDefinition> object_definitions;

	boost::intrusive_ptr<chaos::GLProgram> sprite_program;
};
