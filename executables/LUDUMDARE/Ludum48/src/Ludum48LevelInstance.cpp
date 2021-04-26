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

static glm::ivec2 GetAnalogicStickPosition(glm::vec2 stick_position)
{
	stick_position.x = MathTools::AnalogicToDiscret(stick_position.x);
	stick_position.y = MathTools::AnalogicToDiscret(stick_position.y);
	if (std::abs(stick_position.x) > std::abs(stick_position.y))
		stick_position.y = 0.0f;
	else
		stick_position.x = 0.0f;
	return RecastVector<glm::ivec2>(stick_position);
}

void LudumLevelInstance::CapturePlayerInputs()
{
	Player* player = GetPlayer(0);
	if (player == nullptr)
		return;

	bool beginning = (frame_timer < frame_duration * 0.3f);

	// Fake move
	Key const fake_move_keys[] = { KeyboardButton::LEFT_CONTROL, KeyboardButton::RIGHT_CONTROL, GamepadButton::A, Key() };
	if (player->CheckButtonPressed(fake_move_keys, false)) // current frame
	{
		if (beginning)
		{
			// button is just beeing pressed
			if (!player->CheckButtonPressed(fake_move_keys, true)) // previous frame
				cached_fake_move = true;
		}
		else
		{
			cached_fake_move = true;
		}
	}
	// Stick
	glm::ivec2 stick_position = GetAnalogicStickPosition(player->GetLeftStickPosition(false)); // current frame
	if (stick_position != glm::ivec2(0, 0))
	{
		if (beginning)
		{
			// stick is just beeing moved
			if (GetAnalogicStickPosition(player->GetLeftStickPosition(true)) != stick_position) // previous frame
				cached_stick_position = stick_position;
		}
		else
		{
			cached_stick_position = stick_position;
		}
	}
}

void LudumLevelInstance::FlushPlayerInputs()
{
	cached_fake_move = false;
	cached_stick_position = { 0, 0 };
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
		FlushPlayerInputs();
	}

	// normal frames	
	CapturePlayerInputs();
	DisplaceObjects(delta_time);

	// last frame
	frame_timer += delta_time;
	if (frame_timer >= frame_duration)
	{
		CommitDisplacements();
		DisplacementConsequences();
		FinalizeDisplacements();
		CreatePendingDiamonds(); // XXX : create particles only at the end, not to break all particles pointers in grid_info
		frame_timer = 0.0f;
	}

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
	bool play_stone_sound = false;

	

	for (GameObjectType type : {GameObjectType::Monster1, GameObjectType::Monster2, GameObjectType::Diamond, GameObjectType::Rock, GameObjectType::Player}) // order of priorities
	{
		for (size_t y = 0; y < grid_info.size.y; ++y)
		{
			for (size_t x = 0; x < grid_info.size.x; ++x)
			{
				glm::ivec2 p = { x, y };

				GridCellInfo& cell = grid_info(p);
				if (cell.particle != nullptr && cell.particle->type == type)
				{
					if (type == GameObjectType::Monster1 || type == GameObjectType::Monster2)
						NegociateMonsterDisplacement(p, cell);
					else if (type == GameObjectType::Diamond || type == GameObjectType::Rock)
					{
						NegociateFallerDisplacement(p, cell);

						play_stone_sound |= (cell.particle->falling_previous_frame && cell.particle->direction.y >= 0.0f);
						cell.particle->falling_previous_frame = (cell.particle->direction.y < 0.0f);
					}
					else if (type == GameObjectType::Player)
						NegociatePlayerDisplacement(p, cell);
				}
			}
		}
	}

	
	if (play_stone_sound)
		GetGame()->PlaySound("stone_falling", false, false, 0.0f, chaos::SoundContext::GAME);
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
		// nothing yet below
		if (below.locked_by != nullptr)
		{
			if (below.locked_by->type == GameObjectType::Monster1 || below.locked_by->type == GameObjectType::Monster2 || below.locked_by->type == GameObjectType::Player) // still "falling" but waiting for the objects to arrive to kill it
			{
				cell.particle->direction = { 0.0f, -1.0f };
				cell.particle->speed = 0.0f; // WAITING !
			}
		}
		// something below
		else
		{
			assert(below.particle != nullptr);
			if (below.particle->type == GameObjectType::Rock || below.particle->type == GameObjectType::Diamond || below.particle->type == GameObjectType::Wall || below.particle->type == GameObjectType::HardWall) // may slip on theses elements
			{
				if (below.particle->direction == glm::vec2(0.0f, 0.0f)) // may not slip on a moving objects
				{
					// check left or right position in a random order
					int random = rand();

					constexpr int directions[] = { -1, +1 };
					int index1 = random & 1;
					int index2 = index1 ^ 1;

					if (!TrySlipFaller(p, cell, directions[index1]))
						TrySlipFaller(p, cell, directions[index2]);
				}
			}
		}
	}
}

