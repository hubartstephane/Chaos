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



bool LudumGamepadManager::DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	if (game != nullptr)
		return game->OnPhysicalGamepadInput(physical_gamepad);
	return true;
}

chaos::MyGLFW::SingleWindowApplication * LudumGame::GetApplication()
{
	return chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
}

chaos::SoundManager * LudumGame::GetSoundManager()
{
	chaos::MyGLFW::SingleWindowApplication * application = GetApplication();
	if (application == nullptr)
		return nullptr;
	return application->GetSoundManager();
}

void LudumGame::BlendMusic(chaos::Sound * music, bool blend_in)
{
	if (music == nullptr)
		return;

	chaos::BlendVolumeDesc blend_desc;
	blend_desc.blend_type   = (blend_in)? 
		chaos::BlendVolumeDesc::BLEND_IN: 
		chaos::BlendVolumeDesc::BLEND_OUT;
	blend_desc.blend_time   = 0.4f,
	blend_desc.kill_at_end  = false;
	blend_desc.pause_at_end = !blend_in;

	music->StartBlend(blend_desc, true, true);
}

chaos::Sound * LudumGame::PlaySound(char const * name, bool paused, bool looping)
{
	chaos::SoundManager * sound_manager = GetSoundManager();
	if (sound_manager == nullptr)
		return nullptr;

	chaos::SoundSource * source = sound_manager->FindSource(name);
	if (source == nullptr)
		return nullptr;

	chaos::PlaySoundDesc play_desc;
	play_desc.paused  = paused;
	play_desc.looping = looping;
	return source->PlaySound(play_desc);
}

void LudumGame::CreateAllMusics()
{
	if (menu_music == nullptr)
		menu_music = PlaySound("menu_music", true, true);
	if (pause_music == nullptr)
		pause_music = PlaySound("pause_music", true, true);
	if (game_music == nullptr)
		game_music = PlaySound("game_music", true, true);
}

void LudumGame::ChangeMusic(chaos::Sound ** musics, size_t count, bool restart_first)
{
	if (musics == nullptr || count == 0)
		return;

	// restore the main music
	chaos::Sound * music1 = musics[0];
	if (music1 != nullptr)
	{
		music1->Pause(false);		
		if (restart_first)
			music1->SetSoundTrackPosition(0);
		BlendMusic(music1, true);
	}

	// pause all other musics
	for (size_t i = 1; i < count; ++i)
	{
		chaos::Sound * music = musics[i];
		if (music == nullptr)
			continue;
		BlendMusic(music, false);
	}
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

bool LudumGame::GenerateAtlas(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	chaos::BitmapAtlas::AtlasInput input;

	// get the directory where the sprites are
	std::string sprite_directory;
	chaos::JSONTools::GetAttribute(config, "sprite_directory", sprite_directory);

	// get the path of the font
	std::string font_path;
	chaos::JSONTools::GetAttribute(config, "font_path", font_path);

	std::string title_font_path;
	chaos::JSONTools::GetAttribute(config, "title_font_path", title_font_path);

	// Add sprites
	chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return false;

	bitmap_set->AddBitmapFilesFromDirectory(sprite_directory);

	// Add the font
	chaos::BitmapAtlas::CharacterSetInputParams font_params;
	font_params.max_character_width = 64;
	font_params.max_character_height = 64;

	chaos::BitmapAtlas::CharacterSetInput * character_set1 =
		input.AddCharacterSet("normal", nullptr, font_path.c_str(), nullptr, true, font_params);

	chaos::BitmapAtlas::CharacterSetInput * character_set2 =
		input.AddCharacterSet("title", nullptr, title_font_path.c_str(), nullptr, true, font_params);
	// generate the atlas
	int ATLAS_SIZE = 1024;
	int ATLAS_PADDING = 10;
	chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());

#if _DEBUG
	params.debug_dump_atlas_dirname = "LudumAtlas";
#endif

	chaos::BitmapAtlas::TextureArrayAtlasGenerator generator;
	texture_atlas = generator.ComputeResult(input, params);
	if (texture_atlas == nullptr)
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

bool LudumGame::InitializeGame(GLFWwindow * in_glfw_window)
{
	// initialize the window
	assert(in_glfw_window != nullptr);
	glfw_window = in_glfw_window;

	// create the game automata
	game_automata = new LudumAutomata(this);
	if (game_automata == nullptr)
		return false;

	// initialize the gamepad manager
	gamepad_manager = new LudumGamepadManager(this);
	if (gamepad_manager == nullptr)
		return false;

	// create the musics
	CreateAllMusics();

	// initialize particle classes
	DeclareParticleClasses();

	return true;
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


bool LudumGame::DoLoadLevel(int level_number, std::vector<std::string> & level_content)
{


	return true;
}

bool LudumGame::LoadLevels()
{
	chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
	if (application == nullptr)
		return false;

	// compute resource path
	boost::filesystem::path resources_path = application->GetResourcesPath();
	boost::filesystem::path levels_path = resources_path / "levels";

	// iterate the files and load the levels
	boost::filesystem::directory_iterator end;
	for (boost::filesystem::directory_iterator it(levels_path); it != end; ++it)
	{
		std::vector<std::string> level_content = chaos::FileTools::ReadFileLines(it->path());

		int level_number = chaos::StringTools::SkipAndAtoi(it->path().filename().string().c_str());
		if (!DoLoadLevel(level_number, level_content))
			return false;
	}

	// sort the levels
	std::sort(levels.begin(), levels.end(),
		[](boost::intrusive_ptr<LudumLevel> l1, boost::intrusive_ptr<LudumLevel> l2)
	{
		return (l1->level_number < l2->level_number);
	});
	return true;
}


bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	// the atlas
	if (!GenerateAtlas(config, config_path))
		return false;
	// initialize the particle manager
	if (!InitializeParticleManager())
		return false;
	// initialize the particle text generator manager
	if (!InitializeParticleTextGenerator())
		return false;
	// initialize game values
	if (!InitializeGameValues(config, config_path))
		return false;
	// load exisiting levels
	if (!LoadLevels())
		return false;
	// load the best score if any
	SerializeBestScore(false);
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

bool LudumGame::InitializeParticleTextGenerator()
{
	// create the generator
	particle_text_generator = new chaos::ParticleTextGenerator::Generator(*texture_atlas);
	if (particle_text_generator == nullptr)
		return false;

	// for each bitmap, that correspond to a button, register a [NAME] in the generator	
	chaos::BitmapAtlas::BitmapSet const * bitmap_set = texture_atlas->GetBitmapSet("sprites");


	return true;
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
	// create the manager
	particle_manager = new chaos::ParticleManager();
	if (particle_manager == nullptr)
		return false;
	particle_manager->SetTextureAtlas(texture_atlas.get());

	int render_order = 0;

	particle_manager->AddLayer<ParticleBackgroundTrait>(++render_order, BACKGROUND_LAYER_ID, "background");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, GAMEOBJECT_LAYER_ID, "gameobject");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, TEXT_LAYER_ID, "text");

	// fill the background
	FillBackgroundLayer();

	return true;
}


