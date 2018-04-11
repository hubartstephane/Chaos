#include "LudumWindow.h"


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







float const LudumWindow::VIEWPORT_WANTED_ASPECT = (16.0f / 9.0f);

void LudumWindow::OnKeyEvent(int key, int scan_code, int action, int modifier)
{
	// kill the window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (modifier & GLFW_MOD_SHIFT)
		{
			RequireWindowClosure();
			return;
		}						
	}

	// give inputs to the game
	if (game != nullptr)
		if (game->OnKeyEvent(key, action))
			return;
}

bool LudumWindow::OnDraw(glm::ivec2 size) 
{
	chaos::box2 viewport = chaos::GLTools::SetViewportWithAspect(size, VIEWPORT_WANTED_ASPECT);
	if (game != nullptr)
		game->Display(viewport);
	return true;
}

void LudumWindow::Finalize() 
{
	game = nullptr;
}

bool LudumWindow::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) 
{   
	// create the game
	game = new LudumGame;
	if (game == nullptr)
		return false;

	// basic initialization of the game */
	if (!game->InitializeGame(glfw_window))
		return false;
	
	// initialize the game
	nlohmann::json const * game_config = chaos::JSONTools::GetStructure(config, "game");
	if (game_config != nullptr)
		if (!game->InitializeFromConfiguration(*game_config, config_path))
			return false;

	return true;
}

void LudumWindow::TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const 
{
	chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
#if !_DEBUG
	hints.toplevel  = 1;
#endif
	hints.decorated = 1;
}

bool LudumWindow::Tick(double delta_time) 
{
	game->Tick(delta_time);
	return true; // refresh
}
