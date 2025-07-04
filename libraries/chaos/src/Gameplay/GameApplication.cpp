#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GameApplication::GameApplication(SubClassOf<Game> in_game_class):
			game_class(in_game_class)
	{
		assert(in_game_class.IsValid());
	}

	void GameApplication::FinalizeGPUResources()
	{
		assert(glfwGetCurrentContext() == shared_context);

		if (game != nullptr)
		{
			game->Finalize();
			game = nullptr;
		}
		WindowApplication::FinalizeGPUResources();
	}

	bool GameApplication::InitializeGPUResources(JSONReadConfiguration config)
	{
		assert(glfwGetCurrentContext() == shared_context);

		// create the game
		game = game_class.CreateInstance();
		if (game == nullptr)
			return false;
		GiveChildConfiguration(game.get(), "game");
		if (!game->Initialize())
			return false;
		// super method : need to be done game initialization ! (because atlas creation requires the game to have loaded its levels)
		if (!WindowApplication::InitializeGPUResources(config))
			return false;
		// create now some game resources that need application resources to be initialized
		if (!game->CreateGPUResources())
			return false;
		return true;
	}

	bool GameApplication::DoTick(float delta_time)
	{
		assert(glfwGetCurrentContext() == shared_context);
		// super
		WindowApplication::DoTick(delta_time);
		// update the game
		if (game != nullptr)
			if (!IsGameSuspended())
				game->Tick(delta_time);
		return true;
	}

	bool GameApplication::FillAtlasGeneratorInput(AtlasInput& input)
	{
		if (!WindowApplication::FillAtlasGeneratorInput(input))
			return false;
		if (game != nullptr)
			if (!game->FillAtlasGeneratorInput(input))
				return false;
		return true;
	}

	bool GameApplication::OnMouseMoveImpl(glm::vec2 const& delta)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnMouseMove(delta))
					return true;
		return WindowApplication::OnMouseMoveImpl(delta);
	}

	bool GameApplication::OnMouseButtonImpl(MouseButtonEvent const &mouse_button_event)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnMouseButton(mouse_button_event))
					return true;
		return WindowApplication::OnMouseButtonImpl(mouse_button_event);
	}

	bool GameApplication::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnMouseWheel(scroll_x, scroll_y))
					return true;
		return WindowApplication::OnMouseWheelImpl(scroll_x, scroll_y);
	}

	bool GameApplication::OnKeyEventImpl(KeyEvent const& key_event)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnKeyEvent(key_event))
					return true;
		return WindowApplication::OnKeyEventImpl(key_event);
	}

	bool GameApplication::OnCharEventImpl(unsigned int c)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnCharEvent(c))
					return true;
		return WindowApplication::OnCharEventImpl(c);
	}

	bool GameApplication::IsGameSuspended() const
	{
		return IsImGuiMenuEnabled();
	}

}; // namespace chaos

