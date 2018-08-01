#include "Ludum42Game.h"
#include "Ludum42Particles.h"

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

LudumGame::~LudumGame()
{		
	SerializeBestScore(true);
}

bool LudumGame::LoadBestScore(std::ifstream & file)
{
	file >> best_score;
	return true;
}

bool LudumGame::SaveBestScore(std::ofstream & file)
{
	file << best_score;
	return true;
}


void LudumGame::IncrementScore(int delta)
{
	if (delta <= 0)
		return;
	current_score += delta * current_combo_multiplier;
	should_update_score = true;
}

void LudumGame::CreateGameTitle()
{
	CreateTitle("AsciiPaouf 3", false);

	if (best_score > 0)
	{
		chaos::ParticleTextGenerator::GeneratorParams params;
		params.line_height = 50;
		params.hotpoint_type = chaos::Hotpoint::CENTER;
		params.position.x = 0.0f;
		params.position.y = -130.0f;

		params.character_set_name = "normal";

		std::string str = chaos::StringTools::Printf("Best score : %d", best_score);
		best_score_allocations = CreateTextParticles(str.c_str(), params);
	}
}

chaos::ParticleAllocation * LudumGame::CreateScoringParticles(bool & update_flag, char const * format, int value, float Y)
{
	// test flag
	if (!update_flag)
		return nullptr;
	update_flag = false;

	// get world size
	chaos::box2 world = GetWorldBox();

	std::pair<glm::vec2, glm::vec2> corners = world.GetCorners();

	// set the values
	chaos::ParticleTextGenerator::GeneratorParams params;
	params.line_height = 30;
	params.hotpoint_type = chaos::Hotpoint::TOP_LEFT;
	params.position.x = corners.first.x + 20.0f;
	params.position.y = corners.second.y - Y;
	params.character_set_name = "normal";

	// format text and create particles
	std::string str = chaos::StringTools::Printf(format, value);
	return CreateTextParticles(str.c_str(), params);	
}

void LudumGame::UpdateLifeParticles()
{

#if 0
	// get the number of particles already existing
	size_t life_particles = 0;
	if (life_allocations != nullptr)
		life_particles = life_allocations->GetParticleCount();

	// no changes ?
	if (life_particles == current_life)
		return;

	// some life lost (destroy particles)
	if ((size_t)current_life < life_particles)
	{
		life_allocations->Resize(current_life);
		return;
	}

	// some life gained
	assert(life_particles < (size_t)current_life);
	
	//if (life_allocations == nullptr)
		
	life_allocations = CreateGameObjects("life", current_life, LIFE_LAYER_ID);
	if (life_allocations == nullptr)
		return;

	// set the color
	chaos::ParticleAccessor<ParticleObject> particles = life_allocations->GetParticleAccessor<ParticleObject>();
	if (particles.GetCount() == 0)
		return;

	glm::vec2 world_size = GetWorldSize();

	glm::vec2 particle_size;
	particle_size.x = 35.0f;
	particle_size.y = 20.0f;

	for (size_t i = 0; i < (size_t)current_life; ++i)
	{
		glm::vec2 position;
		position.x = -world_size.x * 0.5f + 20.0f + (particle_size.x + 5.0f) * (float)i;
		position.y = -world_size.y * 0.5f + 15.0f;

		particles[i].bounding_box.position = chaos::Hotpoint::Convert(position, particle_size, chaos::Hotpoint::BOTTOM_LEFT, chaos::Hotpoint::CENTER);
		particles[i].bounding_box.half_size = 0.5f * particle_size;

		particles[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
#endif
}


void LudumGame::UpdateScoreParticles()
{
	chaos::ParticleAllocation * allocation = CreateScoringParticles(should_update_score, "Score : %d", current_score, 20.0f);
	if (allocation != nullptr)
		score_allocations = allocation;
}

void LudumGame::CreateTitle(char const * title, bool normal)
{
	chaos::ParticleTextGenerator::GeneratorParams params;
	params.line_height = TITLE_SIZE;
	params.hotpoint_type = chaos::Hotpoint::CENTER;
	params.position.y = TITLE_PLACEMENT_Y;

	params.character_set_name = (normal) ? "normal" : "title";

	text_allocations = CreateTextParticles(title, params);
}

void LudumGame::DestroyTitle()
{
	text_allocations = nullptr;
	best_score_allocations = nullptr;
}

bool LudumGame::OnAbordGame()
{

	return true;
}

void LudumGame::OnEnterMainMenu(bool very_first)
{
	if (very_first)
	{
		chaos::MathTools::ResetRandSeed();

		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		StartMainMenuMusic(true);
		CreateGameTitle();
	}
}

bool LudumGame::OnEnterPause()
{
	StartPauseMusic(true);
	CreateTitle("Pause", true);
	return true;
}

bool LudumGame::OnLeavePause()
{
	StartGameMusic(false);
	DestroyTitle();
	return true;
}

bool LudumGame::OnEnterGame()
{
	StartGameMusic(true);
	DestroyTitle();
	ResetGameVariables();
	CreateAllGameObjects(0);
	return true;
}

bool LudumGame::OnLeaveGame(bool gameover)
{
	StartMainMenuMusic(true);
	return true;
}

bool LudumGame::OnCharEvent(unsigned int c)
{

	return true;
}

bool LudumGame::OnKeyEvent(int key, int action)
{
	// MAIN MENU to PLAYING
	if (action == GLFW_PRESS)
		if (RequireStartGame())
			return true;

	// PLAYING to PAUSE
	if ((key == GLFW_KEY_KP_ENTER || key == GLFW_KEY_ENTER) && action == GLFW_PRESS)
		if (RequireTogglePause())
			return true;

	// QUIT GAME
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		if (RequireExitGame())
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
	// ignore invalid gamepad : should never happen
	if (!physical_gamepad->IsAnyAction())
		return true;

	// change the application mode
	chaos::Application::SetApplicationInputMode(chaos::InputMode::Gamepad);

	// cache the stick position
	glm::vec2 lsp = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_LEFT_AXIS);
	if (glm::length2(lsp) > 0.0f)
		left_stick_position = gamepad_sensitivity * lsp;
	else
	{
		if (physical_gamepad->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_LEFT))
			left_stick_position.x = -gamepad_sensitivity * 1.0f;
		else if (physical_gamepad->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_RIGHT))
			left_stick_position.x =  gamepad_sensitivity * 1.0f;
	}

	glm::vec2 rsp = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);
	if (glm::length2(rsp) > 0.0f)
		right_stick_position = gamepad_sensitivity * rsp;

	// maybe a start game
	if (physical_gamepad->IsAnyButtonPressed())
		if (game_automata->main_menu_to_playing->TriggerTransition(true))
			return true;

	// maybe a game/pause resume
	if (
		(physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_SELECT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED) ||
		(physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_START) == chaos::MyGLFW::BUTTON_BECOME_PRESSED))
	{
		if (RequireTogglePause())
			return true;
	}

	return true;
}

