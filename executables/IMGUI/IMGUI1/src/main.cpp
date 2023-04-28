#include <chaos/chaos.h>
#include "Imgui.h"





class WindowOpenGLTest : public chaos::Window
{
	friend class MyEvent;

	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

public:

	WindowOpenGLTest()
	{
		static int k = 0;
		id = k;
		++k;
	}

protected:

	int id = 0;


	virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
	{
		if (action == GLFW_PRESS)
		{
		}
		return true;
	}

	virtual bool DoTick(float delta_time) override
	{

		return true;
	}

	virtual bool OnDraw(chaos::GPURenderer* renderer, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.5f, 0.3f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (id == 0)
		{
			ImGui::Begin("##truc1");
			ImGui::Text("Hello");
			ImGui::End();
		}
		if (id == 1)
		{
			ImGui::Begin("##truc2");
			ImGui::Text("machin");
			ImGui::End();
		}
		if (id == 2)
		{
			ImGui::Begin("##truc3");
			ImGui::Text("bidule");
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		return true;
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const& event) override
	{
		if (event.IsKeyReleased(GLFW_KEY_ESCAPE))
		{
			RequireWindowClosure();
			return true;
		}
		return chaos::Window::OnKeyEventImpl(event);
	}
};


int main(int argc, char** argv, char** env)
{
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 1200;
	create_params.height = 500;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}
