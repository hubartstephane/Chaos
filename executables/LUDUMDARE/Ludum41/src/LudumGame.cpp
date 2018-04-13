#include "LudumGame.h"
#include <chaos/JSONTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>





#if 0
#include <chaos/CollisionFramework.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 

#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>

#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/SoundManager.h>
#include <json.hpp>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/SpriteManager.h>
#include <chaos/JSONTools.h>
#endif




bool LudumGamepadManager::DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	if (game != nullptr)
		return game->OnPhysicalGamepadInput(physical_gamepad);
	return true;
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
	if (!texture_atlas.LoadFromBitmapAtlas(atlas))
		return false;

	// dump the atlas
#if _DEBUG
	chaos::Application * application = chaos::Application::GetInstance();
	atlas.SaveAtlas(application->GetUserLocalTempPath() / "LudumAtlas");
#endif

	return true;
}

/*
void LudumGame::EnterPause()
{
	

}
*/

bool LudumGame::RequireGamePauseOrResume()
{
	if (game_state == STATE_PLAYING)
	{

		return true;
	}
	else if (game_state == STATE_PAUSE)
	{


		return true;
	}
	return false;
}

bool LudumGame::RequireReturnToMainMenu()
{
	if (game_state != STATE_PLAYING || game_state != STATE_PAUSE)
		return false;




	return true;
}

bool LudumGame::RequireGameStart()
{
	if (game_state != STATE_MAINMENU)
		return false;



	return true;
}

bool LudumGame::RequireGameOver()
{
	if (game_state != STATE_PLAYING)
		return false;



	return true;

}

void LudumGame::HandleKeyboardInputs()
{
	// test whether the stick position can be overriden
	glm::vec2 simulated_stick = glm::vec2(0.0f, 0.0f);

	if (glfwGetKey(glfw_window, GLFW_KEY_LEFT))
		simulated_stick.x -= 1.0f;
	if (glfwGetKey(glfw_window, GLFW_KEY_RIGHT))
		simulated_stick.x += 1.0f;

	if (glfwGetKey(glfw_window, GLFW_KEY_DOWN))
		simulated_stick.y -= 1.0f;
	if (glfwGetKey(glfw_window, GLFW_KEY_UP))
		simulated_stick.y += 1.0f;

	if (glm::length2(simulated_stick) > 0)
		left_stick_position = simulated_stick;
}

void LudumGame::UpdateGameState(double delta_time)
{
	game_automata->Tick(delta_time);

	if (pending_gameover)
	{

	}
	else if (pending_restart_game)
	{

	}
	else
	{




	}

}

void LudumGame::Tick(double delta_time)
{
	// catch all stick inputs
	gamepad_manager->Tick((float)delta_time);
	// handle keyboard inputs
	HandleKeyboardInputs();
	// update the game state
	UpdateGameState(delta_time);
	// clear the cached inputs
	ResetPlayerCachedInputs();
}

bool LudumGame::OnKeyEvent(int key, int action)
{
	// MAIN MENU to PLAYING
	if (action == GLFW_PRESS)
		if (RequireGameStart())
			return true;

	// PLAYING to PAUSE
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		if (RequireGamePauseOrResume())
			return true;

	// QUIT GAME
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		if (RequireReturnToMainMenu())
			return true;

	return false;
}

void LudumGame::Display(chaos::box2 const & viewport)
{
	// clear the buffers
	glm::vec4 clear_color(1.0f, 0.0f, 0.0f, 0.0f);
	glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

	float far_plane = 1000.0f;
	glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

	// some states
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE); 




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

	return true;
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	GenerateAtlas(config, config_path);




	return true;
}

void LudumGame::ResetPlayerCachedInputs()
{
	left_stick_position  = glm::vec2(0.0f, 0.0f);
	right_stick_position = glm::vec2(0.0f, 0.0f);
}

bool LudumGame::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	// cache the stick position
	glm::vec2 lsp = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_LEFT_AXIS);
	if (glm::length2(lsp) > 0.0f)
		left_stick_position = lsp;

	glm::vec2 rsp = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);
	if (glm::length2(rsp) > 0.0f)
		right_stick_position = rsp;

	// maybe a start game
	if (physical_gamepad->IsAnyButtonPressed())
		if (RequireGameStart())
			return true;

	// maybe a game/pause resume

	if (
		(physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_SELECT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED) ||
		(physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_START) == chaos::MyGLFW::BUTTON_BECOME_PRESSED))
	{
		if (RequireGamePauseOrResume())
			return true;
	}

	return true;
}