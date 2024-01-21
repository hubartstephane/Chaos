#include "Ludum41PCH.h"
#include "Ludum41Game.h"

class LudumGameApplication : public chaos::GameApplication
{
public:

	using chaos::GameApplication::GameApplication;

	virtual void OnWindowCreated(chaos::Window* window) override
	{
		// super
		chaos::GameApplication::OnWindowCreated(window);
		// hidden cursor
		window->SetCursorMode(chaos::CursorMode::DISABLED);
	}
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}