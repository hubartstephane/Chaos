#include "Ludum41CustomPCH.h"
#include "Ludum41CustomGame.h"

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
	chaos::WindowPlacementInfo placement_info;

	return chaos::RunGame<LudumGame>(argc, argv, env, placement_info);
}