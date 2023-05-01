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
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (id == 0)
			{
				ImGui::Begin("##truc1");
				ImGui::Text("Hello");
				ImGui::End();
			}

		}
		return true;
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const& event) override
	{
		{
			ImGui::Begin("##truc1");
			ImGui::Text("Hello");
			ImGui::End();
		}

		if (event.action == GLFW_PRESS || event.action == GLFW_REPEAT)
		{
		//	if (id == 0)


			col += 0.05f;
			if (col > 1.0f)
				col = 0.0f;
		}

		if (event.IsKeyReleased(GLFW_KEY_ESCAPE))
		{
			RequireWindowClosure();
			return true;
		}
		return chaos::Window::OnKeyEventImpl(event);
	}

	virtual void DrawImGui(chaos::WindowDrawParams const& draw_params) override
	{
		if (id == 0)
		{
			ImGui::Begin("##truc1");
			ImGui::Text("Hello");
			ImGui::End();

			ImGui::Begin("##truc1");
			ImGui::Text("Machin");
			ImGui::Text("Machin");
			ImGui::End();

			ImGui::Begin("##truc1");
			if (ImGui::Button("Machin"))
			{
				id = id;
			}
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
	}

	virtual bool DoTick(float delta_time) override
	{








		return true;
	}

	virtual bool OnDraw(chaos::GPURenderer* renderer, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(col, 0.3f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);



		return true;
	}

	float col = 0.0f;
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
