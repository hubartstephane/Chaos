#include <chaos/Chaos.h>

namespace chaos
{
	void GameWindow::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
	{
		if (game != nullptr)
			game->OnInputModeChanged(new_mode, old_mode);
	}

	bool GameWindow::OnCharEventImpl(unsigned int c)
	{
		// give inputs to the game
		if (game != nullptr)
			if (game->OnCharEvent(c))
				return true;
		return Window::OnCharEventImpl(c);
	}

	bool GameWindow::OnKeyEventImpl(KeyEvent const& event)
	{
		// super method
		if (Window::OnKeyEventImpl(event))
			return true;
		// give inputs to the game
		if (game != nullptr)
			if (game->OnKeyEvent(event))
				return true;
		return false;
	}

	bool GameWindow::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (game != nullptr)
			if (game->OnMouseButton(button, action, modifier))
				return true;
		return Window::OnMouseButtonImpl(button, action, modifier);
	}

	bool GameWindow::OnMouseMoveImpl(double x, double y)
	{
		if (game != nullptr)
			if (game->OnMouseMove(x, y))
				return true;
		return Window::OnMouseMoveImpl(x, y);
	}

	ViewportPlacement GameWindow::GetRequiredViewport(glm::ivec2 const & size) const
	{
		if (game != nullptr)
			return game->GetRequiredViewport(size);
		return Window::GetRequiredViewport(size);
	}

	bool GameWindow::OnDraw(GPURenderer * renderer, WindowDrawParams const& draw_params, GPUProgramProviderInterface const * uniform_provider)
	{
		// shurender

		if (game != nullptr)
		{
			GPURenderParams render_params;
			render_params.viewport = draw_params.viewport;
			render_params.full_size = draw_params.full_size;
			game->Display(renderer, uniform_provider, render_params);
		}
		return true;
	}

	void GameWindow::SetGame(Game* in_game)
	{
		game = in_game;
	}

	void GameWindow::Finalize()
	{
		game = nullptr;
	}
#if _DEBUG
	CHAOS_APPLICATION_ARG(bool, NoAutoPause);
#endif

	void GameWindow::OnIconifiedStateChange(bool iconified)
	{
		// do not execute following code in debug because it does not fit well with debugger
#if _DEBUG
		if (!Arguments::NoAutoPause)
#endif
		if (iconified)
			game->RequirePauseGame();
	}

	void GameWindow::OnFocusStateChange(bool gain_focus)
	{
		// do not execute following code in debug because it does not fit well with debugger
#if _DEBUG
		if (!Arguments::NoAutoPause)
#endif
		if (!gain_focus)
			game->RequirePauseGame();

	}

}; // namespace chaos

