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
	blend_desc.blend_time   = 1.0f,
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

	// Add sprites
	chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return false;

	bitmap_set->AddBitmapFilesFromDirectory(sprite_directory);

	// Add the font
	chaos::BitmapAtlas::CharacterSetInput * character_set1 =
		input.AddCharacterSet("character_set", nullptr, font_path.c_str(), nullptr, true, chaos::BitmapAtlas::CharacterSetInputParams());

	// generate the atlas
	int ATLAS_SIZE = 1024;
	int ATLAS_PADDING = 10;
	chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());

	chaos::BitmapAtlas::Atlas          atlas;
	chaos::BitmapAtlas::AtlasGenerator generator;
	if (!generator.ComputeResult(input, atlas, params))
		return false;

	// generate texture Atlas
	texture_atlas = new chaos::BitmapAtlas::TextureArrayAtlas;
	if (texture_atlas == nullptr)
		return false;
	if (!texture_atlas->LoadFromBitmapAtlas(atlas))
		return false;

	// dump the atlas
#if _DEBUG
	chaos::Application * application = chaos::Application::GetInstance();
	atlas.SaveAtlas(application->GetUserLocalTempPath() / "LudumAtlas");
#endif

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

	return true;
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
#define LUDUMGAME_JSON_ATTRIBUTE(x) chaos::JSONTools::GetAttribute(config, #x, x)
	LUDUMGAME_JSON_ATTRIBUTE(initial_life);
	LUDUMGAME_JSON_ATTRIBUTE(max_life);
	LUDUMGAME_JSON_ATTRIBUTE(max_ball_count);
	LUDUMGAME_JSON_ATTRIBUTE(ball_size);	
	LUDUMGAME_JSON_ATTRIBUTE(player_max_length);
	LUDUMGAME_JSON_ATTRIBUTE(player_min_length);
	LUDUMGAME_JSON_ATTRIBUTE(player_initial_length);
	LUDUMGAME_JSON_ATTRIBUTE(player_length_increment);
	LUDUMGAME_JSON_ATTRIBUTE(player_length_decrement);	
	LUDUMGAME_JSON_ATTRIBUTE(ball_max_speed);
	LUDUMGAME_JSON_ATTRIBUTE(ball_initial_speed);
	LUDUMGAME_JSON_ATTRIBUTE(ball_speed_increment);
	LUDUMGAME_JSON_ATTRIBUTE(mouse_sensitivity);
	LUDUMGAME_JSON_ATTRIBUTE(gamepad_sensitivity);
	LUDUMGAME_JSON_ATTRIBUTE(challenge_time_dilation);	
	LUDUMGAME_JSON_ATTRIBUTE(delay_before_ball_move);		
#undef LUDUMGAME_JSON_ATTRIBUTE

	return true;
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	// the atlas
	if (!GenerateAtlas(config, config_path))
		return false;
	// the dictionnary
	if (!InitializeDictionnary(config, config_path))
		return false;
	// initialize the button map
	if (!InitializeGamepadButtonInfo())
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
	
	



	return true;
}

bool LudumGame::IsWordValid(std::string const & word) const 
{
	char const * valid_letters = "abcdefghijklmnopqrstuvwxyz";

	size_t len = word.length();

	for (size_t i = 0 ; i < len ; ++i)
		if (strchr(valid_letters, word[i]) == nullptr)
			return false;
	return true;
}

void LudumGame::ReplaceSpecialLetters(std::string & word) const 
{
	size_t len = word.length();

	for (size_t i = 0 ; i < len ; ++i)
	{
		char & c = word[i];
		if (c == 'é' || c == 'è' || c == 'ê')
			c = 'e';
		else if (c == 'à' || c == 'â')
			c = 'a';
		else if (c == 'î')
			c = 'i';	
		else if (c == 'ù')
			c = 'u';	
	}
}

