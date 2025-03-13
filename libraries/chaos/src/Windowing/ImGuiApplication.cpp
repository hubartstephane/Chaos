#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiApplication::ImGuiApplication(ImGuiObject* in_imgui_object, SubClassOf<ImGuiWindow> in_main_window_class, WindowPlacementInfo const& in_main_window_placement_info, WindowCreateParams const& in_main_window_create_params):
		WindowApplication(in_main_window_class, in_main_window_placement_info, in_main_window_create_params),
		imgui_object(in_imgui_object)
	{
		assert(in_imgui_object != nullptr);
	}

	Window* ImGuiApplication::DoCreateMainWindow(WindowCreateParams const& create_params)
	{
		if (Window* result = CreateTypedWindow(main_window_class, main_window_placement_info, create_params, "main_window"))
		{
			if (ImGuiWindow* imgui_window = auto_cast(result))
			{
				imgui_window->SetContent(imgui_object.get());
				imgui_window->SetWindowCategory(WindowCategory::MAIN_WINDOW);
			}
			return result;
		}
		return nullptr;
	}

}; // namespace chaos
