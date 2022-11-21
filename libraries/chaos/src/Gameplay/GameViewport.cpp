#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void GameViewport::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
	{
		if (game != nullptr)
			game->OnInputModeChanged(new_mode, old_mode);
	}

	bool GameViewport::OnCharEventImpl(unsigned int c)
	{
		// give inputs to the game
		if (game != nullptr)
			if (game->OnCharEvent(c))
				return true;
		return Viewport::OnCharEventImpl(c);
	}

	bool GameViewport::OnKeyEventImpl(KeyEvent const& event)
	{
		// super method
		if (Viewport::OnKeyEventImpl(event))
			return true;
		// give inputs to the game
		if (game != nullptr)
			if (game->OnKeyEvent(event))
				return true;
		return false;
	}

	bool GameViewport::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (game != nullptr)
			if (game->OnMouseButton(button, action, modifier))
				return true;
		return Viewport::OnMouseButtonImpl(button, action, modifier);
	}

	bool GameViewport::OnMouseMoveImpl(double x, double y)
	{
		if (game != nullptr)
			if (game->OnMouseMove(x, y))
				return true;
		return Viewport::OnMouseMoveImpl(x, y);
	}
#if 0
	ViewportPlacement GameViewport::GetRequiredViewport(glm::ivec2 const & size) const
	{
		if (game != nullptr)
			return game->GetRequiredViewport(size);
		return Window::GetRequiredViewport(size);
	}
#endif

	bool GameViewport::OnDraw(GPURenderer * renderer, WindowDrawParams const& draw_params, GPUProgramProviderInterface const * uniform_provider)
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

	void GameViewport::SetGame(Game* in_game)
	{
		game = in_game;
	}

	namespace Arguments
	{
#if _DEBUG
		CHAOS_APPLICATION_ARG(bool, NoAutoPause);
#endif
	};

	void GameViewport::OnIconifiedStateChange(bool iconified)
	{
		// do not execute following code in debug because it does not fit well with debugger
#if _DEBUG
		if (!Arguments::NoAutoPause.Get())
#endif
		if (game != nullptr)
			if (iconified)
				game->RequirePauseGame();
		Viewport::OnIconifiedStateChange(iconified);
	}

	void GameViewport::OnFocusStateChange(bool gain_focus)
	{
		// do not execute following code in debug because it does not fit well with debugger
#if _DEBUG
		if (!Arguments::NoAutoPause.Get())
#endif
		if (game != nullptr)
			if (!gain_focus)
				game->RequirePauseGame();
		Viewport::OnFocusStateChange(gain_focus);
	}

}; // namespace chaos

