#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiWindow::ImGuiWindow()
	{
		SetWindowCategory(WindowCategory::OptionalWindow);
		SetStandardImGuiMenuPluggedIn(false);
	}

	void ImGuiWindow::SetContent(ImGuiObject* in_content)
	{
		if (content != in_content)
		{
			if (content != nullptr)
			{
				RemoveImGuiObject(content.get());
			}
			content = in_content;
			if (content != nullptr)
			{
				content->SetName("content");
				content->SetImGuiObjectFlags(ImGuiObjectFlags::FullViewport);
				AddImGuiObject(content.get());
			}
		}
	}

	void ImGuiWindow::DrawImGui()
	{
		Window::DrawImGui();
		// check whether the window as to be closed
		if (content != nullptr)
			if (content->IsClosingRequested())
				RequireWindowClosure();
	}

	bool ImGuiWindow::EnumerateKnownImGuiObjects(EnumerateKnownImGuiObjectFunc func)
	{
		return false; // don't call super method
	}

	bool ImGuiWindow::OnInitialize(JSONReadConfiguration config)
	{
		if (!Window::OnInitialize(config))
			return false;

		if (ImGuiWindowData const* imgui_window_data = GetImGuiWindowData())
		{
			ImGuiObject* imgui_object = imgui_window_data->imgui_object_class->CreateInstance();
			if (imgui_object == nullptr)
				return false;
			SetContent(imgui_object);	
		}

		return true;
	}

	bool ImGuiWindow::IsWindowDataValid(WindowData const* in_window_data) const
	{
		if (in_window_data == nullptr) // accept empty window data
			return true;

		ImGuiWindowData const * imgui_window_data = auto_cast(in_window_data);
		if (imgui_window_data == nullptr)
		{
			ApplicationLog::Error("ImGuiWindow requires [ImGuiWindowData] class");
			return false;
		}
		return true;
	}

	ImGuiWindowData const * ImGuiWindow::GetImGuiWindowData() const
	{
		return auto_cast(window_data);
	}

}; // namespace chaos
