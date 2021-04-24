#include <chaos/Chaos.h>

#include "Ludum48.h"
#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Game.h"
#include "Ludum48Player.h"
#include "Ludum48GameInstance.h"
#include "Ludum48PlayerDisplacementComponent.h"








// =============================================================
// GridInfo implementation
// =============================================================


GridCellInfo& GridInfo::operator ()(glm::ivec2 const & p)
{
	assert(p.x >= 0 && p.x < size.x);
	assert(p.y >= 0 && p.y < size.y);
	int index = p.x + p.y * size.x;
	return cells[index];
}

GridCellInfo const& GridInfo::operator ()(glm::ivec2 const& p) const
{
	assert(p.x >= 0 && p.x < size.x);
	assert(p.y >= 0 && p.y < size.y);
	int index = p.x + p.y * size.x;
	return cells[index];
}

GridCellInfo& GridInfo::operator ()(glm::vec2 const& p)
{
	return operator ()(GetIndexForPosition(p));
}

GridCellInfo const& GridInfo::operator () (glm::vec2 const& p) const
{
	return operator ()(GetIndexForPosition(p));
}

glm::ivec2 GridInfo::GetIndexForPosition(glm::vec2 const& p) const
{
	glm::vec2 position = p - min_position;

	return chaos::RecastVector<glm::ivec2>(position / chaos::RecastVector<glm::vec2>(tile_size));
}












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
	CollectObjects();


	float SPEED = 2.0f;
	float TIMER = 1.0f;

	GameObjectParticle BLOCKER_PARTICLE;
	BLOCKER_PARTICLE.type = GameObjectType::Blocker;

	// from left to right then from right to left
	static int direction = 1;
	direction = 1 - direction;

	int start = (direction > 0) ? 0 : grid_info.size.x - 1;
	int end   = (direction > 0) ? grid_info.size.x : - 1;
	int next  = (direction > 0) ? +1 : -1;

	for (int x = start; x != end; x += next)
	{

		int previous_status = 0;

		for (int y = 0; y < grid_info.size.y; ++y)
		{
			int index = x + y * grid_info.size.x;

			GameObjectParticle* particle = grid_info.cells[index].particle;

			if (particle == nullptr)
			{
				previous_status = 0;
				continue;
			}
			if (particle->type != GameObjectType::Rock && particle->type != GameObjectType::Diamond)
			{
				previous_status = 0;
				continue;
			}

			// search where the particle may move
			if (y > 0)
			{
				GameObjectParticle* particle_below = grid_info.cells[index - grid_info.size.x].particle; // particle below
				if (particle_below == nullptr)
				{
					if (particle->fall_timer < 0.0f) // particle is resting
						particle->fall_timer = TIMER;
					else
					{
						particle->fall_timer = std::max(particle->fall_timer - delta_time, 0.0f);
						if (particle->fall_timer == 0.0f)
						{
							particle->offset.y = std::max(particle->offset.y - SPEED * delta_time, -1.0f);
							if (particle->offset.y == -1.0f)
							{
								particle->bounding_box.position.y -= GetTiledMap()->tile_size.y;
								particle->offset.y = 0.0f;
							}
						}
					}
				}
				else
				{
					auto DI = chaos::GetDebugDrawInterface();
					chaos::DrawBox(*DI, particle_below->bounding_box, { 1.0f, 0.0f, 0.0f, 1.0f }, false);
					chaos::DrawLine(*DI, particle->bounding_box.position, particle_below->bounding_box.position, { 1.0f, 0.0f, 0.0f, 1.0f });

					if (particle_below->type == GameObjectType::Player) // do not try going left or right above the player
						continue;

					for (int i : { 0, 1 })
					{
						if (((i + next) & 1) == 0) // check one branch before the other in a random order
						{
							if (x > 0) // fall to the left
							{
								GameObjectParticle* particle_left = grid_info.cells[index - 1].particle;
								GameObjectParticle* particle_left_below = grid_info.cells[index - 1 - grid_info.size.x].particle;

								if (particle_left == nullptr && particle_left_below == nullptr) // can go left
								{
									particle->fall_timer = 0.0f; // do not wait

									particle->offset.x = std::max(particle->offset.x - SPEED * delta_time, -1.0f);
									if (particle->offset.x == -1.0f)
									{
										particle->bounding_box.position.x -= GetTiledMap()->tile_size.x;
										particle->offset.x = 0.0f;
									}


								}
							}
						}
						else
						{
							if (x < grid_info.size.x - 1) // fall to the right
							{
								GameObjectParticle* particle_right = grid_info.cells[index + 1].particle;
								GameObjectParticle* particle_right_below = grid_info.cells[index + 1 - grid_info.size.x].particle;

								if (particle_right == nullptr && particle_right_below == nullptr) // can go right
								{
									particle->fall_timer = 0.0f; // do not wait

									particle->offset.x = std::max(particle->offset.x + SPEED * delta_time, +1.0f);
									if (particle->offset.x == +1.0f)
									{
										particle->bounding_box.position.x += GetTiledMap()->tile_size.x;
										particle->offset.x = 0.0f;
									}

								}

							}
						}
					}













				}
				

			
			}
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



void LudumLevelInstance::CollectObjects()
{
	LudumLevel* ludum_level = GetLevel();
	if (ludum_level == nullptr || ludum_level->GetTiledMap() == nullptr)
		return;

	// XXX : Initialization map must be closed!!!
	if (grid_info.cells == nullptr)
	{
		
		grid_info.min_position = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
		grid_info.max_position = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

		chaos::TMTileCollisionIterator it = GetTileCollisionIterator(GetBoundingBox(), COLLISION_GAMEOBJECT, false);
		while (it)
		{
			grid_info.min_position = glm::min(grid_info.min_position, it->particle->bounding_box.position);
			grid_info.max_position = glm::max(grid_info.max_position, it->particle->bounding_box.position);
			++it;
		}

		// no element in grid
		if (grid_info.min_position.x == std::numeric_limits<float>::max())
			return;

		grid_info.tile_size = ludum_level->GetTiledMap()->tile_size;

		grid_info.size.x = 1 + int((grid_info.max_position.x - grid_info.min_position.x) / float(grid_info.tile_size.x));
		grid_info.size.y = 1 + int((grid_info.max_position.y - grid_info.min_position.y) / float(grid_info.tile_size.y));

		grid_info.cells = new GridCellInfo[size_t(grid_info.size.x * grid_info.size.y)];

		
	}

	// fill the grid
	for (int i = 0; i < grid_info.size.x * grid_info.size.y; ++i)
		grid_info.cells[i].particle = nullptr;

	chaos::TMTileCollisionIterator it = GetTileCollisionIterator(GetBoundingBox(), COLLISION_GAMEOBJECT, false);
	while (it)
	{
		grid_info(it->particle->bounding_box.position) = { (GameObjectParticle*)it->particle };
		++it;
	}
}

