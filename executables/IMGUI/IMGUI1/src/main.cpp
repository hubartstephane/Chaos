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

	bool my_tool_active = true;
	glm::vec4 my_color = { 0.0f, 0.0f, 0.0f, 1.0f };


	bool xxx = true;


	virtual void DrawImGui(chaos::WindowDrawParams const& draw_params) override
	{

		//ImGuiWindowFlags_NoTitleBar

		// Create a window called "My First Tool", with a menu bar.

		if (my_tool_active)
		{

			ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);// | ImGuiWindowFlags_NoTitleBar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
					if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
					if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}


			for (auto arg : chaos::ApplicationArgumentManager::GetInstance()->GetArguments())
			{
				if (*arg->GetTypeInfo() == typeid(bool))
				{
					chaos::ApplicationArgument<bool>* bool_arg = auto_cast(arg);

					ImGui::Checkbox(arg->GetName(), &bool_arg->Get());
				}

			}








			float f = 0.3f;
			char buf[30] = { 0 };
			ImGui::Text("Hello, world %d", 123);
			if (ImGui::Button("Save"))
			{
			}
			ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

			// Edit a color stored as 4 floats
			ImGui::ColorEdit4("Color", &my_color.x);

			// Generate samples and plot them
			float samples[100];
			for (int n = 0; n < 100; n++)
				samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
			ImGui::PlotLines("Samples", samples, 100);

			// Display contents in a scrolling region
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");

			ImGui::BeginChild("Scrolling");
			for (int n = 0; n < 5; n++)
				ImGui::Text("%04d: Some text", n);

			ImGui::BeginChild("Other Scrolling");
			for (int n = 0; n < 2; n++)
				ImGui::Text("Other Text");
			ImGui::EndChild();
			ImGui::EndChild();



			ImGui::End();


		}










		if (id == 0)
		{
			ImGui::Begin("##truc1", &xxx);
			ImGui::Text("Hello");
			ImGui::End();

			ImGui::Begin("##truc1", &xxx);
			ImGui::Text("Machin");
			ImGui::Text("Machin");
			ImGui::Text("machinccc %d", xxx);
			ImGui::End();

			ImGui::Begin("##truc1", &xxx);
			if (ImGui::Button("Machin"))
			{
				id = id;
			}
			ImGui::End();
		}
		if (id == 1)
		{
			ImGui::Begin("##truc2", &xxx);
			ImGui::Text("machin");
			ImGui::Text("machin %d", xxx);
			ImGui::End();
		}
		if (id == 2)
		{
			ImGui::Begin("##truc3", &xxx);
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