glm::vec2 LudumGame::GetWorldSize() const
{
	glm::vec2 result;
	result.x = 1600.0f;
	result.y = result.x / viewport_wanted_aspect;
	return result;
}

chaos::box2 LudumGame::GetWorldBox() const
{
	chaos::box2 result;
	result.position  = glm::vec2(0.0f, 0.0f);
	result.half_size = GetWorldSize() * 0.5f;
	return result;
}

void LudumGame::Display(glm::ivec2 const & size)
{
	chaos::box2 viewport = chaos::GLTools::SetViewportWithAspect(size, viewport_wanted_aspect);

	// clear the color buffers
	glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

#if 0
	int state_id = GetCurrentStateID();
	if (state_id == LudumAutomata::STATE_MAINMENU)
		clear_color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	else if (state_id == LudumAutomata::STATE_PAUSE)
		clear_color = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	else if (state_id == LudumAutomata::STATE_PLAYING)
		clear_color = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
#endif
	glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

	// clear the depth buffers
	float far_plane = 1000.0f;
	glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

	// some states
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// a variable provider
	chaos::GPUProgramProvider main_uniform_provider;
	main_uniform_provider.AddVariableValue("viewport_size", viewport.half_size * 2.0f);

	glm::vec2 world_size;
	world_size.x = 1600.0f;
	world_size.y = world_size.x * (viewport.half_size.y / viewport.half_size.x);
	main_uniform_provider.AddVariableValue("world_size", world_size);

	// draw particle system
	if (particle_manager != nullptr)
		particle_manager->Display(&main_uniform_provider);
}

void LudumGame::OnInputModeChanged(int new_mode, int old_mode)
{

}

void LudumGame::ResetGameVariables()
{
	current_life  = initial_life;
	heart_warning = 1.0f;

	current_score = 0;
	current_combo_multiplier  = 1;
	current_level = 0;

	should_update_score = true;
	should_update_combo = true;
}

void LudumGame::OnGameOver()
{
	if (best_score < current_score)
	{
		best_score = current_score;
		SerializeBestScore(true);
	}
	DestroyGameObjects();
}

