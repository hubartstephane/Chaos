#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void DrawImGui(bool& value)
	{
		ImGuiTools::PushID(&value);
		ImGui::Checkbox("", &value);
		ImGui::PopID();
	}

	void DrawImGui(int& value)
	{
		ImGuiTools::PushID(&value);
		ImGui::InputInt("", &value);
		ImGui::PopID();
	}

	void DrawImGui(float& value)
	{
		ImGuiTools::PushID(&value);
		ImGui::InputFloat("", &value);
		ImGui::PopID();
	}

	void DrawImGui(double& value)
	{
		ImGuiTools::PushID(&value);
		ImGui::InputDouble("", &value);
		ImGui::PopID();
	}

	namespace ImGuiTools
	{
		void PushID(void* ptr) // native ImGui provides PushID(char *) and PushID(void *). Using this ImGuiTools method prevent ambiguity. If a char * pointer was given as parameter, the content
		{                      // would not be hashed, only the pointer
			ImGui::PushID(ptr);
		}

	}; // namespace ImGuiTools


}; // namespace chaos