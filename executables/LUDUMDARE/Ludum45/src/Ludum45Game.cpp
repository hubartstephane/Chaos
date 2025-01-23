#include "Ludum45PCH.h"
#include "Ludum45Game.h"
#include "Ludum45Particles.h"
#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45HUD.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

LudumGame::LudumGame()
{
	game_name = "Jesaouf\nVI";
	game_instructions = R"INSTRUCTIONS(
	[ButtonA] or [KEYBOARD SPACE] : Fire
	[ButtonB] or [KEYBOARD CTRL]  : Dash
	[ButtonY] or [KEYBOARD ALT]   : Bright side of life)INSTRUCTIONS";

	looping_levels = false;

	game_instance_class = LudumGameInstance::GetStaticClass();
}

bool LudumGame::OnEnterGame(chaos::PhysicalGamepad * in_physical_gamepad)
{
	if (!chaos::Game::OnEnterGame(in_physical_gamepad))
		return false;
    PlaySound("start", false, false, 0.0f, chaos::SoundContext::GAME);
	return true;
}


chaos::GameHUD * LudumGame::DoCreatePlayingHUD()
{
	return new LudumPlayingHUD();
}

bool LudumGame::OnReadConfigurableProperties(chaos::JSONReadConfiguration config, chaos::ReadConfigurablePropertiesContext context)
{
	if (!chaos::Game::OnReadConfigurableProperties(config, context))
		return false;

	CHAOS_JSON_ATTRIBUTE(config, player_speed_factor);
	CHAOS_JSON_ATTRIBUTE(config, player_speed_damping);
	CHAOS_JSON_ATTRIBUTE(config, player_tan_speed_damping);
	CHAOS_JSON_ATTRIBUTE(config, player_dash_velocity_boost);
	CHAOS_JSON_ATTRIBUTE(config, player_dash_duration);

	CHAOS_JSON_ATTRIBUTE(config, fire_size_ratio);
	CHAOS_JSON_ATTRIBUTE(config, fire_velocity);

	CHAOS_JSON_ATTRIBUTE(config, scroll_factor);

	CHAOS_JSON_ATTRIBUTE(config, player_speeds);
	CHAOS_JSON_ATTRIBUTE(config, player_damages);
	CHAOS_JSON_ATTRIBUTE(config, player_dash_cooldowns);
	CHAOS_JSON_ATTRIBUTE(config, player_power_rates);
	CHAOS_JSON_ATTRIBUTE(config, player_power_spreads);
	CHAOS_JSON_ATTRIBUTE(config, player_specialpowers);

	return true;
}

chaos::TMLevel * LudumGame::CreateTMLevel()
{
	return new LudumLevel();
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const * config)
{
	if (!chaos::Game::InitializeFromConfiguration(config))
		return false;

	RegisterEnemyTypes();

	return true;
}

void LudumGame::RegisterObjectTypeDefinition(char const * prefix, chaos::LightweightFunction<void(chaos::TiledMap::ObjectTypeDefinition const*)> func)
{
	size_t prefix_length = strlen(prefix);

	chaos::TiledMap::Manager const * manager = GetTiledMapManager();
	if (manager == nullptr)
		return;

	size_t ots_count = manager->GetObjectTypeSetCount();
	for (size_t i = 0 ; i < ots_count ; ++i)
	{
		chaos::TiledMap::ObjectTypeSet const * ots = manager->GetObjectTypeSet(i);
		if (ots == nullptr)
			continue;

		size_t def_count = ots->GetObjectTypeDefinitionCount();
		for (size_t j = 0 ; j < def_count ; ++j)
		{
			chaos::TiledMap::ObjectTypeDefinition const * def = ots->GetObjectTypeDefinition(j);
			if (def != nullptr)
				if (chaos::StringTools::Strnicmp(prefix, def->name, prefix_length) == 0)
					func(def);
		}
	}
}

void LudumGame::DoRegisterEnemyType(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	EnemyType * type = new EnemyType;
	if (type == nullptr)
		return;
	type->name = def->name;
	if (!type->Initialize(def))
		delete(type);
	else
		enemy_types.push_back(type);
}



void LudumGame::RegisterEnemyTypes()
{
	RegisterObjectTypeDefinition("ET_", [this](chaos::TiledMap::ObjectTypeDefinition const * def)
	{
		DoRegisterEnemyType(def);
	});
}

EnemyType * LudumGame::FindEnemyType(char const * name)
{
	for (auto const & t : enemy_types)
		if (chaos::StringTools::Stricmp(t->name, name) == 0)
			return t.get();
	return nullptr;
}

EnemyType const * LudumGame::FindEnemyType(char const * name) const
{
	for (auto const & t : enemy_types)
		if (chaos::StringTools::Stricmp(t->name, name) == 0)
			return t.get();
	return nullptr;
}


void LudumGame::DoDisplayGame(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::GPURenderParams const & render_params)
{
	chaos::Game::DoDisplayGame(render_context, uniform_provider, render_params);

	LudumPlayer * ludum_player = GetPlayer(0);

	if (ludum_player != nullptr && ludum_player->brightsideoflife)
	{
		chaos::GPUResourceManager * resource_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager == nullptr)
			return;
		chaos::GPURenderMaterial * postprocess_material = resource_manager->FindRenderMaterial("brightsideoflife");
		if (postprocess_material == nullptr)
			return;

		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		render_context->DrawFullscreenQuad(postprocess_material, uniform_provider, render_params);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}




















