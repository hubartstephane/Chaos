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
#include <chaos/ClassTools.h>

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

int LudumGame::GetCurrentStateID() const
{
	if (game_automata == nullptr)
		return -1;

	chaos::StateMachine::State const * current_state = game_automata->GetCurrentState();
	if (current_state == nullptr)
		return -1;

	return current_state->GetStateID();
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
	chaos::ClassTools::DeclareClass<ParticleObject>("ParticleObject");
	chaos::ClassTools::DeclareClass<ParticleBackground>("ParticleBackground");
	chaos::ClassTools::DeclareClass<ParticleBrick, ParticleObject>("ParticleBrick");
	chaos::ClassTools::DeclareClass<ParticleMovableObject, ParticleObject>("ParticleMovableObject");
	chaos::ClassTools::DeclareClass<ParticleChallenge, ParticleObject>("ParticleChallenge");

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
	LUDUMGAME_JSON_ATTRIBUTE(challenge_frequency);	
	LUDUMGAME_JSON_ATTRIBUTE(challenge_duration);
	LUDUMGAME_JSON_ATTRIBUTE(delay_before_ball_move);		
	LUDUMGAME_JSON_ATTRIBUTE(heart_beat_speed);
	LUDUMGAME_JSON_ATTRIBUTE(min_brick_life);		
	LUDUMGAME_JSON_ATTRIBUTE(max_brick_life);		
	LUDUMGAME_JSON_ATTRIBUTE(brick_per_line);		
	LUDUMGAME_JSON_ATTRIBUTE(brick_line_count);		
	LUDUMGAME_JSON_ATTRIBUTE(ball_collision_speed_increment);		
	LUDUMGAME_JSON_ATTRIBUTE(ball_collision_max_speed);		
	LUDUMGAME_JSON_ATTRIBUTE(points_per_brick);		
	LUDUMGAME_JSON_ATTRIBUTE(points_per_challenge);
	LUDUMGAME_JSON_ATTRIBUTE(split_angle);
	LUDUMGAME_JSON_ATTRIBUTE(ball_angle_limit);
	
	LUDUMGAME_JSON_ATTRIBUTE(max_brick_offset);
	LUDUMGAME_JSON_ATTRIBUTE(brick_offset_speed);
	LUDUMGAME_JSON_ATTRIBUTE(brick_offset_increment);

#undef LUDUMGAME_JSON_ATTRIBUTE

	return true;
}


