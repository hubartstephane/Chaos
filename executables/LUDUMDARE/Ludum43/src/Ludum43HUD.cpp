#include "Ludum43HUD.h"
#include "Ludum43Game.h"
#include "Ludum43Level.h"
#include "Ludum43Player.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43GameInstance.h"

#include <death/GameHUDComponent.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_HUD(Ludum);



// ====================================================================
// GameHUDWakenParticleComponent
// ====================================================================

GameHUDWakenParticleComponent::GameHUDWakenParticleComponent(chaos::TagType in_layer_id) :
	death::GameHUDCacheValueComponent<int>("Particles: %d", -1, in_layer_id) 
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(20.0f, -80.0f);
	generator_params.hotpoint_type = chaos::Hotpoint::TOP_LEFT;
}

bool GameHUDWakenParticleComponent::UpdateCachedValue(bool & destroy_allocation)
{
	LudumPlayingHUD const * playing_hud = auto_cast(hud);
	if (playing_hud != nullptr)
	{
		LudumGameInstance const * ludum_game_instance = playing_hud->GetLudumGameInstance();
		if (ludum_game_instance != nullptr)
		{
			int waken_up_particle_count = ludum_game_instance->GetWakenUpParticleCount();
			if (waken_up_particle_count != cached_value)
			{
				cached_value = waken_up_particle_count;
				return true;
			}
		}
	}
	return false;
}

// ====================================================================
// GameHUDHealthBarComponent
// ====================================================================

bool GameHUDHealthBarComponent::DoTick(float delta_time)
{
	LudumPlayingHUD const * playing_hud = auto_cast(hud);
	if (playing_hud == nullptr)
		return true;

	LudumGame const * ludum_game = playing_hud->GetLudumGame();
	if (ludum_game == nullptr)
		return true;

	LudumPlayer const* ludum_player = playing_hud->GetLudumPlayer(0);
	if (ludum_player == nullptr)
		return true;

	float life = ludum_game->GetPlayerParticleLife(0);
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
		chaos::ParticleAccessor<ParticleLife> particles = allocations->GetParticleAccessor();
		if (particles.GetCount() == 0)
			return true;

		glm::vec2 canvas_size = ludum_game->GetCanvasSize();

		glm::vec2 position1, position2;
		position1.x = -canvas_size.x * 0.5f + 40.0f;
		position1.y = -canvas_size.y * 0.5f + 40.0f;

		position2.x = canvas_size.x * 0.5f - 40.0f;
		position2.y = -canvas_size.y * 0.5f + 70.0f;

		particles[0].bounding_box = chaos::box2(std::make_pair(position1, position2));
		particles[0].texcoords.bottomleft = glm::vec2(0.0f, 0.0f);
		particles[0].texcoords.topright = glm::vec2(ludum_player->GetHealth(), 1.0f);
		particles[0].color = glm::vec4(life, life, life, life);

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
	RegisterComponent(death::GameHUDKeys::WAKENUP_PARTICLE_COUNT_ID, new GameHUDWakenParticleComponent());
	RegisterComponent(death::GameHUDKeys::LEVEL_TIMEOUT_ID, new death::GameHUDTimeoutComponent());
	RegisterComponent(death::GameHUDKeys::LIFE_HEALTH_ID, new GameHUDHealthBarComponent());

	RegisterComponent(death::GameHUDKeys::LEVEL_TITLE_ID, new death::GameHUDLevelTitleComponent());

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
		particle_manager->AddLayer<ParticleLifeTrait>(++render_order, death::GameHUDKeys::LIFE_LAYER_ID, "health", life_trait);
	}
	return true;
}