void LudumGame::DisplacePlayer(double delta_time)
{
	float value = left_stick_position.x;
	if (abs(right_stick_position.x) > abs(left_stick_position.x))
		value = right_stick_position.x;

	glm::vec2 position = GetPlayerPosition();
	SetPlayerPosition(position.x + value);
}







void LudumGame::ChangeLife(int delta_life)
{
	if (delta_life == 0)
		return;
	current_life = chaos::MathTools::Maximum(current_life + delta_life, 0);
}

bool LudumGame::TickGameOverDetection(double delta_time)
{
	if (current_life <= 0)
	{
		RequireGameOver();
		return false;
	}
	return true;
}


LudumLevel * LudumGame::GetCurrentLevel()
{
	return GetLevel(current_level);
}

LudumLevel const * LudumGame::GetCurrentLevel() const
{
	return GetLevel(current_level);
}

LudumLevel * LudumGame::GetLevel(int level_number)
{
	if (levels.size() == 0)
		return nullptr;
	return levels[level_number % (int)levels.size()].get();
}

LudumLevel const * LudumGame::GetLevel(int level_number) const
{
	if (levels.size() == 0)
		return nullptr;
	return levels[level_number % (int)levels.size()].get();
}


bool LudumGame::IsLevelCompleted()
{

	return false;
}

void LudumGame::TickLevelCompleted(double delta_time)
{
	LudumLevel const * level = GetCurrentLevel();
	if (level == nullptr)
		return;

#if _DEBUG
	bool completed = cheat_next_level || IsLevelCompleted();
#else
	bool completed = IsLevelCompleted();
#endif

	if (completed)
	{



	}
}

void LudumGame::TickHeartWarning(double delta_time)
{
	if (current_life == 1)
	{
		heart_warning -= heart_beat_speed * (float)delta_time;
		if (heart_warning <= 0.0f)
		{
			PlaySound("heartbeat", false, false);

			float fractionnal_part, integer_part;
			fractionnal_part = modf(heart_warning, &integer_part);

			heart_warning = (1.0f + fractionnal_part);
		}
	}
	else
		heart_warning = 1.0f;
}


void LudumGame::TickGameLoop(double delta_time)
{
	DisplacePlayer(delta_time);

	if (TickGameOverDetection(delta_time))
	{
		// create the score text
		UpdateScoreParticles();
		// create the life 
		UpdateLifeParticles();
		// some other calls
		TickLevelCompleted(delta_time);
		TickHeartWarning(delta_time);
	}
}


void LudumGame::OnMouseMove(double x, double y)
{
	left_stick_position.x = mouse_sensitivity * (float)x;
}

void LudumGame::OnMouseButton(int button, int action, int modifier)
{

	chaos::StateMachine::State const * state = game_automata->GetCurrentState();
	if (state != nullptr)
	{
#if 0
		if (state->GetStateID() == LudumAutomata::STATE_PLAYING)
		{
			if (button == 0 && action == GLFW_PRESS)
				sequence_challenge = CreateSequenceChallenge(0);
		}
		else 
#endif			
		if (state->GetStateID() == LudumAutomata::STATE_MAINMENU)
		{
			if (action == GLFW_PRESS)
				RequireStartGame();
		}		
	}
}

void LudumGame::DestroyGameObjects()
{
	player_allocations = nullptr;
	life_allocations = nullptr;
	score_allocations = nullptr;
	combo_allocations = nullptr;
	best_score_allocations = nullptr;
}

chaos::ParticleAllocation * LudumGame::CreateGameObjects(char const * name, size_t count, int layer_id)
{
	// find layer of concern
	chaos::ParticleLayer * layer = particle_manager->FindLayer(layer_id);
	if (layer == nullptr)
		return nullptr;

	// find bitmap set
	chaos::BitmapAtlas::BitmapSet const * bitmap_set = texture_atlas->GetBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return nullptr;

	// find bitmap entry
	chaos::BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(name);
	if (entry == nullptr)
		return nullptr;

	// allocate the objects
	chaos::ParticleAllocation * allocation = layer->SpawnParticles(count);
	if (allocation == nullptr)
		return nullptr;

	chaos::ParticleAccessor<ParticleObject> particles = allocation->GetParticleAccessor<ParticleObject>();

	for (size_t i = 0 ; i < count ; ++i)
	{
		ParticleObject & particle = particles[i];
		particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*entry, texture_atlas->GetAtlasDimension());
	}
		
	return allocation;
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
	return glm::vec2(0.0f, 0.0f);
}

void LudumGame::SetPlayerPosition(float position)
{

	RestrictedPlayerToScreen();
}


