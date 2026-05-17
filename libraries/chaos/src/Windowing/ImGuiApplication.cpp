#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	Window * ImGuiApplication::CreateMainWindow()
	{
		ImGuiApplicationData const * imgui_application_data = auto_cast(application_data);
		if (imgui_application_data == nullptr)
			return nullptr;

		if (Window* result = WindowApplication::CreateMainWindow())
		{		
			if (ImGuiWindow* imgui_window = auto_cast(result))
			{
				if (ImGuiObject* imgui_object = imgui_application_data->imgui_object_class.CreateInstance())
					imgui_window->SetContent(imgui_object);
			}
			return result;
		}
		return nullptr;
	}

	bool ImGuiApplication::IsApplicationDataValid(ApplicationData const * in_application_data) const
	{
		ImGuiApplicationData const* imgui_application_data = auto_cast(in_application_data);
		if (imgui_application_data == nullptr)
		{
			ApplicationLog::Error("Application requires [ImGuiApplicationData] class");
			return false;
		}
		return true;
	}

}; // namespace chaos
