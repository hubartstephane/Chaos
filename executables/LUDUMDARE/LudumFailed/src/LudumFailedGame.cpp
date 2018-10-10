#include "LudumFailedGame.h"
#include "LudumFailedParticles.h"
#include "LudumFailedLevel.h"

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



LudumGame::LudumGame()
{		
	game_name = "Escape Paouf 3";
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

	// FORCE GAMEOVER
#if _DEBUG
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
			cheat_next_level = true;
#endif

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
	// fire
	if (in_gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_A))
	{
		PlayerThrowWater();
	}
	return false;
}

void LudumGame::DoDisplay(chaos::box2 const & viewport, chaos::GPUProgramProvider & uniform_provider)
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

bool LudumGame::CheckGameOverCondition(double delta_time)
{
	if (current_life <= 0)
	{
		RequireGameOver();
		return true;
	}
	return false;
}

bool LudumGame::IsLevelCompleted()
{

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
	// test whether current level is terminated
	TickLevelCompleted(delta_time);



	return true;
}


void LudumGame::OnMouseMove(double x, double y)
{
	
}

void LudumGame::DestroyGameObjects()
{
	player_allocations = nullptr;
	water_allocations = nullptr;
	life_allocations = nullptr;
}

chaos::ParticleAllocation * LudumGame::CreateGameObjects(char const * name, size_t count, int layer_id)
{
	// allocate the objects
	chaos::ParticleAllocation * allocation = SpawnObjects(layer_id, count);
	if (allocation == nullptr)
		return nullptr;

	if (!InitializeGameObjects(allocation, name, count))
	{
		delete(allocation);
		return nullptr;
	}
	return allocation;
}

bool LudumGame::InitializeGameObjects(chaos::ParticleAllocation * allocation, char const * name, size_t count)
{
	// find bitmap set
	chaos::BitmapAtlas::FolderInfo const * bitmap_set = texture_atlas->GetFolderInfo("sprites");
	if (bitmap_set == nullptr)
		return false;

	// find bitmap info
	chaos::BitmapAtlas::BitmapInfo const * info = bitmap_set->GetBitmapInfo(name);
	if (info == nullptr)
		return false;

	chaos::ParticleAccessor<ParticleObject> particles = allocation->GetParticleAccessor<ParticleObject>();

	size_t particles_count = particles.GetCount();
	size_t start = 0;

	if (particles_count < count)
		count = particles_count; 
	else
		start = particles_count - count;

	for (size_t i = 0 ; i < count ; ++i)
	{
		ParticleObject & particle = particles[start + i];
		particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*info, texture_atlas->GetAtlasDimension());
	}
		
	return true;
}


chaos::ParticleAllocation * LudumGame::CreatePlayer()
{
	// create the object
	chaos::ParticleAllocation * result = CreateGameObjects("player", 1);
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








glm::vec2 LudumGame::GetPlayerPosition() const
{
	chaos::box2 b = GetPlayerBox();
	return b.position;
}

bool LudumGame::SetPlayerPosition(glm::vec2 const & position)
{
	chaos::box2 b = GetPlayerBox();
	b.position = position;
	if (SetPlayerBox(b))
	{
		RestrictPlayerToWorld();
		return true;
	}
	return false;
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
#if 0
	if (player_allocations == nullptr)
	{
		player_allocations = CreatePlayer();
		SetPlayerPosition(0.0f);
	}
#endif
}

bool LudumGame::FillAtlasGenerationInputWithTileSets(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	chaos::BitmapAtlas::FolderInfoInput * folder_input = input.AddFolder("sprites", 0);
	if (folder_input == nullptr)
		return false;
	return chaos::TiledMapTools::AddIntoAtlasInput(tiledmap_manager.get(), folder_input);
}

bool LudumGame::FillAtlasGenerationInput(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::FillAtlasGenerationInput(input, config, config_path))
		return false;
	if (!FillAtlasGenerationInputWithTileSets(input, config, config_path))
		return false;
	return true;
}

chaos::SM::StateMachine * LudumGame::DoCreateGameStateMachine()
{
	return new LudumStateMachine(this);
}

