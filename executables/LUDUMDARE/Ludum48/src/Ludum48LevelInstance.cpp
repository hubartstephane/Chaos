#include <chaos/Chaos.h>

#include "Ludum48.h"
#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Game.h"
#include "Ludum48Player.h"
#include "Ludum48GameInstance.h"
#include "Ludum48PlayerDisplacementComponent.h"

// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance()
{
	player_displacement_component_class = chaos::Class::FindClass("MyDisplacementComponent");
}

void LudumLevelInstance::CreateCameraComponents(chaos::Camera* camera, chaos::TMCameraTemplate* camera_template)
{
	chaos::TMLevelInstance::CreateCameraComponents(camera, camera_template);

	camera->SetSafeZone(glm::vec2(0.8f, 0.8f));
	camera->AddComponent(new chaos::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
	camera->AddComponent(new chaos::SoundListenerCameraComponent());
}

int LudumLevelInstance::DoDisplay(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params)
{
	return chaos::TMLevelInstance::DoDisplay(renderer, uniform_provider, render_params);
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	GridInfo grid_info = CollectObjects(); // before everything else

	for (size_t y = 0; y < grid_info.size.y; ++y)
	{
		for (size_t x = 0; x < grid_info.size.x; ++x)
		{



		}


	}












	chaos::TMLevelInstance::DoTick(delta_time);


	// completed ?
	if (completion_timer > 0.0f)
		completion_timer = std::max(0.0f, completion_timer - delta_time);



	return true;
}

bool LudumLevelInstance::Initialize(chaos::Game * in_game, chaos::Level * in_level)
{
	if (!chaos::TMLevelInstance::Initialize(in_game, in_level))
		return false;

	LudumLevel* ludum_level = auto_cast(in_level);
	if (ludum_level != nullptr)
	{

	}	
	return true;
}


bool LudumLevelInstance::IsPlayerDead(chaos::Player* player)
{
	if (chaos::TMLevelInstance::IsPlayerDead(player))
		return true;





	return false;
}

bool LudumLevelInstance::CheckLevelCompletion() const
{
	LudumLevel const * ludum_level = GetLevel();

	LudumPlayer const * ludum_player = GetPlayer(0);
	if (ludum_player != nullptr && ludum_level != nullptr)
	{



	}
	return false;
}

bool LudumLevelInstance::CanCompleteLevel() const
{
	if (completion_timer == 0.0f)
		return true;
	return false;
}

uint64_t LudumLevelInstance::GetCollisionFlagByName(char const* name) const
{
	assert(name != nullptr);

	if (chaos::StringTools::Stricmp(name, "World") == 0)
		return COLLISION_GAMEOBJECT;

	return chaos::TMLevelInstance::GetCollisionFlagByName(name);
}



GridInfo LudumLevelInstance::CollectObjects()
{
	LudumLevel* ludum_level = GetLevel();
	if (ludum_level == nullptr || ludum_level->GetTiledMap() == nullptr)
		return {};

	glm::ivec2 size = ludum_level->GetTiledMap()->size;
	glm::ivec2 tile_size = ludum_level->GetTiledMap()->tile_size;

	// grid mod unimplemented
	// step 1: get min/max
	// step 2: put on the grid

	GridInfo result;

	// step 1
	glm::vec2 min_position = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	glm::vec2 max_position = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

	chaos::TMTileCollisionIterator it = GetTileCollisionIterator(GetBoundingBox(), COLLISION_GAMEOBJECT, false);
	while (it)
	{
		min_position = glm::min(min_position, it->particle->bounding_box.position);
		max_position = glm::max(max_position, it->particle->bounding_box.position);
		++it;
	}

	// no element in grid
	if (min_position.x == std::numeric_limits<float>::max())
		return {};

	// step 2
	result.size.x = 1 + int((max_position.x - min_position.x) / float(tile_size.x));
	result.size.y = 1 + int((max_position.y - min_position.y) / float(tile_size.y));

	result.cells = new GridCellInfo[size_t(result.size.x * result.size.y)];

	 it = GetTileCollisionIterator(GetBoundingBox(), COLLISION_GAMEOBJECT, false);
	while (it)
	{
		glm::vec2 position = it->particle->bounding_box.position - min_position;

		glm::ivec2 p = chaos::RecastVector<glm::ivec2>(position) / size;

		size_t index = p.x + p.y * result.size.x;
		result.cells[index] = { (GameObjectParticle*)it->particle };

		++it;
	}

	return result;
}





class MyParticle : public chaos::ParticleDefault
{
public:

	float rest_timer = 0.0f;

	void Update(float delta_time)
	{


	}


};


void GameGG()
{







}