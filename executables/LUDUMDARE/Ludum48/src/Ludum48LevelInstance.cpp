#include <chaos/Chaos.h>

#include "Ludum48.h"
#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Game.h"
#include "Ludum48Player.h"
#include "Ludum48GameInstance.h"
#include "Ludum48PlayerDisplacementComponent.h"

#define DEBUG_DRAW _DEBUG

// =============================================================
// GridInfo implementation
// =============================================================

bool GridCellInfo::CanLock(GameObjectParticle* p) const
{
	return locked_by == nullptr && particle == nullptr;
}

void GridCellInfo::Lock(GameObjectParticle* p)
{
	assert(p != nullptr);
	locked_by = p;
}

// =============================================================
// GridInfo implementation
// =============================================================

bool GridInfo::IsInside(glm::ivec2 const& p) const
{
	if (p.x < 0 || p.x >= size.x)
		return false;
	if (p.y < 0 || p.y >= size.y)
		return false;
	return true;
}

GridCellInfo& GridInfo::operator ()(glm::ivec2 const & p)
{
	assert(IsInside(p));
	int index = p.x + p.y * size.x;
	return cells[index];
}

GridCellInfo const& GridInfo::operator ()(glm::ivec2 const& p) const
{
	assert(IsInside(p));
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

	return RecastVector<glm::ivec2>(position / RecastVector<glm::vec2>(tile_size));
}

glm::ivec2 GridInfo::GetCellCoord(GridCellInfo const& cell) const
{
	int offset = int(&cell - &cells[0]);
	return { offset % size.x, offset / size.x };
}

box2 GridInfo::GetBoundingBox(GridCellInfo const& cell) const
{
	box2 result;
	result.position = min_position + tile_size * RecastVector<glm::vec2>(GetCellCoord(cell));

	result.half_size = tile_size * 0.5f;
	return result;
}

// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance()
{
	player_displacement_component_class = Class::FindClass("MyDisplacementComponent");
}