bool LudumGame::DeclareParticleClasses()
{
	chaos::ClassTools::DeclareClass<ParticleObject>("ParticleObject");
	chaos::ClassTools::DeclareClass<ParticleObjectAtlas, ParticleObject>("ParticleObjectAtlas");
	chaos::ClassTools::DeclareClass<ParticleWater, ParticleObject>("ParticleWater");
	chaos::ClassTools::DeclareClass<ParticlePlayer, ParticleObject>("ParticlePlayer");
	chaos::ClassTools::DeclareClass<ParticleBackground>("ParticleBackground");
	return true;
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeGameValues(config, config_path))
		return false;
	DEATHGAME_JSON_ATTRIBUTE(initial_life);
	DEATHGAME_JSON_ATTRIBUTE(cooldown);
	DEATHGAME_JSON_ATTRIBUTE(water_speed);
	DEATHGAME_JSON_ATTRIBUTE(water_lifetime);

	return true;
}

bool LudumGame::DoLoadLevelInitialize(LudumNarrativeLevel * level, nlohmann::json const & json_level)
{



	return true;
}

bool LudumGame::DoLoadLevelInitialize(LudumGameplayLevel * level, chaos::TiledMap::Map * tiled_map)
{
	// initialize level
	level->tiled_map = tiled_map;


	return true;
}


death::GameLevel * LudumGame::DoLoadLevel(int level_number, chaos::FilePathParam const & path)
{
	boost::filesystem::path const & resolved_path = path.GetResolvedPath();
#if 0
	if (chaos::FileTools::IsTypedFile(resolved_path, "json"))
	{
		nlohmann::json json_level;
		if (!chaos::JSONTools::LoadJSONFile(path, json_level, false))
			return nullptr;

		// allocate a level
		LudumNarrativeLevel * ludum_result = new LudumNarrativeLevel(this);
		if (ludum_result == nullptr)
			return false;
		// some additionnal computation
		if (!DoLoadLevelInitialize(ludum_result, json_level))
		{
			delete(ludum_result);
			return nullptr;
		}	
		return ludum_result;	
	}
	else 
#endif		
		if (chaos::FileTools::IsTypedFile(resolved_path, "tmx"))
	{
		// load the resource
		chaos::TiledMap::Map * tiled_map = tiledmap_manager->LoadMap(path);
		if (tiled_map == nullptr)
			return false;	
	
		// allocate a level
		LudumGameplayLevel * ludum_result = new LudumGameplayLevel(this);
		if (ludum_result == nullptr)
			return false;
		// some additionnal computation
		if (!DoLoadLevelInitialize(ludum_result, tiled_map))
		{
			delete(ludum_result);
			return nullptr;
		}	
		return ludum_result;
	}
	return nullptr;
}

bool LudumGame::LoadLevels()
{
	// create the manager
	tiledmap_manager = new chaos::TiledMap::Manager;
	if (tiledmap_manager == nullptr)
		return false;
	// super call
	return death::Game::LoadLevels();
}

void LudumGame::FillBackgroundLayer()
{
	background_allocations = SpawnObjects(BACKGROUND_LAYER_ID, 1);
	if (background_allocations == nullptr)
		return;

	chaos::ParticleAccessor<ParticleBackground> particles = background_allocations->GetParticleAccessor<ParticleBackground>();
	if (particles.GetCount() == 0)
		return;

	particles->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

bool LudumGame::InitializeParticleManager()
{
	if (!death::Game::InitializeParticleManager())
		return false;

	int render_order = 0;

	particle_manager->AddLayer<ParticleBackgroundTrait>(++render_order, BACKGROUND_LAYER_ID, "space_background");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, GROUND_LAYER_ID, "gameobject");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, WALLS_LAYER_ID, "gameobject");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, GAMEOBJECT_LAYER_ID, "gameobject");


	ParticlePlayerTrait player_trait;
	player_trait.game = this;
	player_trait.atlas_dimension = glm::ivec2(8, 8);
	particle_manager->AddLayer<ParticlePlayerTrait>(++render_order, PLAYER_LAYER_ID, "gameobject", player_trait);


	ParticleObjectAtlasTrait atlas_trait;
	atlas_trait.game = this;
	particle_manager->AddLayer<ParticleObjectAtlasTrait>(++render_order, FIRE_LAYER_ID, "gameobject", atlas_trait);

	ParticleWaterTrait water_trait;
	water_trait.game = this;
	water_trait.atlas_dimension = glm::ivec2(7, 1);
	particle_manager->AddLayer<ParticleWaterTrait>(++render_order, WATER_LAYER_ID, "gameobject", water_trait);

	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, TEXT_LAYER_ID, "text");

	// fill the background
	FillBackgroundLayer();

	return true;
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

