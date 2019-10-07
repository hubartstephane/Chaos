#include "Ludum45Game.h"
#include "Ludum45Particles.h"
#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45HUD.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

#include <chaos/JSONTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/MathTools.h>
#include <chaos/InputMode.h>
#include <chaos/GeometryFramework.h>
#include <chaos/CollisionFramework.h>
#include <chaos/GPUFramebufferGenerator.h>
#include <chaos/GLTools.h>

#include <death/GameParticles.h>
#include <death/SoundContext.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAME(Ludum);

LudumGame::LudumGame()
{		
	game_name = "Jesaouf\nVI";
	game_instructions = R"INSTRUCTIONS(
	[ButtonA] or [KEYBOARD SPACE] : Fire
	[ButtonB] or [KEYBOARD CTRL]  : Dash
	[ButtonY] or [KEYBOARD ALT]   : Bright side of life)INSTRUCTIONS";

	looping_levels = false;
}

bool LudumGame::OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
{
	if (!death::Game::OnEnterGame(in_physical_gamepad))
		return false;
	Play("start", false, false, 0.0f, death::SoundContext::GAME);
	return true;
}


death::GameHUD * LudumGame::DoCreatePlayingHUD()
{
	return new LudumPlayingHUD(this);
}

chaos::SM::StateMachine * LudumGame::DoCreateGameStateMachine()
{
	return new LudumStateMachine(this);
}

bool LudumGame::DeclareParticleClasses()
{
	if (!Game::DeclareParticleClasses())
		return false;
	chaos::ClassTools::DeclareClass<ParticlePlayer,ParticleBase>("ParticlePlayer");
	chaos::ClassTools::DeclareClass<ParticleFire, ParticleBase>("ParticleFire");
	chaos::ClassTools::DeclareClass<ParticleBonus, ParticleBase>("ParticleBonus");
	chaos::ClassTools::DeclareClass<ParticleEnemy, ParticleBase>("ParticleEnemy");
	chaos::ClassTools::DeclareClass<ParticleExplosion, chaos::ParticleDefault::Particle>("ParticleExplosion");



	
	return true;
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!death::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;

	DEATHGAME_JSON_ATTRIBUTE(player_speed_factor);
	DEATHGAME_JSON_ATTRIBUTE(player_speed_damping);
	DEATHGAME_JSON_ATTRIBUTE(player_tan_speed_damping);
	DEATHGAME_JSON_ATTRIBUTE(player_dash_velocity_boost);	
	DEATHGAME_JSON_ATTRIBUTE(player_dash_duration);
	DEATHGAME_JSON_ATTRIBUTE(fire_size_ratio);	
	DEATHGAME_JSON_ATTRIBUTE(fire_velocity);	
	
	DEATHGAME_JSON_ATTRIBUTE(scroll_factor);
	DEATHGAME_JSON_ATTRIBUTE(initial_player_life);
	
	DEATHGAME_JSON_ATTRIBUTE(player_speeds);
	DEATHGAME_JSON_ATTRIBUTE(player_damages);
	DEATHGAME_JSON_ATTRIBUTE(player_dash_cooldowns);
	DEATHGAME_JSON_ATTRIBUTE(player_power_rates);
	DEATHGAME_JSON_ATTRIBUTE(player_power_spreads);
	DEATHGAME_JSON_ATTRIBUTE(player_specialpowers);
						
	return true;
}

death::TiledMap::Level * LudumGame::CreateTiledMapLevel()
{
	return new LudumLevel();
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeFromConfiguration(config, config_path))
		return false;

	RegisterEnemyPatterns();
	RegisterEnemyTypes();

	return true;
}

void LudumGame::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
{
	// super method
	death::Game::OnLevelChanged(new_level, old_level, new_level_instance);

	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		Play("next_level", false, false, 0.0f, death::SoundContext::GAME);
}

ParticlePlayer * LudumGame::GetPlayerParticle(int player_index)
{
	LudumPlayer * player = GetLudumPlayer(player_index);
	if (player == nullptr)
		return nullptr;
	return player->GetPlayerParticle();
}

ParticlePlayer const * LudumGame::GetPlayerParticle(int player_index) const
{
	LudumPlayer const * player = GetLudumPlayer(player_index);
	if (player == nullptr)
		return nullptr;
	return player->GetPlayerParticle();
}

chaos::box2 LudumGame::GetWorldBox() const
{
	if (current_level_instance != nullptr)
		 return current_level_instance->GetBoundingBox();
	return chaos::box2();
}

death::GameInstance * LudumGame::CreateGameInstance()
{
	return new LudumGameInstance(this);
}









template<typename FUNC>
void LudumGame::RegisterObjectTypeDefinition(char const * prefix, FUNC func)
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


void LudumGame::DoRegisterEnemyPattern(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	EnemyPattern * pattern = new EnemyPattern;
	if (pattern == nullptr)
		return;
	pattern->name = def->name;
	if (!pattern->Initialize(def))
		delete(pattern);
	else
		enemy_patterns.push_back(pattern);
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

void LudumGame::RegisterEnemyPatterns()
{
	RegisterObjectTypeDefinition("EP_", [this](chaos::TiledMap::ObjectTypeDefinition const * def)
	{
		DoRegisterEnemyPattern(def);
	});
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

EnemyPattern * LudumGame::FindEnemyPattern(char const * name)
{
	for (auto const & t : enemy_patterns)
		if (chaos::StringTools::Stricmp(t->name, name) == 0)
			return t.get();
	return nullptr;
}

EnemyPattern const * LudumGame::FindEnemyPattern(char const * name) const
{
	for (auto const & t : enemy_patterns)
		if (chaos::StringTools::Stricmp(t->name, name) == 0)
			return t.get();
	return nullptr;
}


void LudumGame::DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params)
{
	death::Game::DoDisplayGame(renderer, uniform_provider, render_params);

	LudumPlayer * ludum_player = GetLudumPlayer(0);

	if (ludum_player != nullptr && ludum_player->brightsideoflife)
	{
		chaos::GPUResourceManager * resource_manager = chaos::MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager == nullptr)
			return;
		chaos::GPURenderMaterial * postprocess_material = resource_manager->FindRenderMaterial("brightsideoflife");
		if (postprocess_material == nullptr)
			return;

		glEnable(GL_BLEND);
		renderer->DrawFullscreenQuad(postprocess_material, uniform_provider, render_params);	
		glDisable(GL_BLEND);
	}
}




















