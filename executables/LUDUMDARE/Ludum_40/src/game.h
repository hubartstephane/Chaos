#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/MyGLFWGamepadManager.h> 
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


class MyGamepadManager : public chaos::MyGLFW::GamepadManager
{
public:

	MyGamepadManager(class Game * in_game) : game(in_game) {}

protected:

	virtual bool DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad) override;

protected:

	class Game * game = nullptr;
};

// ======================================================================================

class Particle
{
public:

	glm::vec2 position;
	glm::vec2 half_size;
	glm::vec2 velocity;
	float life_time;
	int   id;
};


// ======================================================================================

class ObjectDefinition
{

public:

	bool LoadFromJSON(nlohmann::json const & json_entry);

public:

	int id = 0;
	int layer = 0;
	int initial_particle_count = 0;
	float scale = 1.0f;
	float min_lifetime = 0.0f;
	float max_lifetime = 0.0f;

	boost::filesystem::path bitmap_path;
};

// ======================================================================================

class GameInfo
{
public:

	GameInfo(class Game const & game);

	chaos::BitmapAtlas::TextureArrayAtlas const & texture_atlas;
	std::vector<ObjectDefinition> const & object_definitions;
};

// ======================================================================================

class SpriteLayer
{
public:

	void Draw(chaos::GLProgramVariableProvider * uniform_provider);

	void Tick(double delta_time, GameInfo game_info, chaos::box2 const * clip_rect);

	void DestroyAllParticles();

	void InitialPopulateSprites(GameInfo game_info);

protected:

	void UpdateParticleLifetime(double delta_time);
	void UpdateParticleVelocity(double delta_time);
	void DestroyParticleByClipRect(chaos::box2 const * clip_rect);
	void UpdateGPUBuffer(GameInfo game_info);

public:

	boost::intrusive_ptr<chaos::SpriteManager> sprite_manager;

	std::vector<Particle> particles;

	int layer;
};

// ======================================================================================

class Game : public chaos::ReferencedObject
{
	friend class GameInfo;

	friend class MyGamepadManager;

public:

	void Tick(double delta_time, chaos::box2 const * clip_rect);

	bool Initialize(boost::filesystem::path const & path);

	void Finalize();

	void Display(glm::ivec2 size);

	void SetPause(bool in_paused);

	void StartGame();

	bool OnKeyEvent(int key, int action);

protected:

	void OnGameStarted();

	bool LoadObjectDefinition(boost::filesystem::path const & path);

	bool LoadObjectDefinition(nlohmann::json const & json_entry);

	bool GenerateAtlas(boost::filesystem::path const & path);

	bool GenerateSpriteGPUProgram(boost::filesystem::path const & path);

	bool GenerateSpriteLayers();

	bool InitializeGamepadManager();

	bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

	SpriteLayer * FindSpriteLayer(int layer);

protected:

	bool game_paused = false;

	bool game_started = false;

	std::vector<SpriteLayer> sprite_layers;

	chaos::BitmapAtlas::TextureArrayAtlas texture_atlas;

	std::vector<ObjectDefinition> object_definitions;

	boost::intrusive_ptr<chaos::GLProgram> sprite_program;

	boost::intrusive_ptr<MyGamepadManager> gamepad_manager;


};
