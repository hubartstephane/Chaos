#include "LudumWindow.h"

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
#if 0
	glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
	glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

	float far_plane = 1000.0f;
	glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

	chaos::GLTools::SetViewportWithAspect(size, VIEWPORT_WANTED_ASPECT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);   // when viewer is inside the cube

	float world_x = 1000.0f;
	float world_y = world_x / VIEWPORT_WANTED_ASPECT;

	if (game != nullptr)
		game->Display(glm::vec2(world_x, world_y));
#endif
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

	// initialize the game
	nlohmann::json const * game_config = chaos::JSONTools::GetStructure(config, "game");
	if (game_config != nullptr)
		if (!game->InitializeFromConfiguration(*game_config, config_path))
			return false;
	

#if 0
	float WORLD_X = 1000.0f;
	glm::vec2 world_size = glm::vec2(WORLD_X, WORLD_X / VIEWPORT_WANTED_ASPECT);
	if (!game->Initialize(glfw_window, chaos::JSONTools::GetStructure(config, "game"), config_path, world_size))
		return false;
#endif
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