bool LudumGame::InitializeDictionnary(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	// detect whether the language is french

#if 1
	LCID lcid = GetUserDefaultLCID();
	LANGID language = LANGIDFROMLCID(lcid);
#else
	LANGID language = GetUserDefaultUILanguage();
#endif
	WORD   primary_language = PRIMARYLANGID(language);
	WORD   subg_language    = SUBLANGID(language);

	bool french_dictionnary = (primary_language == LANG_FRENCH);

	// read the min and max words
	chaos::JSONTools::GetAttribute(config, "min_word_size", min_word_size, 1);
	chaos::JSONTools::GetAttribute(config, "max_word_size", max_word_size, 10);
	if (min_word_size > max_word_size)
		std::swap(min_word_size, max_word_size);

	// read the dictionnary file
	std::string dictionnary_path;
	if (!chaos::JSONTools::GetAttribute(config, (french_dictionnary)? "french_dictionnary" : "english_dictionnary", dictionnary_path))
		return false;

	std::vector<std::string> words = chaos::FileTools::ReadFileLines(dictionnary_path);
	if (words.size() < 100)
		return false;
	
	// transform the vector into a map (by word length)

	for (std::string & word : words)
	{
		// test word length
		size_t len = word.length();
		if (len < (size_t)min_word_size) 
			continue;
		if (len > (size_t)max_word_size) 
			continue;
		// replace accents (not all special characters handled)
		ReplaceSpecialLetters(word);
		// keep words with only simple characters 
		if (!IsWordValid(word))
			continue;
		// add the word into the map

		auto it = dictionnary.find(len);
		if (it == dictionnary.end())
		{
			auto x = dictionnary.insert(std::make_pair(len,std::vector<std::string>()));
			it = dictionnary.find(len);
		}
		it->second.push_back(std::move(word));	
	}

	if (dictionnary.size() == 0)
		return false;
		
	return true;
}

bool LudumGame::InitializeGamepadButtonInfo()
{
	// the map [button ID] => [bitmap name]
	gamepad_button_map[chaos::MyGLFW::XBOX_BUTTON_A] = "xboxControllerButtonA";
	gamepad_button_map[chaos::MyGLFW::XBOX_BUTTON_B] = "xboxControllerButtonB";
	gamepad_button_map[chaos::MyGLFW::XBOX_BUTTON_X] = "xboxControllerButtonX";
	gamepad_button_map[chaos::MyGLFW::XBOX_BUTTON_Y] = "xboxControllerButtonY";

#if 0
	gamepad_button_map[chaos::MyGLFW::XBOX_BUTTON_LEFTBUT]  = "xboxControllerLeftShoulder";
	gamepad_button_map[chaos::MyGLFW::XBOX_BUTTON_RIGHTBUT] = "xboxControllerRightShoulder";

	gamepad_button_map[chaos::MyGLFW::XBOX_BUTTON_LEFTTRIGGER]  = "xboxControllerLeftTrigger";
	gamepad_button_map[chaos::MyGLFW::XBOX_BUTTON_RIGHTTRIGGER] = "xboxControllerRightTrigger";
#endif
	// list of all buttons ID
	for (auto it = gamepad_button_map.begin() ; it != gamepad_button_map.end() ; ++it)
		gamepad_buttons.push_back(it->first);

	return true;
}

chaos::ParticleLayer * LudumGame::DoAddParticleLayer(chaos::ParticleLayer * layer, int render_order, int layer_id, char const * material_name)
{
	if (layer == nullptr)
		return nullptr;

	// give the layer to the manager
	particle_manager->AddLayer(layer);
	// change layer render order / ID
	layer->SetLayerID(layer_id);
	layer->SetRenderOrder(render_order);
	// set the material
	chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
	if (application != nullptr)
	{
		chaos::GPUResourceManager * manager = application->GetGPUResourceManager();
		if (manager != nullptr)
		{
			chaos::RenderMaterial * material = manager->FindRenderMaterial(material_name);
			layer->SetRenderMaterial(material);	
		}
	}
	return layer;
}

