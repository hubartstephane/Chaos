#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiObject::OnDrawImGuiMenuConditional(Window* window, int imgui_window_flags)
	{
		// check wether the application accepts the menu bar
		if (!WindowApplication::IsImGuiMenuEnabled())
			return;

		// display full window menu bar
		if (HasAnyFlags(draw_flags, ImGuiDrawFlags::USE_FULL_WINDOW_MENU))
		{
			OnDrawImGuiMenu(window, ImGuiTools::BeginMainMenuBar());
		}
		// display floating window menu bar (default)
		else
		{
			if ((imgui_window_flags & ImGuiWindowFlags_MenuBar) != 0)
			{
				OnDrawImGuiMenu(window, ImGuiTools::BeginMenuBar());
			}
		}
	}

	int ImGuiObject::UpdateWindowFlagsForMenu(Window* window, int imgui_window_flags)
	{
		// check whether application accepts menu
		if (!WindowApplication::IsImGuiMenuEnabled())
			return imgui_window_flags & ~ImGuiWindowFlags_MenuBar;

		// check whether the menu is to be plugged into full window
		if (HasAnyFlags(draw_flags, ImGuiDrawFlags::USE_FULL_WINDOW_MENU))
			return imgui_window_flags & ~ImGuiWindowFlags_MenuBar;

		// request if there is really some menu content
		bool menu_required = false;
		auto fake_begin_menu_func = [&menu_required](LightweightFunction<void()> func)
		{
			menu_required = true; // this implementation fully ignore the "func" content. we just check whether OnDrawImGuiMenu calls "fake_begin_menu_func"
		};
		OnDrawImGuiMenu(window, fake_begin_menu_func);

		// add or remove menu bar flag
		if (menu_required)
			return imgui_window_flags | ImGuiWindowFlags_MenuBar;
		else
			return imgui_window_flags & ~ImGuiWindowFlags_MenuBar;
	}

	void ImGuiObject::SetDrawFlags(ImGuiDrawFlags in_flags)
	{
		// check flags coherency
		assert(
			(!HasAnyFlags(in_flags, ImGuiDrawFlags::FULL_WINDOW)) ||
			(!HasAnyFlags(in_flags, ImGuiDrawFlags::FLOATING_IMGUI_WINDOW))
		);
		draw_flags = in_flags;
	}

	void ImGuiObject::DrawImGui(Window* window)
	{
		// get ImGui window flags
		int imgui_window_flags = GetImGuiWindowFlags();
		imgui_window_flags = UpdateWindowFlagsForMenu(window, imgui_window_flags);

		// display content + menu delegate
		auto display_func = [this, imgui_window_flags, window]()
		{
			OnDrawImGuiContent(window);
			OnDrawImGuiMenuConditional(window, imgui_window_flags);
		};

		// display fullscreen window
		bool keep_alive = true;
		if (HasAnyFlags(draw_flags, ImGuiDrawFlags::FULL_WINDOW))
		{
			keep_alive = FullscreenWindow(GetName(), imgui_window_flags, display_func);
		}
		// display floating window (default case)
		else
		{
			keep_alive = FloatingWindow(GetName(), imgui_window_flags, display_func);
		}
		// request for closing if necessary
		if (!keep_alive)
			RequestClosing();
	}

	int ImGuiObject::GetImGuiWindowFlags() const
	{
		return ImGuiWindowFlags_NoCollapse;
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