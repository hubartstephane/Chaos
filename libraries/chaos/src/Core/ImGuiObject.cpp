#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumBitmaskMetaData<ImGuiDrawFlags> const ImGuiDrawFlags_bitmask_metadata =
	{
		{ImGuiDrawFlags::FULL_WINDOW, ImGuiDrawFlags::FLOATING_IMGUI_WINDOW}
	};

	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(ImGuiDrawFlags, &ImGuiDrawFlags_bitmask_metadata, CHAOS_API);

	void ImGuiObject::OnDrawImGuiMenuConditional(ImGuiDrawFlags flags, int imgui_window_flags)
	{
		// check wether the application accepts the menu bar
		if (WindowApplication* window_application = Application::GetInstance())
			if (!window_application->IsImGuiMenuEnabled())
				return;

		// display full window menu bar
		if (HasAnyFlags(flags, ImGuiDrawFlags::USE_FULL_WINDOW_MENU))
		{
			OnDrawImGuiMenu(ImGuiTools::BeginMainMenuBar());
		}
		// display floating window menu bar (default)
		else
		{
			if ((imgui_window_flags & ImGuiWindowFlags_MenuBar) != 0)
			{
				OnDrawImGuiMenu(ImGuiTools::BeginMenuBar());
			}
		}
	}

	int ImGuiObject::UpdateWindowFlagsForMenu(ImGuiDrawFlags flags, int imgui_window_flags)
	{
		// check whether application accepts menu
		if (WindowApplication* window_application = Application::GetInstance())
			if (!window_application->IsImGuiMenuEnabled())
				return imgui_window_flags & ~ImGuiWindowFlags_MenuBar;

		// check whether the menu is to be plugged into full window
		if (HasAnyFlags(flags, ImGuiDrawFlags::USE_FULL_WINDOW_MENU))
			return imgui_window_flags & ~ImGuiWindowFlags_MenuBar;

		// request if there is really some menu content
		bool menu_required = false;
		auto fake_begin_menu_func = [&menu_required](LightweightFunction<void()> func)
		{
			menu_required = true; // this implementation fully ignore the "func" content. we just check whether OnDrawImGuiMenu calls "fake_begin_menu_func"
		};
		OnDrawImGuiMenu(fake_begin_menu_func);

		// add or remove menu bar flag
		if (menu_required)
			return imgui_window_flags | ImGuiWindowFlags_MenuBar;
		else
			return imgui_window_flags & ~ImGuiWindowFlags_MenuBar;
	}

	void ImGuiObject::DrawImGui(char const * title, ImGuiDrawFlags flags)
	{
		// check flags coherency
		assert(
			(!HasAnyFlags(flags, ImGuiDrawFlags::FULL_WINDOW)) ||
			(!HasAnyFlags(flags, ImGuiDrawFlags::FLOATING_IMGUI_WINDOW))
		);

		// get ImGui window flags
		int imgui_window_flags = GetImGuiWindowFlags();
		imgui_window_flags = UpdateWindowFlagsForMenu(flags, imgui_window_flags);

		// display content + menu delegate
		auto display_func = [this, flags, imgui_window_flags]()
		{
			OnDrawImGuiContent();
			OnDrawImGuiMenuConditional(flags, imgui_window_flags);
		};

		// display fullscreen window
		if (HasAnyFlags(flags, ImGuiDrawFlags::FULL_WINDOW))
		{
			closing_request = !FullscreenWindow(title, imgui_window_flags, display_func);
		}
		// display floating window (default case)
		else
		{
			closing_request = !FloatingWindow(title, imgui_window_flags, display_func);
		}
	}

	bool ImGuiObject::FullscreenWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func)
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

	bool ImGuiObject::FloatingWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func)
	{
		bool open_value = true;
		if (ImGui::Begin(title, &open_value, imgui_window_flags))
		{
			content_func();
			ImGui::End();
		}
		return open_value;
	}

	int ImGuiObject::GetImGuiWindowFlags() const
	{
		return ImGuiWindowFlags_NoCollapse;
	}

	void ImGuiObject::OnDrawImGuiContent()
	{
	}

	void ImGuiObject::OnDrawImGuiMenu(BeginImGuiMenuFunc begin_menu_func)
	{
	}

	void ImGuiObject::RequestClosing()
	{
		closing_request = true;
	}

	bool ImGuiObject::IsClosingRequested() const
	{
		return closing_request;
	}

}; // namespace chaos