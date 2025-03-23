#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool GameWindow::OnInitialize(JSONReadConfiguration config)
	{
		// get the game (need to be done before super call)
		if (GameApplication* game_application = Application::GetInstance())
			game = game_application->GetGame();
		// super call
		if (!Window::OnInitialize(config))
			return false;

		return true;
	}

	bool GameWindow::CreateRootWidget()
	{
		// super call
		if (!Window::CreateRootWidget())
			return false;
		// create the game viewport
		if (GameApplication* game_application = Application::GetInstance())
		{
			if (GameViewportWidget* game_viewport_widget = game_application->GetGameViewportWidgetClass().CreateInstance())
			{
				WidgetLayout layout;
				layout.aspect_ratio = game->GetViewportWantedAspect();
				game_viewport_widget->SetLayout(layout);
				game_viewport_widget->SetGame(game.get());

				root_widget->AddChildWidget(game_viewport_widget);
			}
		}
		return true;
	}

	// shuxxx GameViewportWidget test

#if 0

	for (int i = 0; i < 3; ++i)
	{
		PlaceHolderWidget* widget = new PlaceHolderWidget;

		WidgetLayout layout;
		layout.aspect_ratio = 4.0f / 3.0f;
		layout.padding = Padding(5.0f);
		layout.fill_mode_x = WidgetFillMode::PACKED_NORMAL;

		widget->SetLayout(layout, false);

		root_widget->AddChildWidget(widget);
	}

#endif










}; // namespace chaos