chaos::ParticleAllocation * LudumGame::SpawnObjects(int layer_id, size_t count)
{
	chaos::ParticleLayer * layer = GetParticleManager()->FindLayer(layer_id);
	if (layer == nullptr)
		return nullptr;
	return layer->SpawnParticles(count);
}


bool LudumGame::SpawnPlayer(ParticleObject const & particle_object)
{
	// spawn the player
	if (player_allocations != nullptr) // already existing
		return false;

	player_allocations = SpawnObjects(LudumGame::PLAYER_LAYER_ID, 1);
	if (player_allocations == nullptr)
		return false;

	chaos::ParticleAccessor<ParticleObject> particles = player_allocations->GetParticleAccessor<ParticleObject>();
	particles[0] = particle_object;

	// create the water allocation
	water_allocations = SpawnObjects(LudumGame::WATER_LAYER_ID, 0);

	return true;
}

void LudumGame::UnSpawnPlayer()
{
	player_allocations = nullptr;
	water_allocations  = nullptr;
}


chaos::box2 LudumGame::GetPlayerBox() const
{
	return GetObjectBox(player_allocations.get(), 0);
}

bool LudumGame::SetPlayerBox(chaos::box2 const & in_player_box)
{

	return SetObjectBox(player_allocations.get(), 0, in_player_box);
}

chaos::box2 LudumGame::GetObjectBox(chaos::ParticleAllocation const * allocation, size_t index) const
{
	if (allocation == nullptr)
		return chaos::box2();

	chaos::ParticleConstAccessor<ParticleObject> particles = allocation->GetParticleConstAccessor<ParticleObject>();
	if (index >= particles.GetCount())
		return chaos::box2();

	return particles[index].bounding_box;
}


bool LudumGame::SetObjectBox(chaos::ParticleAllocation * allocation, size_t index, chaos::box2 const & b)
{
	if (allocation == nullptr)
		return false;

	chaos::ParticleAccessor<ParticleObject> particles = allocation->GetParticleAccessor<ParticleObject>();
	if (index >= particles.GetCount())
		return false;

	particles[index].bounding_box = b;
	return true;
}


void LudumGame::TickLevelCompleted(double delta_time)
{

#if _DEBUG
	if (cheat_next_level)
	{
		SetNextLevel(true);
		cheat_next_level = false;
		return;
	}
#endif

	LudumLevelInstance const * level_instance = dynamic_cast<LudumLevelInstance const *>(GetCurrentLevelInstance());
	if (level_instance == nullptr)
		return;


}

ParticlePlayer * LudumGame::GetPlayerParticle()
{
	if (player_allocations == nullptr)
		return nullptr;
	chaos::ParticleAccessor<ParticlePlayer> particles = player_allocations->GetParticleAccessor<ParticlePlayer>();
	if (particles.GetCount() == 0)
		return nullptr;
	return &particles[0];
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

	glm::vec2 position = GetPlayerPosition();
	SetPlayerPosition(position + value * (float)delta_time);


	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle != nullptr)
	{
		if (glm::length2(left_stick_position) > 0.0f)
		{
			player_particle->image_id.y = GetCircleSectionFromDirection(left_stick_position, 8);
		}
	}
}



void LudumGame::PlayerThrowWater()
{
	if (player_allocations == nullptr) // no player, nothing to do
		return;
	if (water_allocations == nullptr)
		return;

	if (current_cooldown > 0.0f)
		return;

	size_t new_count = water_allocations->GetParticleCount() + 1;
	water_allocations->Resize(new_count);

	InitializeGameObjects(water_allocations.get(), "water", 1);

	chaos::ParticleAccessor<ParticleWater> particles = water_allocations->GetParticleAccessor<ParticleWater>();
	if (particles.GetCount() < new_count)
		return;

	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return;

	ParticleWater & new_particle = particles[new_count - 1];
	new_particle.current_life = water_lifetime;
	new_particle.initial_life = water_lifetime;
	new_particle.bounding_box.position = player_particle->bounding_box.position;
	new_particle.bounding_box.half_size = player_particle->bounding_box.half_size;
	new_particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	new_particle.velocity = water_speed * GetDirectionFromCircleSection(player_particle->image_id.y, 8);
		
	current_cooldown = cooldown;
}

void LudumGame::TickCooldown(double delta_time)
{
	current_cooldown = chaos::MathTools::Maximum(0.0f, current_cooldown - (float)delta_time);
}
