#include <death/GameWindow.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/InputMode.h>
#include <chaos/GeometryFramework.h>

namespace death
{
	void GameWindow::OnInputModeChanged(int new_mode, int old_mode)
	{
		if (game != nullptr)
			game->OnInputModeChanged(new_mode, old_mode);
	}

	bool GameWindow::OnCharEvent(unsigned int c)
	{
		// give inputs to the game
		if (game != nullptr)
			if (game->OnCharEvent(c))
				return true;
		return chaos::MyGLFW::Window::OnCharEvent(c);
	}

	bool GameWindow::OnKeyEvent(int key, int scan_code, int action, int modifier)
	{
		// give inputs to the game
		if (game != nullptr)
			if (game->OnKeyEvent(key, action))
				return true;
		// kill the window
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			if (modifier & GLFW_MOD_SHIFT)
			{
				RequireWindowClosure();
				return true;
			}
		}
		// try to go fullscreen
		if (key == GLFW_KEY_F12 && action == GLFW_PRESS)
		{
			ToggleFullscreen();
			return true;
		}
		// super method
		return chaos::MyGLFW::Window::OnKeyEvent(key, scan_code, action, modifier);
	}


	bool GameWindow::OnMouseButton(int button, int action, int modifier)
	{
		if (game != nullptr)
			if (game->OnMouseButton(button, action, modifier))
				return true;
		return chaos::MyGLFW::Window::OnMouseButton(button, action, modifier);
	}

	bool GameWindow::OnMouseMove(double x, double y)
	{
		if (game != nullptr)
			if (game->OnMouseMove(x, y))
				return true;
		return chaos::MyGLFW::Window::OnMouseMove(x, y);
	}

	bool GameWindow::OnDraw(chaos::Renderer * in_renderer, glm::ivec2 size)
	{
		if (game != nullptr)
			game->Display(in_renderer, size);
		return true;
	}

	void GameWindow::Finalize()
	{
		game = nullptr;
	}

	Game * GameWindow::CreateGameInstance()
	{
		return nullptr;
	}

	bool GameWindow::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		// open user temp directory and dump the config file
		chaos::Application * application = chaos::Application::GetInstance();
		if (application != nullptr)
		{
			boost::filesystem::path user_temp = application->CreateUserLocalTempDirectory(); // XXX : this directory is necessary for Best score

			// display the directories to help debugging
#if _DEBUG
			if (application->HasCommandLineFlag("-ShowDirectories"))
			{
				chaos::WinTools::ShowFile(user_temp);
				chaos::WinTools::ShowFile(application->GetResourcesPath()); 			
			}
#endif
		}

#if 0
		chaos::JSONTools::DumpConfigFile(config);
#endif

		// create the game
		game = CreateGameInstance();
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

	void GameWindow::TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
#if !_DEBUG
		hints.toplevel = 1;
#endif
		hints.decorated = 1;
	}

	bool GameWindow::Tick(double delta_time)
	{
		game->Tick(delta_time);
		return true; // refresh
	}

	void GameWindow::OnIconifiedStateChange(bool iconified)
	{
		if (iconified)
			game->RequirePauseGame();
	}

	void GameWindow::OnFocusStateChange(bool gain_focus)
	{
		if (!gain_focus)
			game->RequirePauseGame();
	}

}; // namespace death

