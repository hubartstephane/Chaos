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

bool GridCellInfo::CanLock(GameObjectParticle* p) const
{
	return !locked && particle == nullptr;
}

void GridCellInfo::Lock(GameObjectParticle* p)
{
	assert(p != nullptr);
	p->locked_cell = this;
#if _DEBUG
	locked_by_box = p->bounding_box;
#endif
	locked = true;
}

void GridCellInfo::UnLock(GameObjectParticle* p)
{
	assert(p != nullptr);
	locked = false;
	p->locked_cell = nullptr;
}





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

glm::ivec2 GridInfo::GetCellCoord(GridCellInfo const& cell) const
{
	int offset = int(&cell - &cells[0]);
	return { offset % size.x, offset / size.x };
}

chaos::box2 GridInfo::GetBoundingBox(GridCellInfo const& cell) const
{
	chaos::box2 result;
	result.position = min_position + tile_size * chaos::RecastVector<glm::vec2>(GetCellCoord(cell));

	result.half_size = tile_size * 0.5f;
	return result;
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

	if (grid_info.cells != nullptr)
	{
		HandlePlayerObject(delta_time);
		HandleFallingObjects(delta_time);
	}


	chaos::TMLevelInstance::DoTick(delta_time);


	// completed ?
	if (completion_timer > 0.0f)
		completion_timer = std::max(0.0f, completion_timer - delta_time);
	return true;
}


bool LudumLevelInstance::HandleFallingObjects(float delta_time)
{
	auto DI = chaos::GetDebugDrawInterface();

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
#if _DEBUG
							if (below.particle != nullptr)
							{
								glm::vec4 RED = { 1.0f, 0.0f, 0.0f, 1.0f };

								chaos::DrawBox(*DI, below.particle->bounding_box, RED, false);
								chaos::DrawLine(*DI, particle->bounding_box.position, below.particle->bounding_box.position, RED);
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
						GridCellInfo& other = grid_info(glm::ivec2(p.x + dx, p.y - 1));
						if (other.particle != nullptr)
						{
							if (other.particle->type == GameObjectType::Player)
							{
								if ((dx == 0) ||
									(dx == -1 && other.particle->direction.x > 0) ||
									(dx == +1 && other.particle->direction.x < 0))
								{



									x = x;
								}
							}
						}

					}



				}
			}
		}
	}

#if _DEBUG
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
				chaos::box2 bb = grid_info.GetBoundingBox(cell);
				bb.half_size *= 0.9f;
				chaos::DrawBox(*DI, bb, YELLOW , false);

				chaos::DrawLine(*DI, bb.position, cell.locked_by_box.position, YELLOW);
			}

			if (cell.particle != nullptr && cell.particle->type == GameObjectType::Player)
			{
				chaos::box2 bb = grid_info.GetBoundingBox(cell);
				bb.half_size *= 0.5f;
				chaos::DrawBox(*DI, bb, GREEN, false);

			}
		}
	}
#endif



	return true;
}


bool LudumLevelInstance::HandlePlayerObject(float delta_time)
{	
	LudumPlayer* player = GetPlayer(0);
	if (player == nullptr)
		return true;

	LudumPlayerDisplacementComponent* displacement_component = player->GetDisplacementComponent();
	if (displacement_component == nullptr)
		return true;
	
	chaos::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	ParticlePlayer* particle = pawn->GetParticle<ParticlePlayer>(0);
	if (particle == nullptr)
		return true;


	chaos::Key const fake_displacement_key_buttons[] = { chaos::KeyboardButton::LEFT_CONTROL, chaos::KeyboardButton::RIGHT_CONTROL, chaos::GamepadButton::A, chaos::Key() };
	bool fake_displacement = player->CheckButtonPressed(fake_displacement_key_buttons);


	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();
	stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
	stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	if (std::abs(stick_position.x) > std::abs(stick_position.y))
		stick_position.y = 0.0f;
	else
		stick_position.x = 0.0f;

	// change pawn direction
	if (particle->direction.x == 0.0f && particle->direction.y == 0.0f)
	{
		if (stick_position != glm::vec2(0.0f, 0.0f))
		{
			glm::ivec2 p = grid_info.GetIndexForPosition(particle->bounding_box.position);

			bool inside_world = false;
			if (stick_position.x > 0 && p.x < grid_info.size.x - 1)
				inside_world = true;
			else if (stick_position.x < 0 && p.x > 0.0f)
				inside_world = true;
			else if (stick_position.y > 0 && p.y < grid_info.size.y - 1)
				inside_world = true;
			else if (stick_position.y < 0 && p.y > 0.0f)
				inside_world = true;

			if (inside_world)
			{
				glm::ivec2 other_index = (p + chaos::RecastVector<glm::ivec2>(stick_position));

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
							if ((rock_p.x > 0 && stick_position.x < 0.0f) || (rock_p.x < grid_info.size.x - 1 && stick_position.x > 0.0f))
							{
								GridCellInfo& next_to_rock = grid_info(rock_p + chaos::RecastVector<glm::ivec2>(stick_position));
								if (next_to_rock.CanLock(other.particle))
								{
									next_to_rock.Lock(other.particle);
									other.particle->direction = stick_position;
									other.particle->speed = displacement_component->push_speed;
									if (!fake_displacement)
									{
										particle->direction = stick_position;
										particle->speed = displacement_component->push_speed;
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
						particle->speed = displacement_component->pawn_speed;
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















	return true;
}

bool LudumLevelInstance::InitializeLevelInstance(chaos::TMObjectReferenceSolver& reference_solver, chaos::TiledMap::PropertyOwner const* property_owner)
{
	if (!chaos::TMLevelInstance::InitializeLevelInstance(reference_solver, property_owner))
		return false;

	required_diamond_count = property_owner->GetPropertyValueInt("REQUIRED_DIAMOND", 10);
	object_speed = property_owner->GetPropertyValueFloat("OBJECT_SPEED", 0.5f);

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
		grid_info(it->particle->bounding_box.position).particle = (GameObjectParticle*)it->particle;
		++it;
	}
}

void LudumLevelInstance::TakeDiamond()
{
	++diamond_count;
}
