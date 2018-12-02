#include "Ludum43Game.h"
#include "Ludum43Particles.h"
#include "Ludum43Level.h"
#include "Ludum43HUD.h"

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

#include <death/GameParticles.h>

LudumGame::LudumGame()
{		
	game_name = "Quantikaouf IV";
	camera_safe_zone = glm::vec2(0.2f, 0.2f);
}

void LudumGame::OnEnterMainMenu(bool very_first)
{
	death::Game::OnEnterMainMenu(very_first);


}

bool LudumGame::OnEnterPause()
{
	death::Game::OnEnterPause();
	return true;
}

bool LudumGame::OnLeavePause()
{
	death::Game::OnLeavePause();
	return true;
}

bool LudumGame::OnEnterGame()
{
	death::Game::OnEnterGame();
	CreateAllGameObjects(0);
	return true;
}

bool LudumGame::OnLeaveGame(bool gameover)
{
	death::Game::OnLeaveGame(gameover);
	return true;
}

bool LudumGame::OnAbordGame()
{
	death::Game::OnAbordGame();
	DestroyGameObjects();
	return true;
}

bool LudumGame::OnCharEvent(unsigned int c)
{

	return true;
}

bool LudumGame::OnKeyEvent(int key, int action)
{
	if (death::Game::OnKeyEvent(key, action))
		return true;
	return false;
}

bool LudumGame::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	if (!death::Game::OnPhysicalGamepadInput(physical_gamepad))
		return false;

	return true;
}

bool LudumGame::OnGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data)
{
	if (death::Game::OnGamepadInput(in_gamepad_data))
		return true;
	return false;
}

void LudumGame::DoDisplay(chaos::RenderParams const & render_params, chaos::GPUProgramProvider & uniform_provider)
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
		particle_manager->Display(&uniform_provider);

	// super method
	death::Game::DoDisplay(render_params, uniform_provider);
}

void LudumGame::OnInputModeChanged(int new_mode, int old_mode)
{

}

void LudumGame::ResetGameVariables()
{
	death::Game::ResetGameVariables();
	current_life  = initial_life;
	current_cooldown  = cooldown;

	current_dash_cooldown = 0.0f;

	waken_up_particle_count = 0;
	saved_particle_count = 0;
	heart_beat_time = 0.0f;
	level_time = 0.0f;

}

void LudumGame::OnGameOver()
{
	death::Game::OnGameOver();
	DestroyGameObjects();
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
	UpdatePlayerAcceleration(delta_time);
	// dash values update
	TickDashValues(delta_time);
	// cooldown
	TickCooldown(delta_time);
	// tick sound for heart beat
	TickHeartBeat(delta_time);
	// update some internal
	level_time += (float)delta_time;

	return true;
}

void LudumGame::TickHeartBeat(double delta_time) 
{
	float limit_value = 2.0f;
	if (waken_up_particle_count < 1)
		limit_value = 0.5f;
	else if (waken_up_particle_count < 2)
		limit_value = 0.75f;
	else if (waken_up_particle_count < 3)
		limit_value = 1.0f;
	else if (waken_up_particle_count < 5)
		limit_value = 1.5f;


	heart_beat_time += (float)delta_time;
	if (heart_beat_time >= limit_value)
	{
		heart_beat_time = 0.0f;
		PlaySound("heartbeat", false, false);	
	}
}

void LudumGame::OnMouseMove(double x, double y)
{

}

void LudumGame::DestroyGameObjects()
{
	//player_allocations = nullptr;
	//life_allocations = nullptr;
}

void LudumGame::RestrictObjectToWorld(chaos::ParticleAllocation * allocation, size_t index)
{
	chaos::box2 box    = GetObjectBox(allocation, index);
	chaos::box2 world = GetWorldBox();
	chaos::RestrictToInside(world, box, false);
	SetObjectBox(allocation, index, box);
}

void LudumGame::RestrictPlayerToWorld()
{
	RestrictObjectToWorld(player_allocations.get(), 0);
}



