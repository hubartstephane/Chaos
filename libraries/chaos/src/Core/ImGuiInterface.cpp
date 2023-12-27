#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_FLAG_METHOD(ImGuiDrawFlags);

	void ImGuiInterface::ConditionalMenuBar(ImGuiDrawFlags flags, int imgui_window_flags, LightweightFunction<void()> func)
	{
		// check wether the application accepts the menu bar
		if (WindowApplication* window_application = Application::GetInstance())
			if (!window_application->GetImGuiMenuMode())
				return;

		// display full window menu bar
		if ((flags & ImGuiDrawFlags::UseFullWindowMenu) != ImGuiDrawFlags::None)
		{
			if (ImGui::BeginMainMenuBar())
			{
				func();
				ImGui::EndMainMenuBar();
			}
		}
		// display floating window menu bar
		else
		{
			if ((imgui_window_flags & ImGuiWindowFlags_MenuBar) != 0)
			{
				if (ImGui::BeginMenuBar())
				{
					func();
					ImGui::EndMenuBar();
				}
			}
		}
	}

	int ImGuiInterface::UpdateWindowFlagsForMenu(ImGuiDrawFlags flags, int imgui_window_flags) const
	{
		// check whether application accepts menu
		if (WindowApplication* window_application = Application::GetInstance())
			if (!window_application->GetImGuiMenuMode())
				return imgui_window_flags & ~ImGuiWindowFlags_MenuBar;

		// check whether the menu is to be plugged into full window
		if ((flags & ImGuiDrawFlags::UseFullWindowMenu) != ImGuiDrawFlags::None)
			return imgui_window_flags & ~ImGuiWindowFlags_MenuBar;

		return imgui_window_flags;
	}

	void ImGuiInterface::DrawImGui(char const * title, ImGuiDrawFlags flags)
	{
		// check flags coherency
		assert(
			((ImGuiDrawFlags::FullWindow & flags) == ImGuiDrawFlags::None) ||
			((ImGuiDrawFlags::FloatingImGuiWindow & flags) == ImGuiDrawFlags::None)
		);

		// get ImGui window flags
		int imgui_window_flags = GetImGuiWindowFlags();
		imgui_window_flags = UpdateWindowFlagsForMenu(flags, imgui_window_flags);

		// display content + menu delegate
		auto DisplayFunc = [this, flags, imgui_window_flags]()
		{
			OnDrawImGuiContent(flags);

			ConditionalMenuBar(flags, imgui_window_flags, [this, flags]()
			{
				OnDrawImGuiMenu(flags);
			});
		};

		// display fullscreen window
		if ((flags & ImGuiDrawFlags::FullWindow) != ImGuiDrawFlags::None)
		{
			closing_request = FullscreenWindow(title, imgui_window_flags, DisplayFunc);
		}
		// display floating window
		else
		{
			closing_request = FloatingWindow(title, imgui_window_flags, DisplayFunc);
		}
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

	int ImGuiInterface::GetImGuiWindowFlags() const
	{
		return ImGuiWindowFlags_NoCollapse; // XXX: do not forget to add ImGuiWindowFlags_MenuBar if you want to have a menu
	}

	void ImGuiInterface::OnDrawImGuiContent(ImGuiDrawFlags flags)
	{
	}

	void ImGuiInterface::OnDrawImGuiMenu(ImGuiDrawFlags flags)
	{
	}

	void ImGuiInterface::RequestClosing()
	{
		closing_request = true;
	}

	bool ImGuiInterface::IsClosingRequested() const
	{
		return closing_request;
	}

}; // namespace chaos