void LudumGame::RestrictedObjectToScreen(chaos::ParticleAllocation * allocation, size_t index)
{
#if 0
	chaos::box2 box = particle->bounding_box;
	chaos::box2 world = GetWorldBox();
	chaos::RestrictToInside(world, box, false);
#endif




}

void LudumGame::RestrictedPlayerToScreen()
{
	RestrictedObjectToScreen(player_allocations.get(), 0);
}



void LudumGame::CreateAllGameObjects(int level)
{
	if (player_allocations == nullptr)
	{
		player_allocations = CreatePlayer();
		SetPlayerPosition(0.0f);
	}
}

bool LudumGame::FillAtlasGenerationInputWithTileSets(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	return chaos::TiledMapTools::GenerateAtlasInput(tiledmap_manager.get(), input, "sprites");
}

bool LudumGame::FillAtlasGenerationInput(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::FillAtlasGenerationInput(input, config, config_path))
		return false;
	if (!FillAtlasGenerationInputWithTileSets(input, config, config_path))
		return false;
	return true;
}

bool LudumGame::CreateGameAutomata()
{
	game_automata = new LudumAutomata(this);
	if (game_automata == nullptr)
		return false;
	return true;
}

bool LudumGame::DeclareParticleClasses()
{

	return true;
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeGameValues(config, config_path))
		return false;
	DEATHGAME_JSON_ATTRIBUTE(initial_life);
	DEATHGAME_JSON_ATTRIBUTE(heart_beat_speed);
	return true;
}

bool LudumGame::DoLoadLevelInitialize(LudumLevel * level)
{



	return true;
}


bool LudumGame::DoLoadLevel(int level_number, chaos::TiledMap::Map * tiled_map)
{
	// allocate a level
	LudumLevel * level = new LudumLevel;
	if (level == nullptr)
		return false;
	// initialize level
	level->level_number = level_number;
	level->tiled_map = tiled_map;
	// some additionnal computation
	if (!DoLoadLevelInitialize(level))
	{
		delete(level);
		return false;
	}
	levels.push_back(level);
	return true;
}

bool LudumGame::LoadLevels()
{
	chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
	if (application == nullptr)
		return false;

	// create the manager
	tiledmap_manager = new chaos::TiledMap::Manager;
	if (tiledmap_manager == nullptr)
		return false;


	// compute resource path
	boost::filesystem::path resources_path = application->GetResourcesPath();
	boost::filesystem::path levels_path = resources_path / "levels";

	// iterate the files and load the levels
	boost::filesystem::directory_iterator end;
	for (boost::filesystem::directory_iterator it(levels_path); it != end; ++it)
	{
		int level_number = chaos::StringTools::SkipAndAtoi(it->path().filename().string().c_str());

		chaos::TiledMap::Map * tiled_map = tiledmap_manager->LoadMap(it->path());
		if (tiled_map == nullptr)
			continue;

		if (!DoLoadLevel(level_number, tiled_map))
		{
			delete(tiled_map);
			return false;
		}
	}

	// sort the levels
	std::sort(levels.begin(), levels.end(),
		[](boost::intrusive_ptr<LudumLevel> l1, boost::intrusive_ptr<LudumLevel> l2)
	{
		return (l1->level_number < l2->level_number);
	});
	return true;
}

void LudumGame::FillBackgroundLayer()
{
	chaos::ParticleLayer * layer = particle_manager->FindLayer(BACKGROUND_LAYER_ID);
	if (layer == nullptr)
		return;

	background_allocations = layer->SpawnParticles(1);
	if (background_allocations == nullptr)
		return;

	chaos::ParticleAccessor<ParticleBackground> particles = background_allocations->GetParticleAccessor<ParticleBackground>();
	if (particles.GetCount() == 0)
		return;

	particles->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}


chaos::ParticleAllocation * LudumGame::CreateTextParticles(char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params)
{
	// find layer of concern
	chaos::ParticleLayer * layer = particle_manager->FindLayer(TEXT_LAYER_ID);
	if (layer == nullptr)
		return nullptr;

	// generate the tokens
	chaos::ParticleTextGenerator::GeneratorResult result;

	particle_text_generator->Generate(text, result, params);

	chaos::ParticleAllocation * allocation = chaos::ParticleTextGenerator::CreateTextAllocation(layer, result);

	return allocation;
}


bool LudumGame::InitializeParticleManager()
{
	if (!death::Game::InitializeParticleManager())
		return false;

	int render_order = 0;

	particle_manager->AddLayer<ParticleBackgroundTrait>(++render_order, BACKGROUND_LAYER_ID, "background");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, GAMEOBJECT_LAYER_ID, "gameobject");
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