void LudumGame::CreateAllGameObjects(int level)
{

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

	chaos::ClassTools::DeclareClass<ParticlePlayer, ParticleBase>("ParticlePlayer");
	chaos::ClassTools::DeclareClass<ParticleEnemy, ParticleBase>("ParticleEnemy");
	chaos::ClassTools::DeclareClass<ParticleAtom, ParticleBase>("ParticleAtom");
	return true;
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeGameValues(config, config_path))
		return false;
	DEATHGAME_JSON_ATTRIBUTE(dash_cooldown);
	DEATHGAME_JSON_ATTRIBUTE(dash_velocity);
	DEATHGAME_JSON_ATTRIBUTE(initial_life);
	DEATHGAME_JSON_ATTRIBUTE(cooldown);

	DEATHGAME_JSON_ATTRIBUTE(player_attraction_minradius);
	DEATHGAME_JSON_ATTRIBUTE(player_attraction_maxradius);
	DEATHGAME_JSON_ATTRIBUTE(player_tangent_force);		
	DEATHGAME_JSON_ATTRIBUTE(player_attraction_force);	
	DEATHGAME_JSON_ATTRIBUTE(player_repulsion_force);	
	DEATHGAME_JSON_ATTRIBUTE(player_slowing_factor);		
	DEATHGAME_JSON_ATTRIBUTE(player_max_velocity);
	DEATHGAME_JSON_ATTRIBUTE(player_acceleration);

	DEATHGAME_JSON_ATTRIBUTE(worldlimits_attraction_maxradius_offset);
	DEATHGAME_JSON_ATTRIBUTE(worldlimits_attraction_minradius_offset);
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_minradius_offset);
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_maxradius_offset);

	DEATHGAME_JSON_ATTRIBUTE(worldlimits_attraction_maxradius_factor);
	DEATHGAME_JSON_ATTRIBUTE(worldlimits_attraction_minradius_factor);
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_minradius_factor);
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_maxradius_factor);

	DEATHGAME_JSON_ATTRIBUTE(enemy_tangent_force);		
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_force);	
	DEATHGAME_JSON_ATTRIBUTE(enemy_repulsion_force);	

	DEATHGAME_JSON_ATTRIBUTE(particle_slowing_factor);		
	DEATHGAME_JSON_ATTRIBUTE(particle_radius_rand_factor);		
	DEATHGAME_JSON_ATTRIBUTE(particle_max_velocity);

	DEATHGAME_JSON_ATTRIBUTE(world_clamp_radius);

	return true;
}

death::TiledMap::Level * LudumGame::CreateTiledMapLevel()
{
	return new LudumLevel();
}

int LudumGame::AddParticleLayers()
{
	return death::Game::AddParticleLayers();
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

void LudumGame::UpdatePlayerAcceleration(double delta_time)
{
	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return;
	player_particle->acceleration = glm::vec2(0.0f, 0.0f);

	float left_length_2  = glm::length2(left_stick_position);
	float right_length_2 = glm::length2(right_stick_position);
	if (left_length_2 > 0.0f || right_length_2 > 0.0f)
	{
		glm::vec2 acceleration = (left_length_2 > right_length_2)?
			left_stick_position / chaos::MathTools::Sqrt(left_length_2):
			right_stick_position / chaos::MathTools::Sqrt(right_length_2);

		player_particle->acceleration = player_acceleration * glm::vec2(1.0f, -1.0f) * acceleration; // axis Y reversed
	}
}

void LudumGame::TickCooldown(double delta_time)
{
	current_cooldown = chaos::MathTools::Maximum(0.0f, current_cooldown - (float)delta_time);
}

void LudumGame::HandleGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data)
{
	death::Game::HandleGamepadInput(in_gamepad_data);

	if (in_gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_RIGHTTRIGGER, false))
		ConditionnalStartDash();

	if (chaos::Application::GetApplicationInputMode() == chaos::InputMode::Gamepad)
	{
		bool reversed_mode = in_gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_A, false);
		SetPlayerReverseMode(reversed_mode);
	}
}

