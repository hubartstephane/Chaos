#include "Ludum44HUD.h"
#include "Ludum44Game.h"
#include "Ludum44Level.h"
#include "Ludum44Player.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44GameInstance.h"

#include <death/GameHUDComponent.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_HUD(Ludum);

// ====================================================================
// GameHUDLifeCountComponent
// ====================================================================

bool GameHUDLifeCountComponent::UpdateCachedValue(bool & destroy_allocation)
{
	LudumPlayingHUD const * playing_hud = auto_cast(hud);
	if (playing_hud != nullptr)
	{
		LudumPlayer const * ludum_player = playing_hud->GetLudumPlayer(0);
		if (ludum_player == nullptr) 
			destroy_allocation = true;
		else
		{
			int current_life_count = ludum_player->GetLifeCount();
			if (current_life_count != cached_value)
			{
				cached_value = current_life_count;
				return true;
			}
		
		}
	}
	return false;
}

// ====================================================================
// GameHUDPowerUpComponent
// ====================================================================


bool GameHUDPowerUpComponent::DoTick(double delta_time)
{
	death::GameHUDSingleAllocationComponent::DoTick(delta_time);

	LudumGameInstance * ludum_game_instance = auto_cast(GetGameInstance());
	if (ludum_game_instance == nullptr)
	{
		cached_power_up = nullptr;
		allocations = nullptr;
		return true;	
	}

	if (ludum_game_instance->current_power_up == nullptr)
	{
		cached_power_up = nullptr;
		allocations = nullptr;
		return true;		
	}

	if (cached_power_up.get() == ludum_game_instance->current_power_up.get())
		return true;

	bool decreasing_power_up = ludum_game_instance->current_power_up_surface->GetGeometricObject()->FindPropertyBool("DECREASE_POWER_UP", false);

	// ensure we do not have already cached this power_up
	cached_power_up = ludum_game_instance->current_power_up.get();

	// get box
	chaos::box2 canvas_box = GetGame()->GetCanvasBox();		

	int hotpoint = chaos::Hotpoint::BOTTOM_LEFT;
	glm::vec2 corner = GetCanvasBoxCorner(canvas_box, hotpoint);

	// create the level title
	chaos::ParticleTextGenerator::GeneratorParams params;
	params.line_height = 40;
	params.hotpoint_type = chaos::Hotpoint::BOTTOM_LEFT;
	params.position.x = corner.x + 40.0f;
	params.position.y = corner.y + 100.0f;
	params.default_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	params.font_info_name = "normal";
	params.bitmap_padding = glm::vec2(-5.0f, -5.0f);
	params.character_spacing = 0.0f;
	params.alignment = chaos::ParticleTextGenerator::GeneratorParams::ALIGN_CENTER;

	std::string title;
	
	if (decreasing_power_up)
		title = chaos::StringTools::Printf("Keep [ButtonY] or [KEYBOARD ALT] Pressed to sell\n[POWERUP %s]", cached_power_up->GetPowerUpTitle());		
	else
		title = chaos::StringTools::Printf("Keep [ButtonY] or [KEYBOARD ALT] Pressed to buy\n[POWERUP %s]", cached_power_up->GetPowerUpTitle());
		

	allocations = hud->GetGameParticleCreator().CreateTextParticles(title.c_str(), params, death::GameHUDKeys::TEXT_LAYER_ID);


	return true;
}

// ====================================================================
// GameHUDLifeBarComponent
// ====================================================================

bool GameHUDLifeBarComponent::DoTick(double delta_time)
{
	LudumPlayingHUD const * playing_hud = auto_cast(hud);
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

	glm::vec2 canvas_size = ludum_game->GetCanvasSize();

	glm::vec2 position1, position2;
	position1.x = -canvas_size.x * 0.5f + 40.0f;
	position1.y = -canvas_size.y * 0.5f + 40.0f;

	position2.x = canvas_size.x * 0.5f - 40.0f;
	position2.y = -canvas_size.y * 0.5f + 80.0f;

	part->bounding_box = chaos::box2(std::make_pair(position1, position2));
	part->texcoords.bottomleft = glm::vec2(0.0f, 0.0f);
	part->texcoords.topright = glm::vec2(1.0f, 1.0f);
	part->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);



	part->color.x = ludum_player->GetCurrentLife();
	part->color.y = ludum_player->GetCurrentMaxLife();
	part->color.z = 0.0f;
	part->color.w = 0.0f;

	if (ludum_game_instance->current_power_up != nullptr && ludum_game_instance->current_power_up_surface != nullptr)
	{
		bool decreasing_power_up = ludum_game_instance->current_power_up_surface->GetGeometricObject()->FindPropertyBool("DECREASE_POWER_UP", false);

		float cost = ludum_game_instance->current_power_up->GetLifeCost();
		float paid_cost_ratio = ludum_player->GetBuyTimer() / ludum_game->GetBuyUpgradeTime();

		float sign1 = (decreasing_power_up) ? 1.0f : 0.0f;
		float sign2 = (decreasing_power_up) ? 1.0f : -1.0f;

		part->color.x += sign1 * cost * paid_cost_ratio;
		part->color.y += sign2 * cost * paid_cost_ratio;
		if (part->color.y > 1.0f)
			part->color.y = 1.0f;
		if (part->color.x > part->color.y)
			part->color.x = part->color.y;
	}
	
	return true;
}

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{

	// shuwww

	if (!death::PlayingHUD::FillHUDContent())
		return false;	
	RegisterComponent(death::GameHUDKeys::LIFE_VITAE_ID, new GameHUDLifeBarComponent());
	RegisterComponent(death::GameHUDKeys::LEVEL_TITLE_ID, new death::GameHUDLevelTitleComponent());
	RegisterComponent(death::GameHUDKeys::POWER_UP_ID, new GameHUDPowerUpComponent());
	RegisterComponent(death::GameHUDKeys::LIFE_ID, new GameHUDLifeCountComponent("normal", 60.0f, glm::vec2(20.0f, -80.0f), chaos::Hotpoint::TOP_LEFT, death::GameHUDKeys::TEXT_LAYER_ID));
	return true;
}


bool LudumPlayingHUD::CreateHUDLayers()
{

	// shuwww


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
