#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace ImGuiTools
	{
		void DrawImGui(bool& value)
		{
			ImGuiTools::PushID(&value);
			ImGui::Checkbox("", &value);
			ImGui::PopID();
		}

		void DrawImGui(float& value)
		{

		}

		void PushID(void* ptr)
		{
			char buffer[17]; // pointers are 64bits, 8 bytes -> %p uses 16 characters (each byte requires 2 characters)(+1 for ending 0)
			memset(buffer, sizeof(buffer), 0);
			_snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "%p", ptr);
			ImGui::PushID(buffer);
		}

	}; // namespace ImGuiTools


}; // namespace chaos