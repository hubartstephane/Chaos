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
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::GLFWWindowHints hints;

	chaos::RunApplication<LudumGameApplication>(argc, argv, env, LudumGame::GetStaticClass(), chaos::GameWindow::GetStaticClass(), params, hints);
	return 0;
}