void LudumLevelInstance::CreateCameraComponents(Camera* camera, TMCameraTemplate* camera_template)
{
	TMLevelInstance::CreateCameraComponents(camera, camera_template);

	camera->SetSafeZone(glm::vec2(0.8f, 0.8f));
	camera->AddComponent(new ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
	camera->AddComponent(new SoundListenerCameraComponent());
}

int LudumLevelInstance::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
{
	return TMLevelInstance::DoDisplay(renderer, uniform_provider, render_params);
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	// timeout
	if (level_timeout == 0.0f)
	{
		PlayerPawn* pawn = GetPlayerPawn(0);
		if (pawn != nullptr)
		{
			if (GameObjectParticle* particle = pawn->GetParticle<GameObjectParticle>(0))
			{
				KillPlayer(particle);
				level_timeout = -1.0f;
			}
		}
	}

	// check for leaving level
	if (door_opened && !level_complete)
	{
		PlayerPawn const* player_pawn = GetPlayerPawn(0);
		if (player_pawn != nullptr)
		{
			GameObjectParticle const * player_particle = player_pawn->GetParticle<GameObjectParticle>(0);
			if (player_particle != nullptr)
			{
				box2 player_box = player_particle->bounding_box;
				player_box.position += player_particle->offset * glm::vec2(32.0f, 32.0f); // HACK shu48
				player_box.half_size *= 0.01f;

				TMTileCollisionIterator it = GetTileCollisionIterator(player_box, COLLISION_GATE, false);
				if (it)
				{
					level_complete = true;
					completion_timer = completion_delay;
				}
			}
		}
	}

	// very first frame
	if (frame_timer == 0.0f)
	{
		CollectObjects();
		NegociateDisplacements();
	}

	// normal frames
	DisplaceObjects(delta_time);




	// last frame
	frame_timer += delta_time;
	if (frame_timer >= frame_duration)
	{
		FinalizeDisplacements();





		frame_timer = 0.0f;
	}

	

	



#if 0

	CollectObjects();

	if (grid_info.cells != nullptr)
	{
		float dt = delta_time * speed_factor;
		HandlePlayerObject(dt);
		HandleFallingObjects(dt);
		HandleMonsterObjects(dt);
		HandleBlobObjects(dt);
		HandleDisplacements(dt);
		CreateDiamonds();
	}
#endif


	TMLevelInstance::DoTick(delta_time);

	// completed ?
	if (completion_timer > 0.0f)
		completion_timer = std::max(0.0f, completion_timer - delta_time);
	return true;
}

float LudumLevelInstance::GetObjectSpeed() const
{
	return 1.0f/ ((frame_duration <= 0.0f)? 0.5f : frame_duration);
}


void LudumLevelInstance::NegociateDisplacements()
{
	for (GameObjectType type : {GameObjectType::Monster, GameObjectType::Diamond, GameObjectType::Rock, GameObjectType::Player}) // order of priorities
	{
		for (size_t y = 0; y < grid_info.size.y; ++y)
		{
			for (size_t x = 0; x < grid_info.size.x; ++x)
			{
				glm::ivec2 p = { x, y };

				GridCellInfo& cell = grid_info(p);
				if (cell.particle != nullptr && cell.particle->type == type)
				{
					// Monster
					if (type == GameObjectType::Monster)
					{
						NegociateMonsterDisplacement(p, cell);
					}
					// Diamond/Rock
					else if (type == GameObjectType::Diamond || type == GameObjectType::Rock)
					{
						NegociateFallerDisplacement(p, cell);


					}
					// Player
					else if (type == GameObjectType::Player)
					{
						NegociatePlayerDisplacement(p, cell);
					}
					//
				}
			}
		}
	}
}

void LudumLevelInstance::NegociateMonsterDisplacement(glm::ivec2 const& p, GridCellInfo& cell)
{

}

void LudumLevelInstance::NegociateFallerDisplacement(glm::ivec2 const& p, GridCellInfo& cell)
{
	glm::ivec2 below_p = p - glm::ivec2(0, 1);
	if (!grid_info.IsInside(below_p))
		return;

	GridCellInfo& below = grid_info(below_p);
	if (below.CanLock(cell.particle))
	{
		below.Lock(cell.particle);
		cell.particle->direction = { 0.0f, -1.0f };
		cell.particle->speed = GetObjectSpeed();
	}
	else
	{
		//if (below)


	}




#if 0
	GridCellInfo& below = grid_info.cells[index - grid_info.size.x];

	if (step == 0)
	{
		if (below.CanLock(particle))
		{
			below.Lock(particle);
			particle->direction = { 0.0f, -1.0f };
			particle->speed = object_speed;
		}
		else
		{
#if DEBUG_DRAW
			if (below.particle != nullptr)
			{
				glm::vec4 RED = { 1.0f, 0.0f, 0.0f, 1.0f };

				DrawBox(*DI, below.particle->bounding_box, RED, false);
				DrawLine(*DI, particle->bounding_box.position, below.particle->bounding_box.position, RED);
			}
#endif
		}
	}
	else if (step == 1)
	{
		if (below.particle == nullptr)
			continue;
		if (below.particle->type != GameObjectType::Wall && below.particle->type != GameObjectType::Rock && below.particle->type != GameObjectType::Diamond) // do not try going left or right above the player
			continue;
		if (below.particle->direction.x != 0.0f || below.particle->direction.y != 0.0f)
			continue;

		int random = rand();
		for (int i : { 0, 1 })
		{
			if (((i + random) & 1) == 0) // check one branch before the other in a random order
			{
				if (x > 0) // fall to the left
				{
					GridCellInfo& left = grid_info.cells[index - 1];
					GridCellInfo& left_below = grid_info.cells[index - 1 - grid_info.size.x];

					if (left.CanLock(particle) && left_below.CanLock(particle))
					{
						left.Lock(particle);
						particle->direction = { -1.0f, 0.0f };
						particle->speed = object_speed;
						break;
					}
				}
			}
			else
			{
				if (x < grid_info.size.x - 1) // fall to the right
				{
					GridCellInfo& right = grid_info.cells[index + 1];
					GridCellInfo& right_below = grid_info.cells[index + 1 - grid_info.size.x];

					if (right.CanLock(particle) && right_below.CanLock(particle))
					{
						right.Lock(particle);
						particle->direction = { +1.0f, 0.0f };
						particle->speed = object_speed;
						break;

					}
				}
			}

#endif


}

void LudumLevelInstance::NegociatePlayerDisplacement(glm::ivec2 const& p, GridCellInfo& cell)
{
}



void LudumLevelInstance::DisplaceObjects(float delta_time)
{
	for (int y = 0; y < grid_info.size.y; ++y)
	{
		for (int x = 0; x < grid_info.size.x; ++x)
		{
			glm::ivec2 p = { x, y };

			GridCellInfo& cell = grid_info(p);
			if (cell.particle == nullptr)
				continue;

			for (int axis : {0, 1})
				if (cell.particle->direction[axis] != 0.0f)
					cell.particle->offset[axis] = std::clamp(cell.particle->offset[axis] + cell.particle->speed * delta_time * cell.particle->direction[axis], -1.0f, 1.0f);
		}
	}
}







void LudumLevelInstance::FinalizeDisplacements()
{
	for (int y = 0; y < grid_info.size.y; ++y)
	{
		for (int x = 0; x < grid_info.size.x; ++x)
		{
			glm::ivec2 p = { x, y };

			GridCellInfo& cell = grid_info(p);
			if (cell.particle == nullptr)
				continue;

			cell.particle->bounding_box.position += cell.particle->direction * RecastVector<glm::vec2>(grid_info.tile_size);
			cell.particle->offset = { 0.0f, 0.0f };
			cell.particle->direction = { 0.0f, 0.0f };
		}
	}
}




void LudumLevelInstance::HandleFallingObjects(float delta_time)
{
#if 0
	auto DI = GetDebugDrawInterface();

	for (int step : {0, 1})
	{
		for (int y = 0; y < grid_info.size.y; ++y)
		{
			for (int x = 0; x < grid_info.size.x; ++x)
			{
				int index = x + y * grid_info.size.x;

				GameObjectParticle* particle = grid_info.cells[index].particle;

				if (particle == nullptr)
					continue;
				if (particle->type != GameObjectType::Rock && particle->type != GameObjectType::Diamond)
					continue;

				// search where the particle may move
				if (y > 0 && particle->direction.x == 0.0f && particle->direction.y == 0.0f)
				{
					GridCellInfo& below = grid_info.cells[index - grid_info.size.x];

					if (step == 0)
					{
						if (below.CanLock(particle))
						{
							below.Lock(particle);
							particle->direction = { 0.0f, -1.0f };
							particle->speed = object_speed;
						}
						else
						{
#if DEBUG_DRAW
							if (below.particle != nullptr)
							{
								glm::vec4 RED = { 1.0f, 0.0f, 0.0f, 1.0f };

								DrawBox(*DI, below.particle->bounding_box, RED, false);
								DrawLine(*DI, particle->bounding_box.position, below.particle->bounding_box.position, RED);
							}
#endif
						}
					}
					else if (step == 1)
					{
						if (below.particle == nullptr)
							continue;
						if (below.particle->type != GameObjectType::Wall && below.particle->type != GameObjectType::Rock && below.particle->type != GameObjectType::Diamond) // do not try going left or right above the player
							continue;
						if (below.particle->direction.x != 0.0f || below.particle->direction.y != 0.0f)
							continue;

						int random = rand();
						for (int i : { 0, 1 })
						{
							if (((i + random) & 1) == 0) // check one branch before the other in a random order
							{
								if (x > 0) // fall to the left
								{
									GridCellInfo& left = grid_info.cells[index - 1];
									GridCellInfo& left_below = grid_info.cells[index - 1 - grid_info.size.x];

									if (left.CanLock(particle) && left_below.CanLock(particle))
									{
										left.Lock(particle);
										particle->direction = { -1.0f, 0.0f };
										particle->speed = object_speed;
										break;
									}
								}
							}
							else
							{
								if (x < grid_info.size.x - 1) // fall to the right
								{
									GridCellInfo& right = grid_info.cells[index + 1];
									GridCellInfo& right_below = grid_info.cells[index + 1 - grid_info.size.x];

									if (right.CanLock(particle) && right_below.CanLock(particle))
									{
										right.Lock(particle);
										particle->direction = { +1.0f, 0.0f };
										particle->speed = object_speed;
										break;

									}
								}
							}
						}

					} // end step 1
				}
			} // end y
		} // end x
	} // end step


		
	// continue movement
	for (int x = 0; x < grid_info.size.x; ++x)
	{
		for (int y = 0; y < grid_info.size.y; ++y)
		{
			int index = x + y * grid_info.size.x;

			GameObjectParticle* particle = grid_info.cells[index].particle;
			if (particle == nullptr)
				continue;
			if (particle->type != GameObjectType::Rock && particle->type != GameObjectType::Diamond)
				continue;

			if (UpdateParticlePositionInGrid(particle, delta_time, grid_info))
			{
				glm::ivec2 p = grid_info.GetIndexForPosition(particle->bounding_box.position);
				if (p.y > 0)
				{
					for (int dx : {-1, 0, 1}) // check the 3 cells below for the player
					{
						glm::ivec2 other_index = glm::ivec2(p.x + dx, p.y - 1);
						if (!grid_info.IsInside(other_index))
							continue;

						GridCellInfo& other = grid_info(other_index);
						if (other.particle != nullptr)
						{
							if ((dx ==  0 /*&& other.particle->direction.y >= 0*/ )|| // object is not going down too !
								(dx == -1 && other.particle->direction.x > 0) || // other object is entering the cell under us
								(dx == +1 && other.particle->direction.x < 0))
							{

								if (other.particle->type == GameObjectType::Player)
								{

									KillPlayer(other.particle);
								}
								else if (other.particle->type == GameObjectType::Monster)
								{

									KillMonster(other.particle);
								}




							}
						}
					}
				}
			}
		}
	}

#if DEBUG_DRAW
	//debug draw
	glm::vec4 YELLOW = { 1.0f, 1.0f, 0.0f, 1.0f };
	glm::vec4 GREEN  = { 0.0f, 1.0f, 0.0f, 1.0f };

	for (int x = 0; x < grid_info.size.x; ++x)
	{
		for (int y = 0; y < grid_info.size.y; ++y)
		{
			GridCellInfo const& cell = grid_info(glm::ivec2(x, y));

			if (cell.locked)
			{
				box2 bb = grid_info.GetBoundingBox(cell);
				bb.half_size *= 0.9f;
				DrawBox(*DI, bb, YELLOW , false);

				DrawLine(*DI, bb.position, cell.locked_by_box.position, YELLOW);
			}

			if (cell.particle != nullptr && cell.particle->type == GameObjectType::Player)
			{
				box2 bb = grid_info.GetBoundingBox(cell);
				bb.half_size *= 0.5f;
				DrawBox(*DI, bb, GREEN, false);

			}
		}
	}
#endif



#endif

}

void LudumLevelInstance::HandlePlayerObject(float delta_time)
{	








#if 0


	LudumPlayer* player = GetPlayer(0);
	if (player == nullptr)
		return;
	
	PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return;

	ParticlePlayer* particle = pawn->GetParticle<ParticlePlayer>(0);
	if (particle == nullptr)
		return;

	Key const fake_displacement_key_buttons[] = { KeyboardButton::LEFT_CONTROL, KeyboardButton::RIGHT_CONTROL, GamepadButton::A, Key() };
	bool fake_displacement = player->CheckButtonPressed(fake_displacement_key_buttons);

	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();
	stick_position.x = MathTools::AnalogicToDiscret(stick_position.x);
	stick_position.y = MathTools::AnalogicToDiscret(stick_position.y);
	if (std::abs(stick_position.x) > std::abs(stick_position.y))
		stick_position.y = 0.0f;
	else
		stick_position.x = 0.0f;

	glm::ivec2 istick_position = RecastVector<glm::ivec2>(stick_position);

	// change pawn direction
	if (particle->direction.x == 0.0f && particle->direction.y == 0.0f)
	{
		if (stick_position != glm::vec2(0.0f, 0.0f))
		{
			glm::ivec2 p = grid_info.GetIndexForPosition(particle->bounding_box.position);

			glm::ivec2 other_index = (p + istick_position);

			if (grid_info.IsInside(other_index))
			{
				GridCellInfo& other = grid_info(other_index);

				bool can_go = false;

				if (other.particle != nullptr)
				{
					if (other.particle->type == GameObjectType::Foam)
					{
						other.particle->destroy_particle = true;
						if (!fake_displacement)
							other.particle = nullptr;
						can_go = true;
					}
					else if (other.particle->type == GameObjectType::Diamond)
					{
						other.particle->destroy_particle = true;
						TakeDiamond();
						if (!fake_displacement)
							other.particle = nullptr;
						can_go = true;
					}
					else if (other.particle->type == GameObjectType::Rock)
					{
						if (stick_position.x != 0.0f && other.particle->direction == glm::vec2(0.0f, 0.0f)) // can only push horizontally
						{
							glm::ivec2 rock_p = grid_info.GetCellCoord(other);
							if (grid_info.IsInside(rock_p + istick_position))
							{
								GridCellInfo& next_to_rock = grid_info(rock_p + istick_position);
								if (next_to_rock.CanLock(other.particle))
								{
									next_to_rock.Lock(other.particle);
									other.particle->direction = stick_position;
									other.particle->speed = push_speed;
									if (!fake_displacement)
									{
										particle->direction = stick_position;
										particle->speed = push_speed;
									}
								}
							}
						}
					}
				}
				else
					can_go = true;

				if (can_go && other.CanLock(particle))
				{
					if (!fake_displacement)
					{
						other.Lock(particle);
						particle->direction = stick_position;
						particle->speed = player_speed;
					}

				}
			}
		}

	}
	// update pawn position
	else
	{
		UpdateParticlePositionInGrid(particle, delta_time, grid_info);
	}


#endif


}

static glm::vec2 GetMonsterDirection(int direction_index)
{
	direction_index = direction_index % 4;
	if (direction_index == 0)
		return { 1.0f, 0.0f };
	else if (direction_index == 1)
		return { 0.0f, -1.0f };
	else if (direction_index == 2)
		return { -1.0f, 0.0f };
	else //if (direction_index == 3)
		return { 0.0f, 1.0f };
}

static int GetMonsterDirectionIndex(glm::vec2 const & direction)
{
	if (direction.x ==  1.0f)
		return 0;
	if (direction.y == -1.0f)
		return 1;
	if (direction.x == -1.0f)
		return 2;
	if (direction.y ==  1.0f)
		return 3;
	return 0;
}


void LudumLevelInstance::HandleMonsterObjects(float delta_time)
{









#if 0

	for (int x = 0; x < grid_info.size.x; ++x)
	{
		for (int y = 0; y < grid_info.size.y; ++y)
		{
			int index = x + y * grid_info.size.x;

			GameObjectParticle* particle = grid_info.cells[index].particle;
			if (particle == nullptr || particle->type != GameObjectType::Monster)
				continue;

			if (particle->direction == glm::vec2(0.0f, 0.0f))
			{
				glm::ivec2 p = grid_info.GetIndexForPosition(particle->bounding_box.position);

				glm::vec2 monster_direction = GetMonsterDirection(particle->monster_direction_index);

				glm::vec2 neighboor_offset = GetMonsterDirection((particle->monster_direction_index + 1) % 4); // the touching neighboor should be there

				glm::ivec2 neighboor_p = p + RecastVector<glm::ivec2>(neighboor_offset);

				// try keeping contact with a neighboor
				if (grid_info.IsInside(neighboor_p))
				{
					GridCellInfo& neighboor = grid_info(neighboor_p);

					// no touching neighboor. Check whether its position would be better
					if (neighboor.particle == nullptr)
					{
						if (neighboor.CanLock(particle))
						{
							glm::ivec2 neighboor_neighboor_offset = RecastVector<glm::ivec2>(GetMonsterDirection((particle->monster_direction_index + 2) % 4)); // the touching neighboor's neighboor should be there

							if (grid_info.IsInside(neighboor_p + neighboor_neighboor_offset) && grid_info(neighboor_p + neighboor_neighboor_offset).particle != nullptr)
							{
								neighboor.Lock(particle);
								particle->direction = neighboor_offset;
								particle->monster_direction_index = (particle->monster_direction_index + 1) % 4;
								particle->speed = object_speed;
							}
						}
					}
				}

				// direction still not found
				if (particle->direction == glm::vec2(0.0f, 0.0f))
				{
					glm::ivec2 other_index = p + RecastVector<glm::ivec2>(monster_direction);
					if (grid_info.IsInside(other_index))
					{
						if (grid_info(other_index).CanLock(particle))
						{
							grid_info(other_index).Lock(particle);
							particle->direction = monster_direction;
							particle->speed = object_speed;
						}

					}
					// still no direction : turn
					if (particle->direction == glm::vec2(0.0f, 0.0f))
					{
						particle->monster_direction_index = (particle->monster_direction_index - 1 + 4) % 4; // nothing more this frame
					}
				}
			}
			UpdateParticlePositionInGrid(particle, delta_time, grid_info);

			// search whether the player is touching the particle
			glm::ivec2 p = grid_info.GetIndexForPosition(particle->bounding_box.position);
			for (int axis : {0, 1})
			{
				for (int offset : {-1, 1})
				{
					glm::ivec2 other_p = p;
					other_p[axis] += offset;

					if (grid_info.IsInside(other_p))
					{
						GameObjectParticle* other_particle = grid_info(other_p).particle;
						if (other_particle != nullptr && other_particle->type == GameObjectType::Player)
						{
							KillPlayer(other_particle);
						}
					}
				}
			}

			


		}
	}

#endif

}

void LudumLevelInstance::HandleBlobObjects(float delta_time)
{


}

void LudumLevelInstance::HandleDisplacements(float delta_time)
{


}




bool LudumLevelInstance::InitializeLevelInstance(TMObjectReferenceSolver& reference_solver, TiledMap::PropertyOwner const* property_owner)
{
	if (!TMLevelInstance::InitializeLevelInstance(reference_solver, property_owner))
		return false;
	required_diamond_count = property_owner->GetPropertyValueInt("REQUIRED_DIAMOND", 10);
	frame_duration = property_owner->GetPropertyValueFloat("FRAME_DURATION", frame_duration);
	return true;
}


bool LudumLevelInstance::Initialize(Game * in_game, Level * in_level)
{
	if (!TMLevelInstance::Initialize(in_game, in_level))
		return false;

	LudumLevel* ludum_level = auto_cast(in_level);
	if (ludum_level != nullptr)
	{

	}	
	return true;
}


bool LudumLevelInstance::IsPlayerDead(Player* player)
{
	// ignore super ... just handle timeout component
	//if (TMLevelInstance::IsPlayerDead(player))
	//	return true;
	return false;
}

bool LudumLevelInstance::CheckLevelCompletion() const
{
	return level_complete;
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

	if (StringTools::Stricmp(name, "World") == 0)
		return COLLISION_GAMEOBJECT;
	if (StringTools::Stricmp(name, "Gate") == 0)
		return COLLISION_GATE;

	return TMLevelInstance::GetCollisionFlagByName(name);
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

		TMTileCollisionIterator it = GetTileCollisionIterator(GetBoundingBox(), COLLISION_GAMEOBJECT, false);
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

	// empty the grid
	for (int i = 0; i < grid_info.size.x * grid_info.size.y; ++i)
		grid_info.cells[i] = { nullptr, nullptr };
	
	// fill the grid
	TMTileCollisionIterator it = GetTileCollisionIterator(GetBoundingBox(), COLLISION_GAMEOBJECT, false);
	while (it)
	{
		grid_info(it->particle->bounding_box.position).particle = (GameObjectParticle*)it->particle;
		++it;
	}
}

void LudumLevelInstance::KillPlayer(GameObjectParticle* player)
{
	player->destroy_particle = true;
	glm::ivec2 player_p = grid_info.GetIndexForPosition(player->bounding_box.position);
	DiamondsCreationRequest(player_p);

}

void LudumLevelInstance::KillMonster(GameObjectParticle* monster)
{
	glm::ivec2 monster_p = grid_info.GetIndexForPosition(monster->bounding_box.position);
	DiamondsCreationRequest(monster_p);
}

void LudumLevelInstance::DiamondsCreationRequest(glm::ivec2 const & p)
{
	for (int dy : {-1, 0, +1})
	{
		for (int dx : {-1, 0, +1})
		{
			glm::ivec2 other_p = p + glm::ivec2(dx, dy);
			if (grid_info.IsInside(other_p))
			{
				GameObjectParticle* other = grid_info(other_p).particle;
				if (other != nullptr)
				{
					other->destroy_particle = true;
				}
				grid_info(other_p).create_diamond = true;
			}
		}
	}
}

void LudumLevelInstance::CreateDiamonds()
{
#if 0
	ParticleSpawner spawner = GetParticleSpawner("GameObjects", "diamond");
	if (!spawner.IsValid())
		return;

	for (int y = 0; y < grid_info.size.y; ++y)
	{
		for (int x = 0; x < grid_info.size.x; ++x)
		{
			GridCellInfo& cell = grid_info.cells[x + y * grid_info.size.y];
			if (cell.create_diamond)
			{
				spawner.SpawnParticles(1, true).Process([this, &cell](ParticleAccessor<GameObjectParticle> accessor)
				{
					for (GameObjectParticle& p : accessor)
					{
						p.bounding_box = grid_info.GetBoundingBox(cell);
						p.type = GameObjectType::Diamond;
						p.flags |= ParticleFlags::HEIGHT_BITS_MODE;

					}
				});
				cell.create_diamond = false;
			}
		}
	}
#endif
}


void LudumLevelInstance::TakeDiamond()
{
	if (++diamond_count == required_diamond_count)
	{
		door_opened = true;


	}
}




// ===========================================================================
// Standalone function
// ===========================================================================

#if 0

bool UpdateParticlePositionInGrid(GameObjectParticle* particle, float delta_time, class GridInfo& grid_info)
{
	bool result = (particle->direction.y < 0.0f);
	for (int axis : {0, 1})
	{
		if (particle->direction[axis] != 0.0f)
		{
			particle->offset[axis] = std::clamp(particle->offset[axis] + particle->speed * delta_time * particle->direction[axis], -1.0f, 1.0f);
			if (particle->offset[axis] == -1.0f || particle->offset[axis] == 1.0f)
			{
				if (grid_info(particle->bounding_box.position).particle == particle)
					grid_info(particle->bounding_box.position).particle = nullptr;
				particle->bounding_box.position += particle->direction * RecastVector<glm::vec2>(grid_info.tile_size);
				grid_info(particle->bounding_box.position).particle = particle;
				grid_info(particle->bounding_box.position).UnLock(particle);

				particle->offset = { 0.0f, 0.0f };
				particle->direction = { 0.0f, 0.0f };

				return result; // object was falling and is now stopped
			}
		}
	}
	return false; // movement not finished yet
}

#endif

