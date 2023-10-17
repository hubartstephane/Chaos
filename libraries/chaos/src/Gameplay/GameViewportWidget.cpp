#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{


	PlaceHolderWidget::PlaceHolderWidget()
	{
		static int color = 0;

		clear_color.x = float((color * 30 + 50) % 256) / float(255.0f);
		clear_color.y = float((color * 30 + 50) % 256) / float(255.0f);
		clear_color.z = float((color * 30 + 50) % 256) / float(255.0f);
		clear_color.w = 1.0f;

		++color;
	
	}


	bool PlaceHolderWidget::OnDraw(GPURenderer* renderer, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params)
	{
		GLTools::SetViewport(placement);

		// scissor: avoid glClearBufferfv(...) functions to clear the whole screen
		GLTools::SetScissorBox(placement);
		glEnable(GL_SCISSOR_TEST);

		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		glDisable(GL_SCISSOR_TEST);

		return true;
	}











	void GameViewportWidget::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
	{
		if (game != nullptr)
			game->OnInputModeChanged(new_mode, old_mode);
	}

	bool GameViewportWidget::OnCharEventImpl(unsigned int c)
	{
		// give inputs to the game
		if (game != nullptr)
			if (game->OnCharEvent(c))
				return true;
		return Widget::OnCharEventImpl(c);
	}

	bool GameViewportWidget::OnKeyEventImpl(KeyEvent const& event)
	{
		// super method
		if (Widget::OnKeyEventImpl(event))
			return true;
		// give inputs to the game
		if (game != nullptr)
			if (game->OnKeyEvent(event))
				return true;
		return false;
	}

	bool GameViewportWidget::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (game != nullptr)
			if (game->OnMouseButton(button, action, modifier))
				return true;
		return Widget::OnMouseButtonImpl(button, action, modifier);
	}

	bool GameViewportWidget::OnMouseMoveImpl(glm::vec2 const & delta)
	{
		if (game != nullptr)
			if (game->OnMouseMove(delta))
				return true;
		return Widget::OnMouseMoveImpl(delta);
	}

	bool GameViewportWidget::OnDraw(GPURenderer * renderer, GPUProgramProviderInterface const * uniform_provider, WindowDrawParams const& draw_params)
	{
		if (game != nullptr)
		{
			if (placement.size.x > 0.0f && placement.size.y > 0.0f)
			{
				GPUProgramProviderChain main_uniform_provider(this, uniform_provider);

				// set the viewport
				GPURenderParams render_params;
				render_params.viewport = placement;
				GLTools::SetViewport(render_params.viewport); // ignore the size from draw_params. use our own placement

				// scissor: avoid glClearBufferfv(...) functions to clear the whole screen
				GLTools::SetScissorBox(render_params.viewport);
				glEnable(GL_SCISSOR_TEST);

				game->Display(renderer, &main_uniform_provider, render_params);

				// restore scissor
				glDisable(GL_SCISSOR_TEST);

			}
		}
		return true;
	}

	void GameViewportWidget::SetGame(Game* in_game)
	{
		game = in_game;
	}

	namespace GlobalVariables
	{
#if _DEBUG
		CHAOS_GLOBAL_VARIABLE(bool, NoAutoPause, false);
#endif
	};








#if 0

	void GameViewportWidget::OnIconifiedStateChange(bool iconified)
	{
		// do not execute following code in debug because it does not fit well with debugger
#if _DEBUG
		if (!GlobalVariables::NoAutoPause.Get())
#endif
		if (game != nullptr)
			if (iconified)
				game->RequirePauseGame();
		Widget::OnIconifiedStateChange(iconified);
	}

	void GameViewportWidget::OnFocusStateChange(bool gain_focus)
	{
		// do not execute following code in debug because it does not fit well with debugger
#if _DEBUG
		if (!GlobalVariables::NoAutoPause.Get())
#endif
		if (game != nullptr)
			if (!gain_focus)
				game->RequirePauseGame();
		Widget::OnFocusStateChange(gain_focus);
	}

#endif









}; // namespace chaos

