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

	static int const SPAWN_TYPE_CENTER = 0;
	static int const SPAWN_TYPE_OUTASCREEN = 1;
	static int const SPAWN_TYPE_OUTASCREEN_TESTCOLLISION = 2;

	bool LoadFromJSON(nlohmann::json const & json_entry);

public:

	int id = 0;
	int layer = 0;
	float size = 1.0f;
	float min_lifetime = 0.0f;
	float max_lifetime = 0.0f;
	int   spawn_type = SPAWN_TYPE_CENTER;
	
	boost::filesystem::path bitmap_path;
};

// ======================================================================================

class GameInfo
{
public:

	GameInfo(class Game const & game);

	ObjectDefinition const * GetObjectDefinition(int id);

	chaos::BitmapAtlas::TextureArrayAtlas const & texture_atlas;
	std::vector<ObjectDefinition> const & object_definitions;
	chaos::box2 world_box;
	chaos::box2 world_box_padding;
};

// ======================================================================================

class SpriteLayer
{
public:

	static const int NO_COLLISION = 0;
	static const int COLLISION_DEATH = 1;
	static const int COLLISION_LEVELUP = 2;
	static const int COLLISION_SPEEDUP = 3;

	void Draw(chaos::GLProgramVariableProvider * uniform_provider);

	void Tick(double delta_time, GameInfo game_info);

	void DestroyAllParticles();

	void PopulateSprites(GameInfo game_info, int count);
	
	void SetVisible(bool in_visible);

	bool LoadFromJSON(nlohmann::json const & json_entry);

protected:

	void UpdateParticleLifetime(double delta_time);
	void UpdateParticleVelocity(double delta_time);
	void DestroyParticleByClipRect(GameInfo game_info);
	void UpdateGPUBuffer(GameInfo game_info);

	std::vector<std::pair<int, size_t>> GetSpritePopulationStats(GameInfo game_info) const;
	void PopulateSpritesWithDef(GameInfo game_info, int & count, ObjectDefinition const & def);

public:

	boost::intrusive_ptr<chaos::SpriteManager> sprite_manager;

	std::vector<Particle> particles;


	// current state	
	bool visible = true;
	
	// properties
	std::string name;
	
	float relative_speed = 1.0f;
	bool  start_visible = true;
	int   collision_type = 0;
	int   max_particle_count = 0;
	int   layer = 0;
};
