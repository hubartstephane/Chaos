#include <chaos/chaos.h>
#include "Imgui.h"




CHAOS_GLOBAL_VARIABLE(float, myfloat)
CHAOS_GLOBAL_VARIABLE(int, myint)
CHAOS_GLOBAL_VARIABLE(std::string, mystring)










#if 0
ImGui::Begin("My First Tool");
for (auto arg : chaos::GlobalVariableManager::GetInstance()->GetArguments())
{
	if (*arg->GetTypeInfo() == typeid(bool))
	{
		chaos::GlobalVariable<bool>* bool_arg = auto_cast(arg);

		ImGui::Checkbox(arg->GetName(), &bool_arg->Get());
	}
}
#endif




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
			chaos::Log::Warning("Message");

			static int i = 0;
#if 1
			if (i % 7 == 0)
				chaos::Logger::GetInstance()->Message("truc0", "truc0");
			if (i % 7 == 1)
				chaos::Logger::GetInstance()->Message("truc1", "truc1");
			if (i % 7 == 2)
				chaos::Logger::GetInstance()->Message("truc2", "truc2");
			if (i % 7 == 3)
				chaos::Logger::GetInstance()->Message("truc3", "truc3");
			if (i % 7 == 4)
				chaos::Logger::GetInstance()->Message("truc4", "truc4");
			if (i % 7 == 5)
				chaos::Logger::GetInstance()->Message("truc5", "truc5");
			if (i % 7 == 6)
				chaos::Logger::GetInstance()->Message("truc6", "truc6");
#endif

			++i;
		}
		return true;
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const& event) override
	{

		if (event.action == GLFW_PRESS || event.action == GLFW_REPEAT)
		{
		//	if (id == 0)


			col += 0.05f;
			if (col > 1.0f)
				col = 0.0f;
		}

		if (event.IsKeyReleased(chaos::KeyboardButton::ESCAPE))
		{
			Destroy();
			//RequireWindowClosure();
			return true;
		}
		return chaos::Window::OnKeyEventImpl(event);
	}

	bool my_tool_active = true;
	glm::vec4 my_color = { 0.0f, 0.0f, 0.0f, 1.0f };



