#include "Ludum41IsolationPCH.h"
#include "Ludum41IsolationGame.h"

class LudumGameApplication : public chaos::GameApplication
{
public:

	using chaos::GameApplication::GameApplication;

	virtual void OnWindowCreated(chaos::Window* window) override
	{
		// super
		chaos::GameApplication::OnWindowCreated(window);
		// hidden cursor
		window->SetCursorMode(chaos::CursorMode::Disabled);
	}
};

int main(int argc, char** argv, char** env)
{
	chaos::WindowCreateParams create_params;
	create_params.width = 500;
	create_params.height = 500;

	return chaos::RunGame<LudumGame>(argc, argv, env, create_params);
}
