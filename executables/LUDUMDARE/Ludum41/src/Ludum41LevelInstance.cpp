#include "Ludum41PCH.h"
#include "Ludum41Level.h"
#include "Ludum41LevelInstance.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"
#include "Ludum41Player.h"

bool LudumLevelInstance::Initialize(chaos::Game * in_game, chaos::Level * in_level)
{
	if (!chaos::LevelInstance::Initialize(in_game, in_level))
		return false;

	bricks_allocations = CreateBricks();

	return true;
}

bool LudumLevelInstance::CheckLevelCompletion() const
{
	if (chaos::LevelInstance::CheckLevelCompletion())
		return true;

	LudumLevel const * ludum_level = GetLevel();
	if (ludum_level == nullptr)
		return false;

	// the only bricks remaining are the one that cannot be destroyed
	if (GetBrickCount() == ludum_level->indestructible_brick_count)
		return true;

	return false;
}

bool LudumLevelInstance::CanCompleteLevel() const
{
	LudumGameInstance const * ludum_game_instance = GetGameInstance();
	if (ludum_game_instance == nullptr)
		return true;
	if (ludum_game_instance->CanStartChallengeBallIndex(true) != std::numeric_limits<size_t>::max()) // any ball is going down
		return true;
	return false;
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	chaos::LevelInstance::DoTick(delta_time);
	RestrictPawnToWorld(GetPlayer(0));
	return true;
}

chaos::ParticleAllocationBase * LudumLevelInstance::CreateBricks()
{
	LudumLevel const * ludum_level = GetLevel();
	if (ludum_level == nullptr)
		return nullptr;

	chaos::WindowApplication* window_application = chaos::Application::GetInstance();
	if (window_application == nullptr)
		return nullptr;

	LudumGame * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return nullptr;

	glm::vec4 const indestructible_color = glm::vec4(1.0f, 0.4f, 0.0f, 1.0f);

	glm::vec4 const colors[] = {
		glm::vec4(0.7f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.7f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 0.7f, 1.0f)
	};

	size_t color_count = sizeof(colors) / sizeof(colors[0]);

	// create the bricks resource
	chaos::ParticleSpawner spawner = ludum_game->GetParticleSpawner(chaos::GameHUDKeys::BRICK_LAYER_ID, "brick");

	size_t brick_count = ludum_level->GetBrickCount();

	chaos::ParticleAllocationBase* result = spawner.SpawnParticles(brick_count, true).Process([=](chaos::ParticleAccessor<ParticleBrick> accessor)
	{
		// compute the brick size
		float BRICK_ASPECT = 16.0f / 9.0f;

		chaos::box2 canvas_box = ludum_game->GetCanvasBox();

		glm::vec2 canvas_size = canvas_box.half_size * 2.0f;

		glm::vec2 particle_size;
		particle_size.x = canvas_size.x / (float)ludum_game->brick_per_line;
		particle_size.y = particle_size.x / BRICK_ASPECT;

		// fill the brick
		size_t k = 0;
		for (size_t i = 0; i < ludum_level->bricks.size(); ++i)
		{
			std::vector<int> const& line = ludum_level->bricks[i];
			for (size_t j = 0; j < line.size(); ++j)
			{
				int b = line[j];
				if (b == LudumLevel::NONE)
					continue;
				if (b < 0 && b != LudumLevel::INDESTRUCTIBLE)
					continue;

				// compute color / indestructible / life
				size_t life = 1;

				if (b == LudumLevel::INDESTRUCTIBLE)
				{
					accessor[k].color = indestructible_color;
					accessor[k].indestructible = true;
					accessor[k].life = 1.0f;
				}
				else
				{
					accessor[k].indestructible = false;

					size_t color_index = std::min((size_t)b, color_count - 1);
					accessor[k].color = colors[color_index];
					accessor[k].life = (float)b;
				}

				accessor[k].starting_life = accessor[k].life;

				// position
				glm::vec2 position;
				position.x = -canvas_size.x * 0.5f + particle_size.x * (float)j;
				position.y = canvas_size.y * 0.5f - particle_size.y * (float)i;

				accessor[k].bounding_box.position = chaos::ConvertHotpoint(position, particle_size, chaos::Hotpoint::TOP_LEFT, chaos::Hotpoint::CENTER);
				accessor[k].bounding_box.half_size = 0.5f * particle_size;

				++k;
			}
		}
	});
	return result;
}