#if 0

	// show the command arguments
	if (my_tool_active)
	{
		ImGui::Begin("CommandLineArguments", &my_tool_active);
		for (auto arg : chaos::GlobalVariableManager::GetInstance()->GetArguments())
		{
			if (*arg->GetTypeInfo() == typeid(bool))
			{
				if (chaos::GlobalVariable<bool>* bool_arg = auto_cast(arg))
				{
					ImGui::Checkbox(arg->GetName(), &bool_arg->Get());
				}
			}


			if (*arg->GetTypeInfo() == typeid(float))
			{
				if (chaos::GlobalVariable<float>* float_arg = auto_cast(arg))
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

	virtual void OnDrawWindowImGuiMenu() override
	{
		chaos::Window::OnDrawWindowImGuiMenu();
	}

	virtual void OnDrawWindowImGuiContent() override
	{
#if 1

		auto add_close_personal_window = [this](bool& p_open)
		{
			chaos::ImGuiDrawableInterface::MenuBar(chaos::ImGuiDrawMenuMode::FullWindow, [this, &p_open]()
			{
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::Separator();
					if (ImGui::BeginMenu("personals"))
					{
						ImGui::MenuItem("main", nullptr, &p_open, true); // whenever the user select the menu item, the p_open value is being toggled
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
			});
		};


		// ---------------------------------------------------------------------------------------------
		if (id == 0)
		{
			static bool show_personal_window = true;
			add_close_personal_window(show_personal_window);


			if (show_personal_window)
			{
				ImGui::Begin("##truc1", &show_personal_window, ImGuiWindowFlags_NoCollapse);

				if (ImGui::CollapsingHeader("Help"))
				{
					ImGui::Text("Hello");
					ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
					ImGui::Text("Hello");
				}
				//ImGui::Dummy({100.0f, 100.0f});
				ImGui::Indent();
				if (ImGui::CollapsingHeader("Help 2"))
				{
					ImGui::Text("Hello 2");
					ImGui::Text("Hello 2");
				}


				if (ImGui::CollapsingHeader("Help 3"))
				{
					static bool mycheck = false;

					static int mycheckflag = 0;

					ImGui::Text("Hello");
					ImGui::CheckboxFlags("CheckboxFlags 1", &mycheckflag, 1);
					ImGui::CheckboxFlags("CheckboxFlags 6", &mycheckflag, 6);
					ImGui::ArrowButton("Arrow", ImGuiDir_Right);
					ImGui::Checkbox("Checkbox", &mycheck);
					ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
					ImGui::Text("Hello");
				}
				ImGui::Unindent();


				if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_TabListPopupButton))
				{
					static bool tb1 = true;
					static bool tb2 = true;
					static bool tb3 = true;

					if (ImGui::BeginTabItem("tab1", &tb1, ImGuiTabItemFlags_UnsavedDocument))
					{
						ImGui::Text("tab1");
						ImGui::Text("tab1");

						if (ImGui::Button("Stacked Popup"))
							ImGui::OpenPopup("another popup");

						static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
						ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 180.0f));


						if (ImGui::BeginListBox("List"))
						{
							static bool b1 = false;
							static bool b2 = false;
							static bool b3 = false;
							ImGui::PushID(0);
							ImGui::Selectable("Selected", &b1);
							ImGui::PopID();
							ImGui::PushID(1);
							ImGui::Selectable("Not Selected", &b2);
							ImGui::PopID();
							ImGui::PushID(2);
							ImGui::Selectable("Not Selected", &b3);
							ImGui::PopID();
							ImGui::EndListBox();
						}

						if (ImGui::TreeNode("truc"))
						{
							if (ImGui::TreeNode("machin"))
							{
								ImGui::Text("YYYYYYYYYY");
								ImGui::TreePop();
							}
							ImGui::TreePop();
						}
						if (ImGui::TreeNode("truc"))
						{
							if (ImGui::TreeNode("machin"))
							{
								ImGui::Text("YYYYYYYYYY");
								ImGui::TreePop();
							}
							ImGui::TreePop();
						}

						bool unused_open = true;
						if (ImGui::BeginPopupModal("another popup"))
						{

							ImGui::Text("I am the last one here.");
							if (ImGui::Button("Close XXX"))
								ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}


						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("tab2", &tb2, ImGuiTabItemFlags_Trailing))
					{
						ImGui::Text("tab2");
						ImGui::Text("tab2");
						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("tab3", &tb3, ImGuiTabItemFlags_Leading))
					{
						ImGui::Text("tab3");
						ImGui::Text("tab3");

						if (ImGui::CollapsingHeader("Window options"))
						{
							if (ImGui::BeginTable("split", 3))
							{
								static bool op1 = false;
								static bool op2 = false;
								static bool op3 = false;
								static bool op4 = false;
								static bool op5 = false;

								ImGui::TableNextColumn(); ImGui::Checkbox("op1", &op1);
								if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
								{
									//ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted("desc");
									//ImGui::PopTextWrapPos();
									ImGui::EndTooltip();
								}
								ImGui::TableNextColumn(); ImGui::Checkbox("op2", &op2);
								ImGui::TableNextColumn(); ImGui::Checkbox("op3", &op3);
								ImGui::TableNextColumn(); ImGui::Checkbox("op4", &op4);
								ImGui::TableNextColumn(); ImGui::Checkbox("op5", &op5);
								ImGui::EndTable();
							}

							//ImGui::SameLine();
							ImGui::Button("Button 1"); ImGui::SameLine();
							ImGui::Button("Button 2"); ImGui::SameLine();
							ImGui::Button("Button 3");
							ImGui::Button("Button 4"); ImGui::SameLine();
							ImGui::SmallButton("Button 5");

							static int counter = 0;
							ImGui::PushButtonRepeat(true);
							if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
							ImGui::SameLine();

							if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
							ImGui::SameLine();

							ImGui::Text("%d", counter);
							ImGui::PopButtonRepeat();

							static int current_combo_item = 0;
							char const* combo_items[] =
							{
								"item1",
								"item2",
								"item3"
							};

							ImGui::Combo("mycombo", &current_combo_item, combo_items, 3);

							static ImGuiTextFilter filter;
							ImGui::Text("Filter usage:\n"
								"  \"\"         display all lines\n"
								"  \"xxx\"      display lines containing \"xxx\"\n"
								"  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
								"  \"-xxx\"     hide lines containing \"xxx\"");
							filter.Draw();
							const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
							for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
								if (filter.PassFilter(lines[i]))
									ImGui::BulletText("%s", lines[i]);


							static float myfloat = 500.0f;
							ImGui::DragFloat("Myfloatdrag", &myfloat);


							ImGui::Dummy({ 200.0f, 200.0f });
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));

							ImGuiIO& io = ImGui::GetIO();
							ImTextureID my_tex_id = io.Fonts->TexID;
							float my_tex_w = (float)io.Fonts->TexWidth;
							float my_tex_h = (float)io.Fonts->TexHeight;
							ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
							ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
							ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h * 4), uv_min, uv_max); // , tint_col, border_col);



						}


						ImGui::EndTabItem();
					}




					ImGui::EndTabBar();
				}

				ImGui::End();
			}
		}



#endif











#if 1
		// ---------------------------------------------------------------------------------------------
		if (id == 1)
		{
			static bool show_personal_window = true;
			add_close_personal_window(show_personal_window);

			if (show_personal_window)
			{
				ImGui::Begin("##truc2", &show_personal_window);
				ImGui::Text("machin");
				ImGui::SeparatorText("General");

				ImGui::Text("machin %d", show_personal_window);
				ImGui::End();
			}
		}

		// ---------------------------------------------------------------------------------------------
		if (id == 2)
		{
			static bool show_personal_window = true;
			add_close_personal_window(show_personal_window);

			if (show_personal_window)
			{
				ImGui::Begin("##truc3", &show_personal_window);
				ImGui::Text("bidule");

				ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;

				ImGui::BeginChild("ChildLX", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), true, window_flags);
				for (int i = 0; i < 100; i++)
					ImGui::Text("%04d: scrollable region", i);
				ImGui::EndChild();

				ImGui::SameLine();

				ImGui::BeginChild("ChildLY", ImVec2(ImGui::GetContentRegionAvail().x, 260), true, window_flags);
				for (int i = 0; i < 100; i++)
					ImGui::Text("%04d: truc scrollable region", i);
				ImGui::EndChild();






				ImGui::End();
			}
		}
#endif
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
	chaos::WindowPlacementInfo placement_info;
	placement_info.size = { 1200, 500 };

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, placement_info);
}
