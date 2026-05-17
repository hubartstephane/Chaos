#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	Window * ImGuiApplication::CreateMainWindow()
	{
#if 0
		if (Window* result = WindowApplication::CreateMainWindow())
		{		
			if (ImGuiWindow* imgui_window = auto_cast(result))
			{
				if (ImGuiObject* imgui_object = GetImGuiApplicationData()->imgui_object_class.CreateInstance())
					imgui_window->SetContent(imgui_object);
			}
			return result;
		}
#endif
		return nullptr;
	}

	bool ImGuiApplication::IsApplicationDataValid(ApplicationData const * in_application_data) const
	{
		ImGuiApplicationData const* imgui_application_data = auto_cast(in_application_data);
		if (imgui_application_data == nullptr)
		{
			ApplicationLog::Error("ImGuiApplication requires [ImGuiApplicationData] class");
			return false;
		}
		return true;
	}

	ImGuiApplicationData const* ImGuiApplication::GetImGuiApplicationData() const
	{
		return auto_cast(application_data);
	}

}; // namespace chaos
