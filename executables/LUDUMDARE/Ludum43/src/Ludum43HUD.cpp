#include "Ludum43PCH.h"
#include "Ludum43HUD.h"
#include "Ludum43Game.h"
#include "Ludum43Level.h"
#include "Ludum43Player.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43GameInstance.h"

// ====================================================================
// GameHUDWakenParticleComponent
// ====================================================================

GameHUDWakenParticleComponent::GameHUDWakenParticleComponent() :
	chaos::GameHUDCacheValueTextComponent<int>("Particles: %d")
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(20.0f, -80.0f);
	generator_params.hotpoint = chaos::Hotpoint::TOP_LEFT;
}

bool GameHUDWakenParticleComponent::QueryValue(int & result) const
{
	LudumGameInstance const* ludum_game_instance = GetGameInstance();
	if (ludum_game_instance == nullptr)
		return false;
	result = ludum_game_instance->GetWakenUpParticleCount();
	return true;
}

// ====================================================================
// GameHUDHealthBarComponent
// ====================================================================

bool GameHUDHealthBarComponent::QueryValue(GameHUDHealthInfo& result) const
{
	LudumPlayer const* ludum_player = GetPlayer(0);
	if (ludum_player == nullptr)
		return false;
	result = { ludum_player->GetHealth() , ludum_player->GetMaxHealth() };
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
	RegisterComponent(CreateHUDComponent<GameHUDWakenParticleComponent>(chaos::GameHUDKeys::WAKENUP_PARTICLE_COUNT_ID));
	RegisterComponent(CreateHUDComponent<chaos::GameHUDTimeoutComponent>(chaos::GameHUDKeys::LEVEL_TIMEOUT_ID));
	RegisterComponent(CreateHUDComponent<GameHUDHealthBarComponent>(chaos::GameHUDKeys::LIFE_HEALTH_ID));
	RegisterComponent(CreateHUDComponent<chaos::GameHUDLevelTitleComponent>(chaos::GameHUDKeys::LEVEL_TITLE_ID));
	return true;
}
