#include <chaos/chaos.h>
#include "Imgui.h"




CHAOS_APPLICATION_ARG(float, myfloat)
CHAOS_APPLICATION_ARG(int, myint)
CHAOS_APPLICATION_ARG(std::string, mystring)

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
		static bool show_demo = false;
		static bool show_metrics = false;
		static bool show_debug_log = false;
		static bool show_stack_tool = false;
		static bool show_about = false;
		static bool show_style_editor = false;
		static bool show_user_guide = false;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("ImGui"))
			{
#define CHAOS_IMGUI_MENUITEM(X) if (ImGui::MenuItem(#X, nullptr, X, true)){ X = !X; }
				CHAOS_IMGUI_MENUITEM(show_demo);
				CHAOS_IMGUI_MENUITEM(show_metrics);
				CHAOS_IMGUI_MENUITEM(show_debug_log);
				CHAOS_IMGUI_MENUITEM(show_stack_tool);
				CHAOS_IMGUI_MENUITEM(show_about);
				CHAOS_IMGUI_MENUITEM(show_style_editor);
				CHAOS_IMGUI_MENUITEM(show_user_guide);
#undef CHAOS_IMGUI_MENUITEM
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (show_demo)
			ImGui::ShowDemoWindow(&show_demo);
		if (show_metrics)
			ImGui::ShowMetricsWindow(&show_metrics);
		if (show_debug_log)
			ImGui::ShowDebugLogWindow(&show_debug_log);
		if (show_stack_tool)
			ImGui::ShowStackToolWindow(&show_stack_tool);
		if (show_about)
			ImGui::ShowAboutWindow(&show_about);
		if (show_style_editor)
			ImGui::ShowStyleEditor();
		if (show_user_guide)
			ImGui::ShowUserGuide();












#if 0
		// create menu bar
		ImGui::SetNextWindowPos({ -1.0f, 0.0f });
		ImGui::SetNextWindowSize({ float(GetWindowSize().x + 2), 0.0f });

		ImGui::Begin("My First Tool", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
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
		ImGui::End();
#endif

#if 0
		// show the command arguments
		if (my_tool_active)
		{
			ImGui::Begin("CommandLineArguments", &my_tool_active);
			for (auto arg : chaos::ApplicationArgumentManager::GetInstance()->GetArguments())
			{
				if (*arg->GetTypeInfo() == typeid(bool))
				{
					if (chaos::ApplicationArgument<bool>* bool_arg = auto_cast(arg))
					{
						ImGui::Checkbox(arg->GetName(), &bool_arg->Get());
					}
				}


				if (*arg->GetTypeInfo() == typeid(float))
				{
					if (chaos::ApplicationArgument<float>* float_arg = auto_cast(arg))
					{
						ImGui::SliderFloat(arg->GetName(), &float_arg->Get(), 0.0f, 1.0f);
					}
				}
			}


			float myvector[2] = { 0.0f, 0.0f };
			ImGui::SliderFloat2("vector", myvector, 0.f, 1.0f);
			ImGui::End();

			ImGui::BeginGroup();
			static int radio_index = 0;
			ImGui::RadioButton("choice1", &radio_index, 0);
			ImGui::RadioButton("choice2", &radio_index, 1);
			ImGui::RadioButton("choice3", &radio_index, 2);
			ImGui::RadioButton("choice4", &radio_index, 3);
			ImGui::EndGroup();
		}

#endif


#if 0
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
#endif







		if (id == 0)
		{
			ImGui::Begin("##truc1", &xxx, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
			ImGui::Text("Hello");
			ImGui::End();

			ImGui::Begin("##truc1", &xxx);
			ImGui::Text("Machin");
			ImGui::Text("Machin");
			ImGui::Text("machinccc %d", xxx);


			ImGui::BeginGroup();

			ImGui::EndGroup();


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
