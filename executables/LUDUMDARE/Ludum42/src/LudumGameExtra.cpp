#include "LudumGame.h"
#include "LudumParticles.h"

#include <chaos/JSONTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/InputMode.h>
#include <chaos/ParticleTextGenerator.h>
#include <chaos/StringTools.h>


bool LudumGame::CreateAllMusics()
{
	if (menu_music == nullptr)
		menu_music = PlaySound("menu_music", true, true);
	if (pause_music == nullptr)
		pause_music = PlaySound("pause_music", true, true);
	if (game_music == nullptr)
		game_music = PlaySound("game_music", true, true);
	return true;
}



void LudumGame::StartMainMenuMusic(bool restart_first)
{
	chaos::Sound * musics[] = {
		menu_music.get(),
		pause_music.get(),
		game_music.get()
	};
	ChangeMusic(musics, 3, restart_first);
}

void LudumGame::StartGameMusic(bool restart_first)
{
	chaos::Sound * musics[] = {
		game_music.get(),
		pause_music.get(),
		menu_music.get()
	};
	ChangeMusic(musics, 3, restart_first);
}

void LudumGame::StartPauseMusic(bool restart_first)
{
	chaos::Sound * musics[] = {
		pause_music.get(),
		menu_music.get(),
		game_music.get()
	};
	ChangeMusic(musics, 3, restart_first);
}


bool LudumGame::IsPauseEnterComplete()
{
	if (pause_music == nullptr)
		return true;
	return !pause_music->HasVolumeBlending();
}

bool LudumGame::IsPauseLeaveComplete()
{
	if (game_music == nullptr)
		return true;
	return !game_music->HasVolumeBlending();
}

bool LudumGame::IsGameEnterComplete()
{
	if (game_music == nullptr)
		return true;
	return !game_music->HasVolumeBlending();
}

bool LudumGame::IsGameLeaveComplete()
{
	if (menu_music == nullptr)
		return true;
	return !menu_music->HasVolumeBlending();
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

int LudumGame::GetCurrentStateID() const
{
	if (game_automata == nullptr)
		return -1;

	chaos::StateMachine::State const * current_state = game_automata->GetCurrentState();
	if (current_state == nullptr)
		return -1;

	return current_state->GetStateID();
}


bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
#define LUDUMGAME_JSON_ATTRIBUTE(x) chaos::JSONTools::GetAttribute(config, #x, x)
	LUDUMGAME_JSON_ATTRIBUTE(initial_life);
	LUDUMGAME_JSON_ATTRIBUTE(mouse_sensitivity);
	LUDUMGAME_JSON_ATTRIBUTE(gamepad_sensitivity);
	LUDUMGAME_JSON_ATTRIBUTE(heart_beat_speed);
#undef LUDUMGAME_JSON_ATTRIBUTE

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


