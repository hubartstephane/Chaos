#include "Ludum42OneGame.h"
#include "Ludum42OneParticles.h"
#include "Ludum42OneLevel.h"
#include "Ludum42OneGameInstance.h"

#include <chaos/JSONTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/InputMode.h>
#include <chaos/GeometryFramework.h>
#include <chaos/CollisionFramework.h>

#include <death/GameParticles.h>

LudumGame::LudumGame()
{		
	game_name = "Escape Paouf 3";
}

bool LudumGame::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	if (!death::Game::OnPhysicalGamepadInput(physical_gamepad))
		return false;

	return true;
}

bool LudumGame::OnGamepadInput(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
{
	if (death::Game::OnGamepadInput(in_physical_gamepad))
		return true;
	return false;
}

void LudumGame::DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
{
	// clear the color buffers
	glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

	// clear the depth buffers
	float far_plane = 1000.0f;
	glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

	// some states
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// draw particle system
	if (particle_manager != nullptr)
		particle_manager->Display(renderer, uniform_provider, render_params);

	// super method
	death::Game::DoDisplay(renderer, uniform_provider, render_params);
}

void LudumGame::OnInputModeChanged(int new_mode, int old_mode)
{

}

void LudumGame::ResetGameVariables()
{
	death::Game::ResetGameVariables();
	current_life  = initial_life;
	current_cooldown  = cooldown;
}

void LudumGame::ChangeLife(int delta_life)
{
	if (delta_life == 0)
		return;
	current_life = chaos::MathTools::Maximum(current_life + delta_life, 0);
}

bool LudumGame::CheckGameOverCondition()
{
	if (current_life <= 0)
	{
		RequireGameOver();
		return true;
	}
	return false;
}

bool LudumGame::TickGameLoop(double delta_time)
{
	// super call
	if (!death::Game::TickGameLoop(delta_time))
		return false;
	// displace the player
	DisplacePlayer(delta_time);
	// cooldown
	TickCooldown(delta_time);

	return true;
}

chaos::ParticleAllocation * LudumGame::CreatePlayer()
{
	// create the object
	chaos::ParticleAllocation * result = GetGameParticleCreator().CreateParticles("player", 1, death::GameHUDKeys::GAMEOBJECT_LAYER_ID);
	if (result == nullptr)
		return nullptr;

	// set the color
	chaos::ParticleAccessor<ParticleObject> particles = result->GetParticleAccessor<ParticleObject>();
	if (particles.GetCount() == 0)
		return nullptr;

	particles->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	particles->bounding_box.position  = glm::vec2(0.0f, 0.0f);
	particles->bounding_box.half_size = glm::vec2(0.0f, 0.0f);

	return result;
}

chaos::SM::StateMachine * LudumGame::DoCreateGameStateMachine()
{
	return new LudumStateMachine(this);
}

bool LudumGame::DeclareParticleClasses()
{
	if (!Game::DeclareParticleClasses())
		return false;
	chaos::ClassTools::DeclareClass<ParticleObject>("ParticleObject");
	chaos::ClassTools::DeclareClass<ParticleObjectAtlas, ParticleObject>("ParticleObjectAtlas");
	chaos::ClassTools::DeclareClass<ParticlePlayer, ParticleObjectAtlas>("ParticlePlayer");
	return true;
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!death::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;
	DEATHGAME_JSON_ATTRIBUTE(initial_life);
	DEATHGAME_JSON_ATTRIBUTE(cooldown);
	return true;
}

death::TiledMap::Level * LudumGame::CreateTiledMapLevel()
{
	return new LudumLevel();
}

int LudumGame::AddParticleLayers()
{
	int render_order = death::Game::AddParticleLayers();
	if (render_order < 0)
		return render_order;

	particle_manager->AddLayer<ParticleObjectTrait>(render_order++, death::GameHUDKeys::GAMEOBJECT_LAYER_ID, "gameobject");
	particle_manager->AddLayer<ParticleObjectTrait>(render_order++, death::GameHUDKeys::PLANETS_LAYER_ID, "gameobject");

	ParticlePlayerTrait player_trait;
	player_trait.game = this;
	particle_manager->AddLayer<ParticlePlayerTrait>(render_order++, death::GameHUDKeys::PLAYER_LAYER_ID, "gameobject", player_trait);

	particle_manager->AddLayer<ParticleObjectTrait>(render_order++, death::GameHUDKeys::TEXT_LAYER_ID, "text");

	return render_order;
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeFromConfiguration(config, config_path))
		return false;

	return true;
}

void LudumGame::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance, death::GameLevelInstance * old_level_instance)
{
	death::Game::OnLevelChanged(new_level, old_level, new_level_instance, old_level_instance);

}

static int GetCircleSectionFromDirection(glm::vec2 const direction, int section_count)
{
	float angle = atan2(direction.y, direction.x);
	if (angle < 0.0f)
		angle += 2.0f * (float)M_PI;

	float section = 2.0f * (float)M_PI / (float)section_count;

	return (int)((angle + section / 2.0f) / section);
}

static glm::vec2 GetDirectionFromCircleSection(int quadran, int section_count)
{
	float section = 2.0f * (float)M_PI / (float)section_count;
	float angle   = ((float)quadran) * section;

	return glm::vec2(cos(angle), sin(-angle));
}

void LudumGame::DisplacePlayer(double delta_time)
{
	glm::vec2 value;

	value.x = (abs(right_stick_position.x) > abs(left_stick_position.x))?
		right_stick_position.x:
		left_stick_position.x;

	value.y = (abs(right_stick_position.y) > abs(left_stick_position.y))?
		-right_stick_position.y:
		-left_stick_position.y;

	glm::vec2 position = GetPlayerPosition(0);
	SetPlayerPosition(0, position + value * (float)delta_time);
	RestrictPlayerToWorld(0);
}

void LudumGame::TickCooldown(double delta_time)
{
	current_cooldown = chaos::MathTools::Maximum(0.0f, current_cooldown - (float)delta_time);
}

death::GameInstance * LudumGame::CreateGameInstance()
{
	return new LudumGameInstance(this);
}
