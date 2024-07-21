#include "Ludum44PCH.h"
#include "Ludum44HUD.h"
#include "Ludum44Game.h"
#include "Ludum44Level.h"
#include "Ludum44Player.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44GameInstance.h"

// ====================================================================
// GameHUDLifeCountComponent
// ====================================================================

GameHUDLifeCountComponent::GameHUDLifeCountComponent() :
	GameHUDCacheValueTextComponent<int>("Life: %d")
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(20.0f, -80.0f);
	generator_params.hotpoint = chaos::Hotpoint::TOP_LEFT;
}

bool GameHUDLifeCountComponent::QueryValue(int & result) const
{
	LudumPlayer const* ludum_player = GetPlayer(0);
	if (ludum_player == nullptr)
		return false;
	result = ludum_player->GetLifeCount();
	return true;
}

// ====================================================================
// GameHUDPowerUpComponent
// ====================================================================

bool GameHUDPowerUpComponent::QueryValue(chaos::weak_ptr<LudumPowerUp> & result) const
{
	LudumGameInstance const* ludum_game_instance = GetGameInstance();
	if (ludum_game_instance == nullptr)
		return false;
	result = ludum_game_instance->current_power_up.get();
	return (result != nullptr);
}

void GameHUDPowerUpComponent::UpdateMesh()
{
	LudumGameInstance const* ludum_game_instance = GetGameInstance();
	if (ludum_game_instance == nullptr)
		return;

	bool decreasing_power_up = ludum_game_instance->current_powerup_trigger->decrease_power;

	// get box
	chaos::box2 canvas_box = GetGame()->GetCanvasBox();

	chaos::Hotpoint hotpoint = chaos::Hotpoint::BOTTOM_LEFT;
	glm::vec2 corner = GetCanvasBoxCorner(canvas_box, hotpoint);

	// create the level title
	chaos::ParticleTextGenerator::GeneratorParams params;
	params.line_height = 40;
	params.hotpoint = chaos::Hotpoint::BOTTOM_LEFT;
	params.position.x = corner.x + 40.0f;
	params.position.y = corner.y + 100.0f;
	params.default_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	params.font_info_name = "normal";
	params.bitmap_padding = glm::vec2(-5.0f, -5.0f);
	params.character_spacing = 0.0f;
	params.alignment = chaos::TextAlignment::CENTER;

	std::string title;

	if (decreasing_power_up)
		title = chaos::StringTools::Printf("Keep [ButtonY] or [KEYBOARD ALT] Pressed to sell\n[POWERUP %s]", cached_value->GetPowerUpTitle());
	else
		title = chaos::StringTools::Printf("Keep [ButtonY] or [KEYBOARD ALT] Pressed to buy\n[POWERUP %s]", cached_value->GetPowerUpTitle());

	chaos::GPUDrawInterface<chaos::VertexDefault> DI(nullptr);
	DrawText(DI, title.c_str(), params);
	mesh = DI.GetDynamicMesh();
}

// ====================================================================
// GameHUDHealthBarComponent
// ====================================================================

bool GameHUDHealthBarComponent::QueryValue(GameHUDHealthInfo& result) const
{
	LudumPlayer const* ludum_player = GetPlayer(0);
	if (ludum_player == nullptr)
		return false;
	LudumGameInstance const* ludum_game_instance = GetGameInstance();
	if (ludum_game_instance == nullptr)
		return false;
	LudumGame const* ludum_game = GetGame();
	if (ludum_game == nullptr)
		return false;

	float health = ludum_player->GetHealth();
	float max_health = ludum_player->GetMaxHealth();

	if (ludum_game_instance->current_power_up != nullptr && ludum_game_instance->current_powerup_trigger != nullptr)
	{
		bool decreasing_power_up = ludum_game_instance->current_powerup_trigger->decrease_power;

		float cost = ludum_game_instance->current_power_up->GetLifeCost();
		float paid_cost_ratio = ludum_player->GetBuyTimer() / ludum_game->GetBuyUpgradeTime();

		float sign1 = (decreasing_power_up) ? +1.0f : 0.0f;
		float sign2 = (decreasing_power_up) ? +1.0f : -1.0f;

		health = health + sign1 * cost * paid_cost_ratio;
		max_health = max_health + sign2 * cost * paid_cost_ratio;
		health = std::min(health, max_health);
	}

	result = { health,  max_health };
	return true;
}

void GameHUDHealthBarComponent::UpdateMesh()
{
	LudumGame const* ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	chaos::GPUDrawInterface<chaos::VertexDefault> DI(nullptr);

	chaos::box2 canvas_box = ludum_game->GetCanvasBox();
	glm::vec2 canvas_size = 2.0f * canvas_box.half_size;

	chaos::QuadPrimitive<chaos::VertexDefault> quads = DI.AddQuads(3);

	glm::vec2 position1, position2;
	position1.x = -canvas_size.x * 0.5f + 40.0f;
	position1.y = -canvas_size.y * 0.5f + 40.0f;

	position2.x = canvas_size.x * 0.5f - 40.0f;
	position2.y = -canvas_size.y * 0.5f + 70.0f;

	// compute the border size
	position2.x = position1.x + (position2.x - position1.x) * cached_value.max_health;

	chaos::ParticleDefault particle;
	particle.texcoords.bitmap_index = -1;
	// the border
	particle.bounding_box = chaos::box2(std::make_pair(position1, position2));
	particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	ParticleToPrimitive(particle, quads);
	++quads;

	// the background
	particle.bounding_box.half_size -= glm::vec2(3.0f, 3.0f);
	particle.color = glm::vec4(0.0, 0.0, 0.0, 1.0);
	ParticleToPrimitive(particle, quads);
	++quads;

	// the life bar
	std::pair<glm::vec2, glm::vec2> corners = chaos::GetBoxCorners(particle.bounding_box);
	corners.second.x = corners.first.x + (cached_value.health / cached_value.max_health) * (corners.second.x - corners.first.x);
	particle.bounding_box = chaos::box2(corners);
	particle.color = glm::vec4(1.0, 0.0, 0.0, 1.0);
	ParticleToPrimitive(particle, quads);
	++quads;

	mesh = DI.GetDynamicMesh();
}

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!chaos::PlayingHUD::FillHUDContent())
		return false;
	RegisterComponent(CreateHUDComponent<GameHUDHealthBarComponent>(chaos::GameHUDKeys::LIFE_HEALTH_ID));
	RegisterComponent(CreateHUDComponent<chaos::GameHUDLevelTitleComponent>(chaos::GameHUDKeys::LEVEL_TITLE_ID));
	RegisterComponent(CreateHUDComponent<GameHUDPowerUpComponent>(chaos::GameHUDKeys::POWER_UP_ID));
	RegisterComponent(CreateHUDComponent<GameHUDLifeCountComponent>(chaos::GameHUDKeys::LIFE_ID));
	RegisterComponent(CreateHUDComponent<chaos::GameHUDNotificationComponent>(chaos::GameHUDKeys::NOTIFICATION_ID));
	return true;
}