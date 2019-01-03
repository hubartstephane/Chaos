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

	void GameWindow::OnCharEvent(unsigned int c)
	{
		// give inputs to the game
		if (game != nullptr)
			if (game->OnCharEvent(c))
				return;
	}

	void GameWindow::OnKeyEvent(int key, int scan_code, int action, int modifier)
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


	void GameWindow::OnMouseButton(int button, int action, int modifier)
	{
		if (game != nullptr)
			game->OnMouseButton(button, action, modifier);
	}

	void GameWindow::OnMouseMove(double x, double y)
	{
		if (game != nullptr)
			game->OnMouseMove(x, y);
	}

	bool GameWindow::OnDraw(glm::ivec2 size)
	{
		if (game != nullptr)
			game->Display(size);
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

#if _DEBUG
			chaos::WinTools::ShowFile(user_temp);
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

		exit(0); // shuxxx
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

}; // namespace death

