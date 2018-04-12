#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/TextureArrayAtlas.h>

// =================================================
// LudumGame
// =================================================

class LudumGame : public chaos::ReferencedObject
{
	// the possible states
	static int const STATE_MAINMENU = 0;
	static int const STATE_PAUSE    = 1;
	static int const STATE_PLAYING  = 2;

	// the possible transition-states
	static int const STATE_TRANSITION_MAINMENU_TO_PLAYING = 3;
	static int const STATE_TRANSITION_PLAYING_TO_PAUSE    = 4;
	static int const STATE_TRANSITION_PAUSE_TO_PLAYING    = 5;

	

public:

	/** the tick method */
	void Tick(double delta_time);
	/** whenever a key event is received */
	bool OnKeyEvent(int key, int action);
	/** the rendering method */
	void Display(chaos::box2 const & viewport);
	/** called whenever a gamepad input is comming */
	bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

	/** basic initialization */
	bool InitializeGame(GLFWwindow * in_glfw_window);
	/** initialization from the config file */
	bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);

protected:

	/** internal method to update the state of the game */
	void UpdateGameState(double delta_time);

	/** internal methods to generate the atlas for sprites */
	bool GenerateAtlas(nlohmann::json const & config, boost::filesystem::path const & config_path);

	/** internal method called to reset cached inputs */
	void ResetPlayerCachedInputs();
	/** internal method to test for keys and update stick position */
	void HandleKeyboardInputs();

	/** require a pause or resume */
	bool RequireGamePauseOrResume();
	/** require a return to main menu */
	bool RequireReturnToMainMenu();
	/** require a game Start */
	bool RequireGameStart();


	/** require a gameover */
	bool RequireGameOver();

protected:

	/** the window in GLFW library */
	GLFWwindow * glfw_window = nullptr;

	/** some states */
	int game_state = STATE_MAINMENU;

	bool game_paused = false;
	bool game_started = false;
	bool pending_gameover = false;
	bool pending_restart_game = false; 

	/** the current stick position */
	glm::vec2 left_stick_position  = glm::vec2(0.0f, 0.0f);
	glm::vec2 right_stick_position = glm::vec2(0.0f, 0.0f);

	/** the current gamepad manager */
	boost::intrusive_ptr<chaos::MyGLFW::GamepadManager> gamepad_manager;

	/** the texture atlas */
	chaos::BitmapAtlas::TextureArrayAtlas texture_atlas;
};

// =================================================
// LudumGamepadManager
// =================================================

class LudumGamepadManager : public chaos::MyGLFW::GamepadManager
{
public:

	/** the constructor */
	LudumGamepadManager(LudumGame * in_game) : game(in_game) {}

protected:

	/** the gamepad manager */
	virtual bool DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad) override;

protected:

	/** pointer on the game */
	LudumGame * game = nullptr;
};