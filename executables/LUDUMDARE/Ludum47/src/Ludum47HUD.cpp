#include <chaos/Chaos.h>

#include "Ludum47HUD.h"
#include "Ludum47Game.h"
#include "Ludum47Level.h"
#include "Ludum47Player.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47GameInstance.h"

// ====================================================================
// GameHUDRacePositionComponent
// ====================================================================

GameHUDRacePositionComponent::GameHUDRacePositionComponent(chaos::TagType in_layer_id) :
	chaos::GameHUDCacheValueComponent<glm::ivec2>("Pos %d/%d", glm::ivec2(-1, -1) , in_layer_id)
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(20.0f, -20.0f);
	generator_params.hotpoint = chaos::Hotpoint::TOP_LEFT;
}

std::string GameHUDRacePositionComponent::FormatText() const
{
	char const* f = ((cached_value.x > cached_value.y / 2) && blink_value) ? "[WARNING Pos %d/%d]" : "Pos %d/%d";




	return chaos::StringTools::Printf(f, 1 + cached_value.x, 1 + cached_value.y);
}


bool GameHUDRacePositionComponent::DoTick(float delta_time)
{
	if (!chaos::GameHUDCacheValueComponent<glm::ivec2>::DoTick(delta_time))
		return false;
	current_dt = delta_time;
	return true;
}

bool GameHUDRacePositionComponent::UpdateCachedValue(bool& destroy_allocation)
{
	LudumPlayingHUD const* playing_hud = auto_cast(hud);
	if (playing_hud != nullptr)
	{
		LudumLevelInstance const* li = playing_hud->GetLevelInstance();
		if (li != nullptr && li->road != nullptr)
		{
			LudumPlayer* player = GetPlayer(0);
			if (player != nullptr && !player->race_position.IsCompleted())
			{


				glm::ivec2 p = li->GetPlayerRacePosition(player);

				
				bool force_update = false;
				if (p.x > p.y / 2)
				{
					blink_timer = std::max(blink_timer - current_dt, 0.0f);
					if (blink_timer == 0.0f)
					{
						blink_timer = 0.5f;
						blink_value = !blink_value;

						LudumLevelInstance* li = GetLevelInstance();
						if (li != nullptr && li->effective_start_timer == 0.0)
						{
							if (blink_value)
								player->GetGame()->PlaySound("bip", false, false, 0.0f, chaos::SoundContext::GAME);
						}
					}
					force_update = true;
				}


				if (force_update || p != cached_value)
				{
					cached_value = p;
					return true;
				}
			}
		}
	}
	return false;
}

// ====================================================================
// GameHUDRaceLapsComponent
// ====================================================================

GameHUDRaceLapsComponent::GameHUDRaceLapsComponent(chaos::TagType in_layer_id) :
	chaos::GameHUDCacheValueComponent<glm::ivec2>("Lap %d/%d", glm::ivec2(-1, -1), in_layer_id)
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(20.0f, -80.0f);
	generator_params.hotpoint = chaos::Hotpoint::TOP_LEFT;
}

std::string GameHUDRaceLapsComponent::FormatText() const
{
	return chaos::StringTools::Printf(format.c_str(), 1 + cached_value.x, cached_value.y);
}

bool GameHUDRaceLapsComponent::UpdateCachedValue(bool& destroy_allocation)
{
	LudumPlayingHUD const* playing_hud = auto_cast(hud);
	if (playing_hud != nullptr)
	{
		LudumLevelInstance const* li = playing_hud->GetLevelInstance();
		if (li != nullptr && li->road != nullptr)
		{
			LudumPlayer* player = GetPlayer(0);
			if (player != nullptr && !player->race_position.IsCompleted())
			{
				glm::ivec2 v = { player->race_position.current_lap, li->road->lap_count };

				if (v != cached_value)
				{
					cached_value = v;
					return true;
				}
			}
		}
	}
	return false;
}


// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{

	// directely call GameHUD super instead of PlayingHUD to avoid the score

#if 0
	if (!chaos::PlayingHUD::FillHUDContent())
		return false;	
#else
	// call super method
	if (!chaos::GameHUD::FillHUDContent())
		return false;
	//RegisterComponent(GameHUDKeys::SCORE_ID, new GameHUDScoreComponent());
#endif

	RegisterComponent(chaos::GameHUDKeys::RACE_POSITION_ID, new GameHUDRacePositionComponent());
	RegisterComponent(chaos::GameHUDKeys::RACE_LAPS_ID, new GameHUDRaceLapsComponent());

	return true;
}