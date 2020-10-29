#include <chaos/Chaos.h>

#include "Ludum43HUD.h"
#include "Ludum43Game.h"
#include "Ludum43Level.h"
#include "Ludum43Player.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43GameInstance.h"

// ====================================================================
// GameHUDWakenParticleComponent
// ====================================================================

GameHUDWakenParticleComponent::GameHUDWakenParticleComponent(chaos::TagType in_layer_id) :
	chaos::GameHUDCacheValueComponent<int>("Particles: %d", -1, in_layer_id) 
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(20.0f, -80.0f);
	generator_params.hotpoint = chaos::Hotpoint::TOP_LEFT;
}

bool GameHUDWakenParticleComponent::UpdateCachedValue(bool & destroy_allocation)
{
	LudumPlayingHUD const * playing_hud = auto_cast(hud);
	if (playing_hud != nullptr)
	{
		LudumGameInstance const * ludum_game_instance = playing_hud->GetGameInstance();
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

	LudumGame const * ludum_game = playing_hud->GetGame();
	if (ludum_game == nullptr)
		return true;

	LudumPlayer const* ludum_player = playing_hud->GetPlayer(0);
	if (ludum_player == nullptr)
		return true;

	float life = ludum_player->GetHealth();
	if (life != cached_value)
	{
		// create the allocation
		if (allocations == nullptr)
		{
			chaos::ParticleLayerBase * layer = hud->GetParticleManager()->FindLayer(chaos::GameHUDKeys::LIFE_LAYER_ID);
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
		if (particles.GetDataCount() == 0)
			return true;

		chaos::box2 canvas_box = ludum_game->GetCanvasBox();

		glm::vec2 canvas_size = 2.0f * canvas_box.half_size;

		glm::vec2 position1, position2;
		position1.x = -canvas_size.x * 0.5f + 40.0f;
		position1.y = -canvas_size.y * 0.5f + 40.0f;

		position2.x = canvas_size.x * 0.5f - 40.0f;
		position2.y = -canvas_size.y * 0.5f + 70.0f;

		particles[0].bounding_box = chaos::box2(std::make_pair(position1, position2));
		particles[0].texcoords.bottomleft = glm::vec2(0.0f, 0.0f);
		particles[0].texcoords.topright = glm::vec2(ludum_player->GetMaxHealth(), 1.0f);
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
	if (!chaos::PlayingHUD::FillHUDContent())
		return false;
	RegisterComponent(chaos::GameHUDKeys::WAKENUP_PARTICLE_COUNT_ID, new GameHUDWakenParticleComponent());
	RegisterComponent(chaos::GameHUDKeys::LEVEL_TIMEOUT_ID, new chaos::GameHUDTimeoutComponent());
	RegisterComponent(chaos::GameHUDKeys::LIFE_HEALTH_ID, new GameHUDHealthBarComponent());

	RegisterComponent(chaos::GameHUDKeys::LEVEL_TITLE_ID, new chaos::GameHUDLevelTitleComponent());

	return true;
}

int LudumPlayingHUD::CreateHUDLayers()
{
	// call super method
	int render_order = chaos::PlayingHUD::CreateHUDLayers();
	if (render_order < 0)
		return render_order;
	// create a layer for the life bar
	LudumGame * ludum_game = GetGame();
	if (ludum_game != nullptr)
	{
		ParticleLifeLayerTrait life_trait;
		life_trait.game = ludum_game;
		particle_manager->AddLayer<ParticleLifeLayerTrait>(render_order++, chaos::GameHUDKeys::LIFE_LAYER_ID, "health", life_trait);
	}
	return render_order;
}
