#include "Ludum41CustomGame.h"
#include "Ludum41CustomLevel.h"
#include "Ludum41CustomLevelInstance.h"
#include "Ludum41CustomParticles.h"
#include "Ludum41CustomHUD.h"
#include "Ludum41CustomGameInstance.h"
#include "Ludum41CustomPlayer.h"


#include <chaos/Chaos.h>

#include <death/GameParticles.h>


LudumGame::LudumGame()
{
	game_name = "AsciiPaouf\nJuju and Barbara Edition";
	game_instructions = R"INSTRUCTIONS(
	Enter : Pause (You should have a look)
	F10   : Toggle Fullscreen)INSTRUCTIONS";
	looping_levels = false;

	game_instance_class = LudumGameInstance::GetStaticClass();
}

death::GameHUD * LudumGame::DoCreatePlayingHUD()
{
	return new LudumPlayingHUD();
}

void LudumGame::OnEnterMainMenu(bool very_first)
{
	death::Game::OnEnterMainMenu(very_first);

	chaos::MathTools::ResetRandSeed();
	if (very_first)
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void LudumGame::OnInputModeChanged(chaos::InputMode new_mode, chaos::InputMode old_mode)
{
	LudumGameInstance * ludum_game_instance = GetGameInstance();
	if (ludum_game_instance != nullptr)
		ludum_game_instance->OnInputModeChanged(new_mode, old_mode);
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!death::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;

	DEATHGAME_JSON_ATTRIBUTE(max_ball_count);
	DEATHGAME_JSON_ATTRIBUTE(ball_size);
	DEATHGAME_JSON_ATTRIBUTE(player_max_length);
	DEATHGAME_JSON_ATTRIBUTE(player_min_length);
	DEATHGAME_JSON_ATTRIBUTE(player_initial_length);
	DEATHGAME_JSON_ATTRIBUTE(player_length_increment);
	DEATHGAME_JSON_ATTRIBUTE(player_length_decrement);
	DEATHGAME_JSON_ATTRIBUTE(ball_max_speed);
	DEATHGAME_JSON_ATTRIBUTE(ball_initial_speed);
	DEATHGAME_JSON_ATTRIBUTE(ball_speed_increment);
	DEATHGAME_JSON_ATTRIBUTE(challenge_time_dilation);
	DEATHGAME_JSON_ATTRIBUTE(challenge_frequency);
	DEATHGAME_JSON_ATTRIBUTE(challenge_duration);
	DEATHGAME_JSON_ATTRIBUTE(delay_before_ball_move);
	DEATHGAME_JSON_ATTRIBUTE(min_brick_life);
	DEATHGAME_JSON_ATTRIBUTE(max_brick_life);
	DEATHGAME_JSON_ATTRIBUTE(brick_per_line);
	DEATHGAME_JSON_ATTRIBUTE(combo_for_reward);
	DEATHGAME_JSON_ATTRIBUTE(ball_collision_speed_increment);
	DEATHGAME_JSON_ATTRIBUTE(ball_collision_max_speed);
	DEATHGAME_JSON_ATTRIBUTE(points_per_brick);
	DEATHGAME_JSON_ATTRIBUTE(points_per_challenge);
	DEATHGAME_JSON_ATTRIBUTE(split_angle);
	DEATHGAME_JSON_ATTRIBUTE(ball_angle_min);
	DEATHGAME_JSON_ATTRIBUTE(ball_angle_max);
	DEATHGAME_JSON_ATTRIBUTE(rebound_angle_decrease);
	DEATHGAME_JSON_ATTRIBUTE(rebound_angle_increase);
	DEATHGAME_JSON_ATTRIBUTE(max_brick_offset);
	DEATHGAME_JSON_ATTRIBUTE(brick_offset_speed);
	DEATHGAME_JSON_ATTRIBUTE(brick_offset_increment);

	DEATHGAME_JSON_ATTRIBUTE(background_fillratio_changespeed);
	DEATHGAME_JSON_ATTRIBUTE(delay_before_next_level);

	return true;
}

death::Level * LudumGame::DoLoadLevel(chaos::FilePathParam const & path)
{
	nlohmann::json level_content;
	if (!chaos::JSONTools::LoadJSONFile(path, level_content, false))
		return nullptr;

	std::vector<std::string> lines;
	if (!chaos::JSONTools::GetAttribute(level_content, "LINES", lines))
		return nullptr;

	LudumLevel * result = new LudumLevel;
	if (result == nullptr)
		return nullptr;

	// get the bricks
	std::vector<int> brick_line;
	for (size_t i = 0; i < lines.size(); ++i)
	{
		brick_line.resize(0);

		char const * l = lines[i].c_str();
		for (size_t j = 0; l[j] != 0; ++j)
		{

			if (j >= brick_per_line)
				break;


			char c = l[j];

			// special
			if (std::toupper(c) == 'T')
			{
				brick_line.push_back(LudumLevel::TWO_LIFE);
				result->destructible_brick_count++;
				continue;
			}
			if (std::toupper(c) == 'F')
			{
				brick_line.push_back(LudumLevel::FOUR_LIFE);
				result->destructible_brick_count++;
				continue;
			}
			if (std::toupper(c) == 'I')
			{
				brick_line.push_back(LudumLevel::INDESTRUCTIBLE);
				result->indestructible_brick_count++;
				continue;
			}
			// separator
			if (!std::isdigit(c))
			{
				brick_line.push_back(LudumLevel::NONE);
				continue;
			}
			// life
			int brick_type = (int)(c - '0');
			brick_line.push_back(brick_type);
			result->destructible_brick_count++;
		}
		result->bricks.push_back(std::move(brick_line));
	}

	// get the music
	chaos::JSONTools::GetAttribute(level_content, "MUSIC", result->music);
	// get the background
	chaos::JSONTools::GetAttribute(level_content, "BACKGROUND_MATERIAL", result->background_material);
	chaos::JSONTools::GetAttribute(level_content, "BACKGROUND_TEXTURE", result->background_texture);
	// get the "dictionnary"
	std::vector<std::string> dictionnary;
	if (chaos::JSONTools::GetAttribute(level_content, "DICTIONNARY", dictionnary))
	{
		for (std::string const & dictionnary_line : dictionnary)
		{
			//std::vector<std::string> words = chaos::StringTools::Split(dictionnary_line.c_str(), ' ');
			//for (std::string const & word : words)
				//result->dictionnary.push_back(word);		

			result->dictionnary.push_back(dictionnary_line.c_str());
		}	
	}
	return result;
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

	for (size_t i = 0; i < len; ++i)
		if (strchr(valid_letters, word[i]) == nullptr)
			return false;
	return true;
}

void LudumGame::ReplaceSpecialLetters(std::string & word) const
{
	size_t len = word.length();

	for (size_t i = 0; i < len; ++i)
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
	WORD   subg_language = SUBLANGID(language);

	bool french_dictionnary = (primary_language == LANG_FRENCH);

	// read the min and max words
	chaos::JSONTools::GetAttribute(config, "min_word_size", min_word_size, 1);
	chaos::JSONTools::GetAttribute(config, "max_word_size", max_word_size, 10);
	if (min_word_size > max_word_size)
		std::swap(min_word_size, max_word_size);

	// read the dictionnary file
	std::string dictionnary_path;
#if _DEBUG
	if (!chaos::JSONTools::GetAttribute(config, (french_dictionnary) ? "small_french_dictionnary" : "small_english_dictionnary", dictionnary_path))
		return false;
#else
	if (!chaos::JSONTools::GetAttribute(config, (french_dictionnary) ? "french_dictionnary" : "english_dictionnary", dictionnary_path))
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
		if (len >(size_t)max_word_size)
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
			auto x = dictionnary.insert(std::make_pair(len, std::vector<std::string>()));
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

	gamepad_buttons.push_back(chaos::XBoxButton::BUTTON_A);
	gamepad_buttons.push_back(chaos::XBoxButton::BUTTON_B);
	gamepad_buttons.push_back(chaos::XBoxButton::BUTTON_X);
	gamepad_buttons.push_back(chaos::XBoxButton::BUTTON_Y);

	return true;
}

bool LudumGame::InitializeRewardsAndPunishments()
{
	rewards.push_back(new LudumChallengeRewardPunishment_BrickOffset);
	rewards.push_back(new LudumChallengeRewardPunishment_BallPower);
	rewards.push_back(new LudumChallengeRewardPunishment_BarSize);
	rewards.push_back(new LudumChallengeRewardPunishment_SpeedDownBall);
	rewards.push_back(new LudumChallengeRewardPunishment_SplitBall);
	rewards.push_back(new LudumChallengeRewardPunishment_ExtraLife);
#if 0
	rewards.push_back(new LudumChallengeRewardPunishment_BrickLife); // too strong
#endif

	punishments.push_back(new LudumChallengeRewardPunishment_BrickOffset);
	punishments.push_back(new LudumChallengeRewardPunishment_BallPower);
	punishments.push_back(new LudumChallengeRewardPunishment_BarSize);
	punishments.push_back(new LudumChallengeRewardPunishment_SpeedDownBall);
	
#if 0
	punishments.push_back(new LudumChallengeRewardPunishment_ExtraLife); // too hard
	punishments.push_back(new LudumChallengeRewardPunishment_BrickLife); // too weak
#endif 
	return true;
}


int LudumGame::AddParticleLayers()
{
	int render_order = death::Game::AddParticleLayers();
	if (render_order < 0)
		return render_order;

	// create layers
	particle_manager->AddLayer<ParticleObjectLayerTrait>(render_order++, death::GameHUDKeys::BACKGROUND_GAMEOBJECT_LAYER_ID, "gameobject");
	particle_manager->AddLayer<ParticleObjectLayerTrait>(render_order++, death::GameHUDKeys::GAMEOBJECT_LAYER_ID, "gameobject");

	ParticleMovableObjectLayerTrait movable_trait;
	movable_trait.game = this;
	particle_manager->AddLayer<ParticleMovableObjectLayerTrait>(render_order++, death::GameHUDKeys::BALL_LAYER_ID, "gameobject", movable_trait);

	ParticleBrickLayerTrait brick_trait;
	brick_trait.game = this;
	particle_manager->AddLayer<ParticleBrickLayerTrait>(render_order++, death::GameHUDKeys::BRICK_LAYER_ID, "gameobject", brick_trait);

	particle_manager->AddLayer<ParticleChallengeLayerTrait>(render_order++, death::GameHUDKeys::CHALLENGE_LAYER_ID, "challenge");
	particle_manager->AddLayer<ParticleObjectLayerTrait>(render_order++, death::GameHUDKeys::TEXT_LAYER_ID, "text");

	return render_order;
}