void LudumGame::HandleKeyboardInputs()
{
	death::Game::HandleKeyboardInputs();
	if (glfwGetKey(glfw_window, GLFW_KEY_SPACE) == GLFW_PRESS) // no repetition for dash
		ConditionnalStartDash();

	if (chaos::Application::GetApplicationInputMode() == chaos::InputMode::Keyboard)
	{
		bool reversed_mode = (glfwGetKey(glfw_window, GLFW_KEY_RIGHT_CONTROL) != GLFW_RELEASE);
		SetPlayerReverseMode(reversed_mode);
	}
}

void LudumGame::SetPlayerReverseMode(bool reversed_mode)
{
	if (level_time < 1.0f) // because the player start could cause a repulsion
		return;

	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle != nullptr)
		player_particle->reversed = reversed_mode;
}

void LudumGame::ConditionnalStartDash()
{
	// dashing or cooling down
	if (current_dash_cooldown > 0.0f)
		return;

	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return;

	glm::vec2 & velocity = player_particle->velocity;

	float velocity_2 = glm::length2(velocity);
	if (velocity_2 == 0.0f)
		return;

	velocity += (velocity / chaos::MathTools::Sqrt(velocity_2)) * dash_velocity;

	current_dash_cooldown = 0.0f; //dash_cooldown;
}

void LudumGame::TickDashValues(double delta_time)
{
	// cooldow in progress
	if (current_dash_cooldown > 0.0f)
		current_dash_cooldown = chaos::MathTools::Maximum(0.0f, current_dash_cooldown - (float)delta_time);
}


void LudumGame::RegisterEnemiesInRange(glm::vec2 const & center, float radius, std::vector<ParticleEnemy> & enemy_particles, char const * layer_name, bool take_all)
{
	// capture all Enemies in range
	LudumLevelInstance const * level_instance = dynamic_cast<LudumLevelInstance const *>(GetCurrentLevelInstance());
	if (level_instance != nullptr)
	{
		death::TiledMap::LayerInstance const * layer_instance = level_instance->FindLayerInstance(layer_name);
		if (layer_instance)
		{
			chaos::ParticleLayer const * particle_layer = layer_instance->GetParticleLayer();
			if (particle_layer != nullptr)
			{
				float square_radius = radius * radius;

				size_t count = particle_layer->GetAllocationCount();
				for (size_t i = 0 ; i < count ; ++i)
				{
					chaos::ParticleAllocation const * allocation = particle_layer->GetAllocation(i);
					if (allocation == nullptr)
						continue;

					chaos::ParticleConstAccessor<ParticleEnemy> enemies = allocation->GetParticleAccessor<ParticleEnemy>();

					size_t enemies_count = enemies.GetCount();
					for (size_t j = 0 ; j < enemies_count ; ++j)
					{
						ParticleEnemy const & enemy = enemies[j];
						if (!take_all)
						{
							float l2 = glm::length2(enemy.bounding_box.position - center);
							if (l2 > square_radius)
								continue;						
						}
						enemy_particles.push_back(enemy);									
					}				
				}
			}		
		}	
	}
}

void LudumGame::NotifyAtomCountChange(int delta)
{
	waken_up_particle_count += delta;
}

float LudumGame::GetPlayerLife() const
{
	ParticlePlayer const * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return 0.0f;
	return player_particle->life;
}

ParticlePlayer * LudumGame::GetPlayerParticle()
{
	if (player_allocations == nullptr)
		return nullptr;
	chaos::ParticleAccessor<ParticlePlayer> player_particles = player_allocations->GetParticleAccessor<ParticlePlayer>();
	if (player_particles.GetCount() == 0)
		return nullptr;
	return &player_particles[0];
}

ParticlePlayer const * LudumGame::GetPlayerParticle() const
{
	if (player_allocations == nullptr)
		return nullptr;
	chaos::ParticleConstAccessor<ParticlePlayer> player_particles = player_allocations->GetParticleAccessor<ParticlePlayer>();
	if (player_particles.GetCount() == 0)
		return nullptr;
	return &player_particles[0];
}

chaos::box2 LudumGame::GetWorldBox() const
{
	if (current_level_instance != nullptr)
		 return current_level_instance->GetBoundingBox();
	return chaos::box2();
}