void LudumGame::FillBackgroundLayer()
{
	chaos::ParticleLayer * layer = particle_manager->FindLayer(BACKGROUND_LAYER_ID);
	if (layer == nullptr)
		return;

	chaos::ParticleRange range = layer->SpawnParticles(1);
	if (range.count == 0)
		return;
	
	ParticleBackground * particle = (ParticleBackground*)layer->GetParticleBuffer(range);
	if (particle == nullptr)
		return;

	particle->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

bool LudumGame::InitializeParticleTextGenerator()
{
	// create the generator
	particle_text_generator = new chaos::ParticleTextGenerator::Generator(*texture_atlas);
	if (particle_text_generator == nullptr)
		return false;

	// for each bitmap, that correspond to a button, register a [NAME] in the generator	
	chaos::BitmapAtlas::BitmapSet const * bitmap_set = texture_atlas->GetBitmapSet("sprites");
	if (bitmap_set != nullptr)
	{
		for (auto it = gamepad_button_map.begin() ; it != gamepad_button_map.end() ; ++it)
		{
			std::string const & bitmap_name = it->second;
			chaos::BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(bitmap_name.c_str());
			if (entry == nullptr)
				continue;
			particle_text_generator->AddBitmap(bitmap_name.c_str(), entry);	
		}		
	}

	return true;
}

std::string LudumGame::GenerateGamepadChallengeString(std::vector<int> const & gamepad_challenge)
{
	std::string result;

	for (size_t i = 0 ; i < gamepad_challenge.size() ; ++i)
	{
		int button_index = gamepad_challenge[i];
		
		auto const it = gamepad_button_map.find(button_index);
		if (it == gamepad_button_map.end())
			continue;
		result = result + "[" + it->second + "]";	
	}
	return result;
}

chaos::ParticleRangeAllocation * LudumGame::CreateTextParticles(char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params)
{
	// find layer of concern
	chaos::ParticleLayer * layer = particle_manager->FindLayer(TEXT_LAYER_ID);
	if (layer == nullptr)
		return nullptr;

	// generate the tokens
	chaos::ParticleTextGenerator::GeneratorResult result;

	particle_text_generator->Generate(text, result, params);

	// count the number of particle to draw
	size_t count = result.GetTokenCount();

	chaos::ParticleRangeAllocation * allocation = layer->SpawnParticlesAndKeepRange(count);
	if (allocation == nullptr)
		return nullptr;

	ParticleObject * particle = (ParticleObject *)allocation->GetParticleBuffer();
	if (particle == nullptr)
		return nullptr;

	size_t k = 0;
	for (size_t i = 0 ; i < result.token_lines.size() ; ++i)
	{
		chaos::ParticleTextGenerator::TokenLine const & line = result.token_lines[i];
		for (size_t j = 0 ; j < line.size() ; ++j)
		{
			chaos::ParticleTextGenerator::Token const & token = line[j];

			particle[k].corners   = token.corners;
			particle[k].texcoords = token.texcoords;
			particle[k].color     = glm::vec4(token.color.r, token.color.g, token.color.b, 1.0f);
			++k;
		}
	}
	return allocation;
}


chaos::ParticleRangeAllocation * LudumGame::CreateChallengeText(LudumSequenceChallenge * challenge)
{
	int  input_mode = chaos::MyGLFW::SingleWindowApplication::GetApplicationInputMode();
	bool keyboard   = chaos::InputMode::IsPCMode(input_mode);

	chaos::ParticleLayer * layer = particle_manager->FindLayer(CHALLENGE_LAYER_ID);
	if (layer == nullptr)
		return nullptr;

	chaos::ParticleTextGenerator::GeneratorResult result;
	chaos::ParticleTextGenerator::GeneratorParams params;

	params.line_height = CHALLENGE_SIZE;
	params.hotpoint_type = chaos::Hotpoint::TOP | chaos::Hotpoint::HMIDDLE;
	params.position.x = 0.0f;
	params.position.y = CHALLENGE_PLACEMENT_Y;

	if (keyboard)
	{
		particle_text_generator->Generate(challenge->keyboard_challenge.c_str(), result, params);
	}
	else
	{
		std::string gamepad_string = GenerateGamepadChallengeString(challenge->gamepad_challenge);
		particle_text_generator->Generate(gamepad_string.c_str(), result, params);	
	}

	// count the number of particle to draw
	size_t count = result.GetTokenCount();

	chaos::ParticleRangeAllocation * allocation = layer->SpawnParticlesAndKeepRange(count);
	if (allocation == nullptr)
		return nullptr;

	ParticleChallenge * particle = (ParticleChallenge *)allocation->GetParticleBuffer();
	if (particle == nullptr)
		return nullptr;

	size_t k = 0;
	for (size_t i = 0 ; i < result.token_lines.size() ; ++i)
	{
		chaos::ParticleTextGenerator::TokenLine const & line = result.token_lines[i];
		for (size_t j = 0 ; j < line.size() ; ++j)
		{
			chaos::ParticleTextGenerator::Token const & token = line[j];

			particle[k].corners   = token.corners;
			particle[k].texcoords = token.texcoords;
			particle[k].color     = glm::vec4(token.color.r, token.color.g, token.color.b, 1.0f);
			particle[k].challenge = challenge;
			particle[k].index     = k;
			++k;
		}
	}

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

	// create layers
	AddParticleLayer<ParticleBackgroundTrait>(++render_order, BACKGROUND_LAYER_ID, "background");
	AddParticleLayer<ParticleObjectTrait>(++render_order, BACKGROUND_GAMEOBJECT_LAYER_ID, "gameobject");
	AddParticleLayer<ParticleObjectTrait>(++render_order, GAMEOBJECT_LAYER_ID, "gameobject");

	ParticleMovableObjectTrait movable_trait;
	movable_trait.game = this;
	AddParticleLayer<ParticleMovableObjectTrait>(++render_order, BALL_LAYER_ID, "gameobject", movable_trait);

	AddParticleLayer<ParticleObjectTrait>(++render_order, TEXT_LAYER_ID, "text");
	AddParticleLayer<ParticleChallengeTrait>(++render_order, CHALLENGE_LAYER_ID, "challenge");

	// fill the background
	FillBackgroundLayer();

	return true;
}

int LudumGame::GetRandomButtonID() const
{
	size_t key_index = (size_t)(rand() % gamepad_buttons.size());
	if (key_index >= gamepad_buttons.size())
		key_index = gamepad_buttons.size() - 1;
	return gamepad_buttons[key_index];
}


LudumSequenceChallenge * LudumGame::CreateSequenceChallenge(size_t len) 
{
	auto it = dictionnary.find(len);

	// no word of this size (search a word with the lengh the more near the request) 
	if (it == dictionnary.end())
	{
		auto better_it = dictionnary.begin();

		int min_distance = std::numeric_limits<int>::max();
		for (it = dictionnary.begin() ; it != dictionnary.end() ; ++it)
		{
			int distance = abs((int)len - (int)it->first);
			if (distance < min_distance)
			{
				min_distance = distance;
				better_it = it;			
			}		
		}	
		if (it == dictionnary.end()) // should never happen
			return nullptr;
	}

	// get the list of words with given length
	std::vector<std::string> const & words = it->second;
	if (words.size() == 0)
		return nullptr;

	// search a word in the list
	size_t index = (size_t)(rand() % words.size());
	if (index >= words.size())
		index = words.size() - 1; // should never happen

	std::string keyboard_challenge = words[index];
	len = keyboard_challenge.size();

	// compose a gamepad combinaison of the same length
	std::vector<int> gamepad_challenge;
	for (size_t i = 0 ; i < len ; ++i)	
		gamepad_challenge.push_back(GetRandomButtonID());	

	// create the challenge
	LudumSequenceChallenge * result = new LudumSequenceChallenge;
	if (result != nullptr)
	{	
		result->gamepad_challenge  = std::move(gamepad_challenge);
		result->keyboard_challenge = std::move(keyboard_challenge);
		result->game = this;	
		result->particle_range = CreateChallengeText(result);


		result->callbacks = CreateSequenceChallengeCallbacks();
		result->SetTimeout(5.0f);

		ball_time_dilation = challenge_time_dilation;
	}
	return result;
}

LudumSequenceChallengeCallbacks * LudumGame::CreateSequenceChallengeCallbacks()
{
	int challenge = rand() % 4;

#if 0
	if (challenge == 0)
		return new LudumSequenceChallenge_LifeBallCallbacks();
	if (challenge == 1)	
		return new LudumSequenceChallenge_SpeedDownBallCallbacks();
	if (challenge == 2)	
		return new LudumSequenceChallenge_ExtraBallCallbacks();

	return new LudumSequenceChallenge_LongBarBallCallbacks();
#endif		
	return new LudumSequenceChallenge_SpeedDownBallCallbacks();
}
