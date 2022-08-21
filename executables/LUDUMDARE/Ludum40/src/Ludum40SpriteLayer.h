#pragma once

#include "chaos/Chaos.h"

#include "Ludum40SpriteManager.h"


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

	static constexpr int SPAWN_TYPE_CENTER = 0;
	static constexpr int SPAWN_TYPE_OUTASCREEN = 1;
	static constexpr int SPAWN_TYPE_OUTASCREEN_TESTCOLLISION = 2;

	bool LoadFromJSON(nlohmann::json const & json);

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

	chaos::BitmapAtlas::TextureArrayAtlas const * texture_atlas = nullptr;
	std::vector<ObjectDefinition> const & object_definitions;
	chaos::box2 world_box;
	chaos::box2 world_box_padding;
};

// ======================================================================================

class SpriteLayer
{
public:

	static constexpr int NO_COLLISION = 0;
	static constexpr int COLLISION_DEATH = 1;
	static constexpr int COLLISION_LEVELUP = 2;
	static constexpr int COLLISION_SPEEDUP = 3;

	void Draw(chaos::GPUProgramProviderBase * uniform_provider);

	void Tick(float delta_time, GameInfo game_info);

	void DestroyAllParticles();

	void PopulateSprites(GameInfo game_info, int count);
	
	void SetVisible(bool in_visible);

	bool LoadFromJSON(nlohmann::json const & json);

protected:

	void UpdateParticleLifetime(float delta_time);
	void UpdateParticleVelocity(float delta_time);
	void DestroyParticleByClipRect(GameInfo game_info);
	void UpdateGPUBuffer(GameInfo game_info);

	std::vector<std::pair<int, size_t>> GetSpritePopulationStats(GameInfo game_info) const;
	void PopulateSpritesWithDef(GameInfo game_info, int & count, ObjectDefinition const & def);

public:

	chaos::shared_ptr<SpriteManager> sprite_manager;

	std::vector<Particle> particles;


	// current state	
	bool visible = true;
	
	// properties
	std::string name;
	
	float relative_speed = 1.0f;
	bool  start_visible = true;
	int   collision_type = 0;
	int   min_particle_count = 0;
	int   max_particle_count = 0;
	int   layer = 0;
};
