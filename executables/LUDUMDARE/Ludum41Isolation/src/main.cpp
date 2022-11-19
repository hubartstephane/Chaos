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
		glfwSetInputMode(window->GetGLFWHandler(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
};

int main(int argc, char** argv, char** env)
{
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 500;
	create_params.height = 500;
	create_params.monitor_index = 0;

	return chaos::RunGame<LudumGame>(argc, argv, env, create_params);
}
