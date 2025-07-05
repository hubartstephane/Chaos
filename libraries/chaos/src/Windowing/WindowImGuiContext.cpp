#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	WindowImGuiContext::WindowImGuiContext(Window * in_window):
		window(in_window)
	{
		assert(in_window != nullptr);
	}

	WindowImGuiContext::~WindowImGuiContext()
	{
		assert(imgui_context == nullptr);
		assert(implot_context == nullptr);
	}

	bool WindowImGuiContext::IsImGuiContextCurrent() const
	{
		return
			(imgui_context != nullptr) &&
			(ImGui::GetCurrentContext() == imgui_context);
	}

	ImGuiManager* WindowImGuiContext::GetImGuiManager() const
	{
		if (WindowApplication* window_application = Application::GetInstance())
			return window_application->GetImGuiManager();
		return nullptr;
	}

	void WindowImGuiContext::OnImGuiMenuEnabledChanged(bool enabled)
	{
		GetImGuiManager()->OnImGuiMenuEnabledChanged(imgui_context, enabled);
	}

	bool WindowImGuiContext::ShouldCaptureInputEvent() const
	{
		if (IsImGuiContextCurrent() && WindowApplication::IsImGuiMenuEnabled())
			return true;
		return false;
	}

	void WindowImGuiContext::OnMonitorEvent(GLFWmonitor* monitor, int monitor_state)
	{
		if (imgui_context != nullptr)
			ImGui_ImplGlfw_MonitorCallback(monitor, monitor_state); // manually call ImGui delegate
	}

	bool WindowImGuiContext::OnMouseMoveImpl(glm::vec2 const& delta)
	{
		if (ShouldCaptureInputEvent())
		{
			ImGui_ImplGlfw_CursorPosCallback(window->GetGLFWHandler(), delta.x, delta.y);

			if (ImGui::GetIO().WantCaptureMouse)
				return true;
		}
		return false;
	}

	bool WindowImGuiContext::OnMouseButtonImpl(MouseButtonEvent const &mouse_button_event)
	{
		if (ShouldCaptureInputEvent())
		{
			ImGui_ImplGlfw_MouseButtonCallback(window->GetGLFWHandler(), mouse_button_event.button, mouse_button_event.action, mouse_button_event.modifier);

			if (ImGui::GetIO().WantCaptureMouse)
				return true;
		}
		return false;
	}

	bool WindowImGuiContext::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		if (ShouldCaptureInputEvent())
		{
			ImGui_ImplGlfw_ScrollCallback(window->GetGLFWHandler(), scroll_x, scroll_y);

			if (ImGui::GetIO().WantCaptureMouse)
				return true;
		}
		return false;
	}

	bool WindowImGuiContext::OnKeyEventImpl(KeyEvent const& key_event)
	{
		if (ShouldCaptureInputEvent())
		{
			ImGui_ImplGlfw_KeyCallback(window->GetGLFWHandler(), (int)key_event.button, key_event.scancode, key_event.action, key_event.modifier);

			if (ImGui::GetIO().WantCaptureKeyboard)
				return true;
		}
		return false;
	}

	bool WindowImGuiContext::OnCharEventImpl(unsigned int c)
	{
		if (ShouldCaptureInputEvent())
		{
			ImGui_ImplGlfw_CharCallback(window->GetGLFWHandler(), c);

			ImGuiIO& io = ImGui::GetIO();
			if (ImGui::GetIO().WantCaptureKeyboard)
				return true;
		}
		return false;
	}

	void WindowImGuiContext::OnDrawImGuiMenu(Window* window, BeginImGuiMenuFunc begin_menu_func)
	{
		begin_menu_func([this]()
		{
			if (ImGui::BeginMenu("Managers"))
			{
				if (ImGui::BeginMenu("ImGui"))
				{
					if (WindowApplication* window_application = Application::GetInstance())
					{
						bool atlas_viewer_exists = (window_application->FindWindow("ImGuiAtlasViewer") != nullptr); // search the atlas viewer
						if (ImGui::MenuItem("Show atlas", nullptr, atlas_viewer_exists))
							window_application->CreateOrDestroyWindow(!atlas_viewer_exists, "ImGuiAtlasViewer", &ImGuiWindow::CreateImGuiWindow<ImGuiAtlasObject>); // create or destroy the atlas
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
		});
	}

	bool WindowImGuiContext::CreateContext()
	{
		ImGuiManager* imgui_manager = GetImGuiManager();

		// save imgui/implot contexts
		ImGuiContext * previous_imgui_context  = ImGui::GetCurrentContext();
		ImPlotContext* previous_implot_context = ImPlot::GetCurrentContext();

		// create a new context for this window
		imgui_context = ImGui::CreateContext(imgui_manager->BuildFontAtlas());
		ImGui::SetCurrentContext(imgui_context);

		imgui_manager->InitializeImGuiContext(imgui_context);

		// initialize the context
		//
		// note: we don't install callbacks. we will manually call the functions ourselves
		//       under normal circumstances, we have the following call chain
		//
		//       Window Message Handler (or NOT) -> GLFW call -> IMGUI callbacks -> our callbacks
		//                ^
		//                +-- settings the IMGUI context here is not enought
		//
		//       due to our multiple-window system, inside the IMGUI callbacks, the current IMGUI context may not be set causing a crash
		//       -> for example, push both LEFT and RIGHT shift and release one of theses keys
		//
		//       we can change the Window Message Function to ensure the IMG context is properly set, but some time the GLFW call is not made from inside the Window Message Handler
		//
		//       By not installing IMGUI callbacks we have the following chain
		//
		//       Window Message Handler or (NOT) -> GLFW call -> our callbacks -> set IMGUI context -> manually call IMGUI callbacks
		//
		//       see in ImGui_ImplGlfw_InstallCallbacks (IMGUI source code) which callback are concerned

		ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWHandler(), false); // do not install callbacks !!
		ImGui_ImplOpenGL3_Init("#version 130");

		// create a new context for implot
		implot_context = ImPlot::CreateContext();

		// restore previous imgui/implot contexts
		ImPlot::SetCurrentContext(previous_implot_context);
		ImGui::SetCurrentContext(previous_imgui_context);

		return true;
	}

	void WindowImGuiContext::DestroyContext()
	{
		// destroy implot context
		if (implot_context != nullptr)
		{
			ImPlotContext* previous_implot_context = ImPlot::GetCurrentContext();

			ImPlot::SetCurrentContext(implot_context);
			ImPlot::DestroyContext(implot_context);
			ImPlot::SetCurrentContext((previous_implot_context != implot_context) ? previous_implot_context : nullptr); // if there was another context, restore it

			implot_context = nullptr;
		}

		// destroy ImGui context (must happen before the windows destruction because some GLFW callbacks rely on the existence of the ImGui context)
		if (imgui_context != nullptr)
		{
			ImGuiContext* previous_imgui_context = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(imgui_context);

			GetImGuiManager()->FinalizeImGuiContext(imgui_context);

			ImGui_ImplGlfw_Shutdown();
			ImGui_ImplOpenGL3_Shutdown();

			ImGui::DestroyContext();
			ImGui::SetCurrentContext((previous_imgui_context != imgui_context) ? previous_imgui_context : nullptr); // if there was another context, restore it

			imgui_context = nullptr;
		}
	}

}; // namespace chaos
