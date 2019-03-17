#include "Ludum41HUD.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"
#include "Ludum41Level.h"
#include "Ludum41LevelInstance.h"
#include "Ludum41Player.h"

DEATH_GAMEFRAMEWORK_IMPLEMENT_HUD(Ludum);


// ====================================================================
// GameHUDComboComponent
// ====================================================================

bool GameHUDComboComponent::UpdateCachedValue(bool & destroy_allocation)
{
	LudumPlayingHUD const * playing_hud = dynamic_cast<LudumPlayingHUD const*>(hud);
	if (playing_hud != nullptr)
	{
		LudumGameInstance const * ludum_game_instance = playing_hud->GetLudumGameInstance();
		if (ludum_game_instance != nullptr)
		{
			int current_combo = ludum_game_instance->GetCurrentComboMultiplier();
			if (current_combo < 2)
				destroy_allocation = true;
			if (current_combo != cached_value)
			{
				cached_value = current_combo;
				return true;
			}
		}
	}
	return false;
}

void GameHUDComboComponent::TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box)
{
	int hotpoint = chaos::Hotpoint::TOP_LEFT;

	glm::vec2 corner = GetViewBoxCorner(view_box, hotpoint);
	params.hotpoint_type = hotpoint;
	params.position.x = corner.x + 20.0f;
	params.position.y = corner.y - 60.0f;
}

// ====================================================================
// GameHUDLifeComponent
// ====================================================================

bool GameHUDLifeComponent::DoTick(double delta_time)
{
	// super call
	death::GameHUDSingleAllocationComponent::DoTick(delta_time);
	// get the player
	death::Player const * player = GetGame()->GetPlayer(0);
	if (player == nullptr)
		return true;
	// get player life, destroy the allocation if no more life
	int current_life = player->GetLifeCount();
	if (current_life <= 0)
	{
		allocations = nullptr;
		return true;
	}
	// create/ resize the allocation
	if (allocations == nullptr)
	{
		allocations = hud->GetGameParticleCreator().CreateParticles("life", current_life, death::GameHUDKeys::LIFE_LAYER_ID);
	}
	else
	{
		allocations->Resize(current_life);
		if (current_life > cached_value)
			hud->GetGameParticleCreator().InitializeParticles(allocations.get(), "life", current_life - cached_value);
	}

	// set the color
	chaos::ParticleAccessor<ParticleObject> particles = allocations->GetParticleAccessor<ParticleObject>();

	glm::vec2 corner = GetViewBoxCorner(GetGame()->GetViewBox(), chaos::Hotpoint::BOTTOM_LEFT);

	glm::vec2 particle_size;
	particle_size.x = 35.0f;
	particle_size.y = 20.0f;

	for (size_t i = 0; i < (size_t)current_life; ++i)
	{
		glm::vec2 position;
		position.x = corner.x + 20.0f + (particle_size.x + 5.0f) * (float)i;
		position.y = corner.y + 20.0f;

		particles[i].bounding_box.position = chaos::Hotpoint::Convert(position, particle_size, chaos::Hotpoint::BOTTOM_LEFT, chaos::Hotpoint::CENTER);
		particles[i].bounding_box.half_size = 0.5f * particle_size;

		particles[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	cached_value = current_life;
	

#if 0


	// has something changed ?
	if (current_life == cached_value)
		return true;




#endif
	return true;
}




// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!death::PlayingHUD::FillHUDContent())
		return false;

	RegisterComponent(death::GameHUDKeys::COMBO_ID, new GameHUDComboComponent());
	RegisterComponent(death::GameHUDKeys::LIFE_ID, new GameHUDLifeComponent());

	return true;
}






bool LudumPlayingHUD::DoTick(double delta_time)
{
	// call super method
	PlayingHUD::DoTick(delta_time);
	// update other objects
//	UpdateLifeParticles();
//	TickHeartWarning(delta_time);
	return true;
}

void LudumPlayingHUD::UpdateLifeParticles()
{
	death::Player const * player = game->GetPlayer(0);
	if (player == nullptr)
		return;

	int current_life = player->GetLifeCount();
	if (current_life == cached_value)
		return;

	if (current_life < 0)
		UnregisterParticles(death::GameHUDKeys::LIFE_ID);
	else
	{
		chaos::ParticleAllocation * allocation = FindParticleAllocation(death::GameHUDKeys::LIFE_ID);
		if (allocation == nullptr)
		{
			allocation = GetGameParticleCreator().CreateParticles("life", current_life, death::GameHUDKeys::LIFE_LAYER_ID);
			if (allocation == nullptr)
				return;
			RegisterParticles(death::GameHUDKeys::LIFE_ID, allocation);
		}
		else
		{
			allocation->Resize(current_life);
			if (current_life > cached_value)
				GetGameParticleCreator().InitializeParticles(allocation, "life", current_life - cached_value);
		}

		// set the color
		chaos::ParticleAccessor<ParticleObject> particles = allocation->GetParticleAccessor<ParticleObject>();

		glm::vec2 view_size = game->GetViewSize();

		glm::vec2 particle_size;
		particle_size.x = 35.0f;
		particle_size.y = 20.0f;

		for (size_t i = 0; i < (size_t)current_life; ++i)
		{
			glm::vec2 position;
			position.x = -view_size.x * 0.5f + 20.0f + (particle_size.x + 5.0f) * (float)i;
			position.y = -view_size.y * 0.5f + 15.0f;

			particles[i].bounding_box.position = chaos::Hotpoint::Convert(position, particle_size, chaos::Hotpoint::BOTTOM_LEFT, chaos::Hotpoint::CENTER);
			particles[i].bounding_box.half_size = 0.5f * particle_size;

			particles[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
	cached_value = current_life;
}

void LudumPlayingHUD::TickHeartWarning(double delta_time)
{
	death::Player const * player = game->GetPlayer(0);
	if (player == nullptr)
		return;

	int current_life = player->GetLifeCount();
	if (current_life == 1)
	{
		heart_warning -= heart_beat_speed * (float)delta_time;
		if (heart_warning <= 0.0f)
		{
			game->PlaySound("heartbeat", false, false);

			float fractionnal_part, integer_part;
			fractionnal_part = modf(heart_warning, &integer_part);

			heart_warning = (1.0f + fractionnal_part);
		}
	}
	else
		heart_warning = 1.0f;
}

bool LudumPlayingHUD::CreateHUDLayers()
{
	// call super method
	if (!death::PlayingHUD::CreateHUDLayers())
		return false;
	// create a layer for the life bar
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game != nullptr)
	{
		int render_order = -1;
		ParticleLifeObjectTrait life_trait;
		life_trait.game = ludum_game;
		particle_manager->AddLayer<ParticleLifeObjectTrait>(++render_order, death::GameHUDKeys::LIFE_LAYER_ID, "gameobject", life_trait);
	}
	return true;
}