bool LudumGame::DoLoadLevel(int level_number, std::vector<std::string> & level_content)
{
	LudumLevel * level = new LudumLevel;
	if (level == nullptr)
		return false;

	level->level_number = level_number;

	std::vector<int> line;
	for (size_t i = 0; i < level_content.size(); ++i)
	{
		line.resize(0);

		char const * l = level_content[i].c_str();
		for (size_t j = 0; l[j] != 0; ++j)
		{
			char c = l[j];

			// indestructible
			if (c == 'B')
			{
				line.push_back(LudumLevel::INDESTRUCTIBLE);
				level->indestructible_brick_count++;
				continue;
			}
			// separator
			if (c < '0' || c > '9')
			{
				line.push_back(LudumLevel::NONE);
				continue;
			}
			// life
			int brick_type = (int)(c - '0');
			line.push_back(brick_type);
		}
		level->bricks.push_back(std::move(line));
	}

	levels.push_back(level);

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
	if (!death::Game::InitializeFromConfiguration(config, config_path))
		return false;
	// the dictionnary
	if (!InitializeDictionnary(config, config_path))
		return false;
	// build the rewards/punishment values
	if (!InitializeRewardsAndPunishments())
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
#if _DEBUG
	if (!chaos::JSONTools::GetAttribute(config, (french_dictionnary)? "small_french_dictionnary" : "small_english_dictionnary", dictionnary_path))
		return false;
#else
	if (!chaos::JSONTools::GetAttribute(config, (french_dictionnary)? "french_dictionnary" : "english_dictionnary", dictionnary_path))
		return false;
#endif

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
	if (!death::Game::InitializeGamepadButtonInfo())
		return false;

	gamepad_buttons.push_back(chaos::MyGLFW::XBOX_BUTTON_A);
	gamepad_buttons.push_back(chaos::MyGLFW::XBOX_BUTTON_B);
	gamepad_buttons.push_back(chaos::MyGLFW::XBOX_BUTTON_X);
	gamepad_buttons.push_back(chaos::MyGLFW::XBOX_BUTTON_Y);

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

bool LudumGame::InitializeRewardsAndPunishments()
{
#if 1
	rewards.push_back(new LudumChallengeRewardPunishment_BrickOffset);
	rewards.push_back(new LudumChallengeRewardPunishment_BallPower);
	rewards.push_back(new LudumChallengeRewardPunishment_BarSize); 
	rewards.push_back(new LudumChallengeRewardPunishment_SpeedDownBall);
	rewards.push_back(new LudumChallengeRewardPunishment_SplitBall);
	rewards.push_back(new LudumChallengeRewardPunishment_BrickLife);
	rewards.push_back(new LudumChallengeRewardPunishment_ExtraLife);	
#endif

#if 1
	punishments.push_back(new LudumChallengeRewardPunishment_BrickOffset);
	punishments.push_back(new LudumChallengeRewardPunishment_BallPower);
	punishments.push_back(new LudumChallengeRewardPunishment_BarSize);	
	punishments.push_back(new LudumChallengeRewardPunishment_SpeedDownBall);
	punishments.push_back(new LudumChallengeRewardPunishment_BrickLife);
	punishments.push_back(new LudumChallengeRewardPunishment_ExtraLife);
#endif
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
		result = result + "[" + it->second.second + "]";	
	}
	return result;
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


chaos::ParticleAllocation * LudumGame::CreateChallengeParticles(LudumChallenge * challenge)
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

	// create the text
	chaos::ParticleAllocation * allocation = chaos::ParticleTextGenerator::CreateTextAllocation(layer, result);
	// and initialize additionnal data
	if (allocation != nullptr)
	{
		chaos::ParticleAccessor<ParticleChallenge> particles = allocation->GetParticleAccessor<ParticleChallenge>();
		for (size_t i = 0; i < particles.GetCount(); ++i)
		{
			ParticleChallenge & p = particles[i];
			p.challenge = challenge;
			p.index = i;
		}
	}

	return allocation;
}

bool LudumGame::InitializeParticleManager()
{
	if (!death::Game::InitializeParticleManager())
		return false;

	int render_order = 0;

	// create layers
	particle_manager->AddLayer<ParticleBackgroundTrait>(++render_order, BACKGROUND_LAYER_ID, "background");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, BACKGROUND_GAMEOBJECT_LAYER_ID, "gameobject");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, GAMEOBJECT_LAYER_ID, "gameobject");

	ParticleMovableObjectTrait movable_trait;
	movable_trait.game = this;
	particle_manager->AddLayer<ParticleMovableObjectTrait>(++render_order, BALL_LAYER_ID, "gameobject", movable_trait);

	ParticleBrickTrait brick_trait;
	brick_trait.game = this;
	particle_manager->AddLayer<ParticleBrickTrait>(++render_order, BRICK_LAYER_ID, "gameobject", brick_trait);

	ParticleLifeObjectTrait life_trait;
	life_trait.game = this;
	particle_manager->AddLayer<ParticleLifeObjectTrait>(++render_order, LIFE_LAYER_ID, "gameobject", life_trait);

	particle_manager->AddLayer<ParticleChallengeTrait>(++render_order, CHALLENGE_LAYER_ID, "challenge");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, TEXT_LAYER_ID, "text");

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


LudumChallenge * LudumGame::CreateSequenceChallenge(size_t len) 
{
	if (len == 0)
		len = min_word_size + rand() % (max_word_size - min_word_size);

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
	LudumChallenge * result = new LudumChallenge;
	if (result != nullptr)
	{	
		result->gamepad_challenge  = std::move(gamepad_challenge);
		result->keyboard_challenge = std::move(keyboard_challenge);
		result->game = this;	
		result->particle_range = CreateChallengeParticles(result);
		result->Show(IsPlaying());

		result->SetTimeout(challenge_duration);

		ball_time_dilation = challenge_time_dilation;
	}
	return result;
}

