#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool GameWindow::InitializeFromConfiguration(nlohmann::json const& config)
	{
		if (!Window::InitializeFromConfiguration(config))
			return false;

		if (GameApplication * game_application = Application::GetInstance())
			game = game_application->GetGame();

		return true;
	}

}; // namespace chaos

