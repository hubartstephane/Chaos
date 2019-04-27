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

	float life = 0.0f; //ludum_game->GetPlayerLife(0);
	if (life != cached_value)
	{
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

		glm::vec2 view_size = ludum_game->GetViewSize();

		glm::vec2 position1, position2;
		position1.x = -view_size.x * 0.5f + 40.0f;
		position1.y = -view_size.y * 0.5f + 40.0f;

		position2.x = view_size.x * 0.5f - 40.0f;
		position2.y = -view_size.y * 0.5f + 70.0f;

		particles->bounding_box = chaos::box2(std::make_pair(position1, position2));
		particles->texcoords.bottomleft = glm::vec2(0.0f, 0.0f);
		//particles->texcoords.topright = glm::vec2(ludum_game->initial_player_life, 1.0f);
		particles->texcoords.topright = glm::vec2(0.0f, 1.0f);
		particles->color = glm::vec4(life, life, life, life);

		cached_value = life;
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