size_t LudumLevelInstance::GetBrickCount() const
{
	if (bricks_allocations == nullptr)
		return 0;
	return bricks_allocations->GetParticleCount();
}

ParticleBrick * LudumLevelInstance::GetBricks()
{
	if (bricks_allocations == nullptr)
		return nullptr;

	chaos::ParticleAccessor<ParticleBrick> particles = bricks_allocations->GetParticleAccessor();
	if (particles.GetDataCount() == 0)
		return nullptr;

	return &particles[0];
}

ParticleBrick const * LudumLevelInstance::GetBricks() const
{
	if (bricks_allocations == nullptr)
		return nullptr;

	chaos::ParticleConstAccessor<ParticleBrick> p = bricks_allocations->GetParticleConstAccessor<ParticleBrick>();
	if (p.GetDataCount() == 0)
		return nullptr;
	return &p[0];
}

void LudumLevelInstance::CreateBackgroundImage()
{
	LudumLevel const * ludum_level = GetLevel();
	if (ludum_level != nullptr)
	{
		char const * background_material = (ludum_level->background_material.empty())? nullptr : ludum_level->background_material.c_str();
		char const * background_texture = (ludum_level->background_texture.empty())? nullptr : ludum_level->background_texture.c_str();
		if (background_material != nullptr || background_texture != nullptr)
		{
			game->CreateBackgroundImage(background_material, background_texture);
			return;
		}
	}
	chaos::LevelInstance::CreateBackgroundImage(); // fallback
}

void LudumLevelInstance::SetInGameMusic()
{
	LudumLevel const * ludum_level = GetLevel();
	if (ludum_level != nullptr)
	{
		if (!ludum_level->music.empty())
		{
			game->SetInGameMusic(ludum_level->music.c_str());
			return;
		}
	}
	chaos::LevelInstance::SetInGameMusic(); // fallback
}


chaos::PlayerPawn * LudumLevelInstance::CreatePlayerPawn(chaos::Player* player)
{
	assert(player != nullptr);

	chaos::PlayerPawn* player_pawn = LevelInstance::CreatePlayerPawn(player);
	if (player_pawn == nullptr)
		return nullptr;

	chaos::WindowApplication* window_application = chaos::Application::GetInstance();
	if (window_application == nullptr)
		return nullptr;

	chaos::ParticleSpawner spawner = GetGame()->GetParticleSpawner(chaos::GameHUDKeys::GAMEOBJECT_LAYER_ID, "player");

	// spawn particles for the pawn
	chaos::ParticleAllocationBase* player_allocation = spawner.SpawnParticles(1, true);
	if (player_allocation == nullptr)
		return player_pawn;
	player_pawn->SetAllocation(player_allocation);
	// initialize the player particle
	chaos::ParticleAccessor<ParticleObject> particles = player_allocation->GetParticleAccessor();
	for (ParticleObject& particle : particles)
	{
		particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		particle.bounding_box.position = glm::vec2(0.0f, 0.0f);
		particle.bounding_box.half_size = glm::vec2(0.0f, 0.0f);
	}

	LudumGame* ludum_game = GetGame();
	LudumPlayer* ludum_player = auto_cast(player);

	// raw copy of LudumPlayer::SetPlayerLength(...) (while pawn is not set yet we cannot call this function)
	float player_length = std::clamp(ludum_player->player_length, ludum_game->player_min_length, ludum_game->player_max_length);

	if (player_pawn != nullptr)
	{
		chaos::box2 box = player_pawn->GetBoundingBox();
		box.half_size = glm::vec2(player_length * 0.5f, LudumPlayer::PLAYER_HEIGHT * 0.5f);
		player_pawn->SetBoundingBox(box);

		RestrictPawnToWorld(player_pawn);
	}

	return player_pawn;
}
