#include "Ludum44HUD.h"
#include "Ludum44Game.h"
#include "Ludum44Level.h"
#include "Ludum44Player.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44GameInstance.h"

#include <death/GameHUDComponent.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_HUD(Ludum);

// ====================================================================
// GameHUDLifeBarComponent
// ====================================================================

bool GameHUDLifeBarComponent::DoTick(double delta_time)
{
	LudumPlayingHUD const * playing_hud = dynamic_cast<LudumPlayingHUD const*>(hud);
	if (playing_hud == nullptr)
		return true;

	LudumGame const * ludum_game = playing_hud->GetLudumGame();
	if (ludum_game == nullptr)
		return true;

	LudumGameInstance const * ludum_game_instance = playing_hud->GetLudumGameInstance();
	if (ludum_game_instance == nullptr)
		return true;

	LudumPlayer const * ludum_player = ludum_game->GetLudumPlayer(0);
	if (ludum_player == nullptr)
		return true;



	// create the allocation
	if (allocations == nullptr)
	{
		chaos::ParticleLayerBase * layer = hud->GetParticleManager()->FindLayer(death::GameHUDKeys::LIFE_LAYER_ID);
		if (layer == nullptr)
			return true;
		allocations = layer->SpawnParticles(1);
		if (allocations == nullptr)
			return true;
	}
	else
	{
		allocations->Resize(1);
	}
	// fill the particle
	chaos::ParticleAccessor<ParticleLife> particles = allocations->GetParticleAccessor<ParticleLife>();
	if (particles.GetCount() == 0)
		return true;

	ParticleLife * part = &particles[0];

	glm::vec2 view_size = ludum_game->GetViewSize();

	glm::vec2 position1, position2;
	position1.x = -view_size.x * 0.5f + 40.0f;
	position1.y = -view_size.y * 0.5f + 40.0f;

	position2.x = view_size.x * 0.5f - 40.0f;
	position2.y = -view_size.y * 0.5f + 90.0f;

	part->bounding_box = chaos::box2(std::make_pair(position1, position2));
	part->texcoords.bottomleft = glm::vec2(0.0f, 0.0f);
	part->texcoords.topright = glm::vec2(1.0f, 1.0f);
	part->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	part->color.x = ludum_player->GetCurrentLife();
	part->color.y = ludum_player->GetCurrentMaxLife();
	part->color.z = 0.0f;
	part->color.w = 0.0f;	
	if (ludum_game_instance->current_power_up != nullptr)
	{		
		part->color.z = ludum_game_instance->current_power_up->GetLifeCost();
		part->color.w = ludum_player->GetBuyTimer() / ludum_game->GetBuyUpgradeTime();
	}
	
	return true;
}

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!death::PlayingHUD::FillHUDContent())
		return false;
	RegisterComponent(death::GameHUDKeys::LEVEL_TIMEOUT_ID, new death::GameHUDTimeoutComponent());
	RegisterComponent(death::GameHUDKeys::LIFE_VITAE_ID, new GameHUDLifeBarComponent());
	return true;
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

		ParticleLifeTrait::LayerTrait life_trait;
		life_trait.game = ludum_game;
		particle_manager->AddLayer<ParticleLifeTrait>(++render_order, death::GameHUDKeys::LIFE_LAYER_ID, "lifebar", life_trait);
	}

	return true;
}