bool LudumLevelInstance::TrySlipFaller(glm::ivec2 const& p, GridCellInfo& cell, int direction)
{
	glm::ivec2 neighb_p = p + glm::ivec2(direction, 0);
	if (!grid_info.IsInside(neighb_p)) // if true, left below is inside too
		return false;

	glm::ivec2 neighb_below_p = p + glm::ivec2(direction, -1);
	assert(grid_info.IsInside(neighb_below_p));

	GridCellInfo& neighb = grid_info(neighb_p);
	GridCellInfo& neighb_below = grid_info(neighb_below_p);

	if (neighb.CanLock(cell.particle) && neighb_below.CanLock(cell.particle))
	{
		neighb.Lock(cell.particle); // XXX : donot lock neigb_below
		cell.particle->direction = { float(direction), 0.0f };
		cell.particle->speed = GetObjectSpeed();
		return true;
	}
	return false;
}


void LudumLevelInstance::NegociatePlayerDisplacement(glm::ivec2 const& p, GridCellInfo& cell)
{
	// early exit
	LudumPlayer* player = GetPlayer(0);
	if (player == nullptr)
		return;

	glm::ivec2 istick_position = cached_stick_position;

	// displace player
	if (istick_position != glm::ivec2(0, 0))
	{
		// check whether wanted cell exists
		glm::ivec2 p = grid_info.GetIndexForPosition(cell.particle->bounding_box.position);
		glm::ivec2 other_p = p + istick_position;
		if (!grid_info.IsInside(other_p))
			return;

		GridCellInfo& other_cell = grid_info(other_p);

		bool can_move = false;

		if (other_cell.particle != nullptr)
		{
			if (other_cell.particle->type == GameObjectType::Diamond)
			{
				TakeDiamond();
				other_cell.particle->destroy_particle = true;
				other_cell.particle = nullptr;
				can_move = true;
			}
			else if (other_cell.particle->type == GameObjectType::Foam)
			{
				other_cell.particle->destroy_particle = true;
				other_cell.particle = nullptr;
				can_move = true;
			}
			else if (other_cell.particle->type == GameObjectType::Rock)
			{
				if (istick_position.x != 0 && other_cell.particle->direction == glm::vec2(0.0f, 0.0f)) // can only push horizontally
				{
					glm::ivec2 next_other_p = other_p + istick_position;
					if (grid_info.IsInside(next_other_p))
					{
						GridCellInfo& next_other_cell = grid_info(next_other_p);
						if (next_other_cell.CanLock(other_cell.particle))
						{
							GetGame()->PlaySound("stone_scrub", false, false, 0.0f, chaos::SoundContext::GAME);

							next_other_cell.Lock(other_cell.particle);
							other_cell.particle->direction = RecastVector<glm::vec2>(istick_position);
							other_cell.particle->speed = GetObjectSpeed();
							can_move = true;
						}
					}
				}
			}
		}
		else
		{
			can_move = other_cell.CanLock(cell.particle);
		}
		
		if (can_move)
		{
			if (!cached_fake_move)
			{
				other_cell.Lock(cell.particle);
				cell.particle->direction = istick_position;
				cell.particle->speed = GetObjectSpeed();
			}
		}
	}
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

static int GetMonsterDirectionIndex(glm::vec2 const& direction)
{
	if (direction.x == 1.0f)
		return 0;
	if (direction.y == -1.0f)
		return 1;
	if (direction.x == -1.0f)
		return 2;
	if (direction.y == 1.0f)
		return 3;
	return 0;
}

void LudumLevelInstance::NegociateMonsterDisplacement(glm::ivec2 const& p, GridCellInfo& cell)
{
	int direction_index = cell.particle->monster_direction_index;

	// first : keep touching neighboor
	glm::ivec2 neighb_dir = RecastVector<glm::ivec2>(GetMonsterDirection(direction_index + 1));

	glm::ivec2 neighb_p = p + neighb_dir;
	if (grid_info.IsInside(neighb_p))
	{
		GridCellInfo& neighb_cell = grid_info(neighb_p);
		if (neighb_cell.CanLock(cell.particle))
		{
			glm::ivec2 neighb_neighb_dir = RecastVector<glm::ivec2>(GetMonsterDirection(direction_index + 2));

			glm::ivec2 neigb_neigb_p = neighb_p + neighb_neighb_dir;
			assert(grid_info.IsInside(neigb_neigb_p));

			GridCellInfo& neighb_neighb_cell = grid_info(neigb_neigb_p);
			if (neighb_neighb_cell.particle != nullptr)
			{
				neighb_cell.Lock(cell.particle);
				cell.particle->direction = RecastVector<glm::vec2>(neighb_dir);
				cell.particle->speed = GetObjectSpeed();
				cell.particle->monster_direction_index = (direction_index + 1) % 4;
				return;
			}
		}
	}

	// check remaining 3 directions
	for (int i = 0; i < 3; ++i)
	{
		glm::ivec2 other_dir = RecastVector<glm::ivec2>(GetMonsterDirection(direction_index + 4 - i));

		glm::ivec2 other_p = p + other_dir;
		if (grid_info.IsInside(other_p))
		{
			GridCellInfo& other_cell = grid_info(other_p);
			if (other_cell.CanLock(cell.particle))
			{
				other_cell.Lock(cell.particle);
				cell.particle->direction = RecastVector<glm::vec2>(other_dir);
				cell.particle->speed = GetObjectSpeed();
				cell.particle->monster_direction_index = (direction_index + 4 - i) % 4;
				return;
			}
		}
	}
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

void LudumLevelInstance::CommitDisplacements()
{
	for (int y = 0; y < grid_info.size.y; ++y)
	{
		for (int x = 0; x < grid_info.size.x; ++x)
		{
			glm::ivec2 p = { x, y };

			GridCellInfo& cell = grid_info(p);
			if (cell.particle != nullptr && cell.particle->speed > 0.0f && cell.particle->direction != glm::vec2(0.0f, 0.0f))
			{
				cell.particle->bounding_box.position += cell.particle->direction * RecastVector<glm::vec2>(grid_info.tile_size);
				cell.particle->offset = { 0.0f, 0.0f };
				cell.particle = nullptr;
			}
		}
	}

	for (int y = 0; y < grid_info.size.y; ++y)
	{
		for (int x = 0; x < grid_info.size.x; ++x)
		{
			glm::ivec2 p = { x, y };

			GridCellInfo& cell = grid_info(p);
			if (cell.locked_by != nullptr)
				cell.particle = cell.locked_by;
			cell.locked_by = nullptr;
		}
	}
}

void LudumLevelInstance::DisplacementConsequences()
{
	for (int y = 0; y < grid_info.size.y; ++y)
	{
		for (int x = 0; x < grid_info.size.x; ++x)
		{
			glm::ivec2 p = { x, y };

			GridCellInfo& cell = grid_info(p);
			if (cell.particle == nullptr)
				continue;

			// ROCK / DIAMOND
			if (cell.particle->type == GameObjectType::Rock || cell.particle->type == GameObjectType::Diamond)
			{
				if (cell.particle->direction.y == -1.0f) // even if speed = 0
				{
					glm::ivec2 other_p = p + glm::ivec2(0, -1);
					if (grid_info.IsInside(other_p))
					{
						GridCellInfo& other_cell = grid_info(other_p);
						if (other_cell.particle != nullptr)
						{
							if (other_cell.particle->type == GameObjectType::Player)
								KillPlayer(other_cell.particle);
							else if (other_cell.particle->type == GameObjectType::Monster1 || other_cell.particle->type == GameObjectType::Monster2)
								KillMonster(other_cell.particle, other_cell.particle->type == GameObjectType::Monster1);
						}
					}
				}
			}
			// MONSTER
			else if (cell.particle->type == GameObjectType::Monster1 || cell.particle->type == GameObjectType::Monster2) // search for player in neighboor
			{
				for (int axis : {0, 1})
				{
					for (int delta : {-1, +1})
					{
						glm::ivec2 other_p = p;
						other_p[axis] += delta;
						if (grid_info.IsInside(other_p))
						{
							GridCellInfo& other_cell = grid_info(other_p);
							if (other_cell.particle != nullptr && other_cell.particle->type == GameObjectType::Player)
								KillPlayer(other_cell.particle);
						}
					}
				}
			}
			//
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
			if (cell.particle != nullptr)
			{
				cell.particle->offset = { 0.0f, 0.0f };
				cell.particle->direction = { 0.0f, 0.0f };
				cell.particle->speed = 0.0f;
			}
		}
	}
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
	DestroyNeighboorsAndCreateDiamonds(player_p, false);

	GetGame()->PlaySound("explosion", false, false, 0.0f, chaos::SoundContext::GAME);
}

void LudumLevelInstance::KillMonster(GameObjectParticle* monster, bool create_diamond)
{
	glm::ivec2 monster_p = grid_info.GetIndexForPosition(monster->bounding_box.position);
	DestroyNeighboorsAndCreateDiamonds(monster_p, create_diamond);

	GetGame()->PlaySound("explosion", false, false, 0.0f, chaos::SoundContext::GAME);
}

void LudumLevelInstance::DestroyNeighboorsAndCreateDiamonds(glm::ivec2 const & p, bool create_diamond)
{
	ParticleSpawner spawner = GetParticleSpawner("Smoke", "Smoke");

	for (int dy : {-1, 0, +1})
	{
		for (int dx : {-1, 0, +1})
		{
			glm::ivec2 other_p = p + glm::ivec2(dx, dy);
			if (grid_info.IsInside(other_p))
			{		
				bool create_smoke = false;

				GameObjectParticle* other = grid_info(other_p).particle;
				if (other != nullptr && other->type != GameObjectType::HardWall)
					create_smoke = other->destroy_particle = true;
				if (create_diamond)
					if (other == nullptr || other->type != GameObjectType::HardWall)
						create_smoke = grid_info(other_p).create_diamond = true;

				if (create_smoke && spawner.IsValid())
				{
					spawner.SpawnParticles(20, false).Process([other_p, this](ParticleAccessor<SmokeParticle> accessor)
					{
						box2 b = grid_info.GetBoundingBox(grid_info(other_p));

						for (SmokeParticle& particle : accessor)
						{
							float size_ratio = MathTools::RandFloat(0.5f, 0.8f);

							float angle = MathTools::RandFloat(0.0f, 6.28f);
							float r = MathTools::RandFloat(0.0f, 0.5f) * b.half_size.x;

							glm::vec2 offset = { r * std::cos(angle), r * std::sin(angle) };

							particle.bounding_box.position = b.position + offset;
							particle.bounding_box.half_size = b.half_size * size_ratio;
							particle.velocity = 7.0f * offset;
							particle.lifetime = MathTools::RandFloat(0.5f, 1.5f);

							float color_ratio = MathTools::RandFloat(0.7f, 1.0f);

							particle.color.x *= color_ratio;
							particle.color.y *= color_ratio;
							particle.color.z *= color_ratio;

						}
					});
				}

			}
		}
	}
}

void LudumLevelInstance::CreatePendingDiamonds()
{
	ParticleSpawner spawner = GetParticleSpawner("GameObjects", "diamond");
	if (!spawner.IsValid())
		return;

	for (int y = 0; y < grid_info.size.y; ++y)
	{
		for (int x = 0; x < grid_info.size.x; ++x)
		{
			glm::ivec2 p = { x, y };

			GridCellInfo& cell = grid_info(p);
			if (cell.create_diamond)
			{
				spawner.SpawnParticles(1, true).Process([this, &cell](ParticleAccessor<GameObjectParticle> accessor)
				{
					for (GameObjectParticle& particle : accessor)
					{
						particle.bounding_box = grid_info.GetBoundingBox(cell);
						particle.type = GameObjectType::Diamond;
						particle.flags |= ParticleFlags::HEIGHT_BITS_MODE;

					}
				});
				cell.create_diamond = false;
			}
		}
	}
}


void LudumLevelInstance::TakeDiamond()
{
	GetGame()->PlaySound("bonus1", false, false, 0.0f, chaos::SoundContext::GAME);
	if (++diamond_count == required_diamond_count)
	{
		door_opened = true;

		GetGame()->PlaySound("door", false, false, 0.0f, chaos::SoundContext::GAME);
	}
}