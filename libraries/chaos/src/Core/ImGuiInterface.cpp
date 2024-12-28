#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumBitmaskMetaData<ImGuiDrawFlags> const ImGuiDrawFlags_bitmask_metadata =
	{
		{ImGuiDrawFlags::FULL_WINDOW, ImGuiDrawFlags::FLOATING_IMGUI_WINDOW}
	};

	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(ImGuiDrawFlags, &ImGuiDrawFlags_bitmask_metadata, CHAOS_API);

	void ImGuiInterface::DrawImGui(Window* window)
	{
		assert(0);
	}

	void ImGuiInterface::OnDrawImGuiContent(Window* window)
	{
	}

	void ImGuiInterface::OnDrawImGuiMenu(Window* window, BeginImGuiMenuFunc begin_menu_func)
	{
	}

	bool ImGuiInterface::FullscreenWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func)
	{
		// update flags
		imgui_window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;

		// prepare ImGui window size and position
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x, ImGui::GetMainViewport()->WorkPos.y });
		ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y });
		// display then window
		if (ImGui::Begin(title, nullptr, imgui_window_flags))
		{
			content_func();
			ImGui::End();
		}
		return true; // window must be kept alive
	}

	bool ImGuiInterface::FloatingWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func)
	{
		bool open_value = true;
		if (ImGui::Begin(title, &open_value, imgui_window_flags))
		{
			content_func();
			ImGui::End();
		}
		return open_value;
	}

}; // namespace chaos