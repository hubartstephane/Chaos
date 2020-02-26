#include "Ludum41Level.h"
#include "Ludum41LevelInstance.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"
#include "Ludum41Player.h"

bool LudumLevelInstance::Initialize(death::Game * in_game, death::Level * in_level)
{
	if (!death::LevelInstance::Initialize(in_game, in_level))
		return false;

	bricks_allocations = CreateBricks();

	return true;
}

bool LudumLevelInstance::CheckLevelCompletion() const
{
	if (death::LevelInstance::CheckLevelCompletion())
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

chaos::box2 LudumLevelInstance::GetBoundingBox() const
{
	return game->GetCanvasBox();
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	death::LevelInstance::DoTick(delta_time);
	RestrictPlayerToWorld(GetPlayer(0));
	return true;
}

chaos::ParticleAllocationBase * LudumLevelInstance::CreateBricks()
{
	LudumLevel const * ludum_level = GetLevel();
	if (ludum_level == nullptr)
		return nullptr;

	LudumGame const * ludum_game = GetGame();
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
	size_t brick_count = ludum_level->GetBrickCount();
	chaos::ParticleAllocationBase * result = game->GetGameParticleCreator().SpawnParticles(death::GameHUDKeys::BRICK_LAYER_ID, "brick", brick_count, true);
	if (result == nullptr)
		return nullptr;

	chaos::ParticleAccessor<ParticleBrick> particles = result->GetParticleAccessor();
	if (particles.GetCount() == 0)
		return nullptr;

	// compute the brick size
	float BRICK_ASPECT = 16.0f / 9.0f;

	glm::vec2 canvas_size = ludum_game->GetCanvasSize();

	glm::vec2 particle_size;
	particle_size.x = canvas_size.x / (float)ludum_game->brick_per_line;
	particle_size.y = particle_size.x / BRICK_ASPECT;

	// fill the brick
	size_t k = 0;
	for (size_t i = 0; i < ludum_level->bricks.size(); ++i)
	{
		std::vector<int> const & line = ludum_level->bricks[i];
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
				particles[k].color = indestructible_color;
				particles[k].indestructible = true;
				particles[k].life = 1.0f;
			}
			else 
			{
				particles[k].indestructible = false;

				size_t color_index = std::min((size_t)b, color_count - 1);
				particles[k].color = colors[color_index];
				particles[k].life = (float)b;
			}

			particles[k].starting_life = particles[k].life;

			// position
			glm::vec2 position;
			position.x = -canvas_size.x * 0.5f + particle_size.x * (float)j;
			position.y = canvas_size.y * 0.5f - particle_size.y * (float)i;

			particles[k].bounding_box.position = chaos::ConvertHotpoint(position, particle_size, chaos::Hotpoint::TOP_LEFT, chaos::Hotpoint::CENTER);
			particles[k].bounding_box.half_size = 0.5f * particle_size;

			++k;
		}
	}

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
	if (particles.GetCount() == 0)
		return nullptr;

	return &particles[0];
}

ParticleBrick const * LudumLevelInstance::GetBricks() const 
{
	if (bricks_allocations == nullptr)
		return nullptr;

	chaos::ParticleConstAccessor<ParticleBrick> p = bricks_allocations->GetParticleConstAccessor<ParticleBrick>();
	if (p.GetCount() == 0)
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
	death::LevelInstance::CreateBackgroundImage(); // fallback
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
	death::LevelInstance::SetInGameMusic(); // fallback
}
