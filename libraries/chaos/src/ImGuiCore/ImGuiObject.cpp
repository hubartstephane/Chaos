#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumTools::EnumBitmaskMetaData<ImGuiObjectFlags> const ImGuiObjectFlags_bitmask_metadata =
	{
		{ ImGuiObjectFlags::FullViewport, ImGuiObjectFlags::FloatingWindow, ImGuiObjectFlags::PopupModalWindow, ImGuiObjectFlags::PopupWindow }
	};

	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(ImGuiObjectFlags, &ImGuiObjectFlags_bitmask_metadata, CHAOS_API);

	void ImGuiObject::OnDrawImGuiMenuConditional(Window* window, int imgui_window_flags)
	{
		// check wether the application accepts the menu bar
		if (!WindowApplication::IsImGuiMenuEnabled())
			return;

		// display full window menu bar
		if (HasAnyFlags(imgui_object_flags, ImGuiObjectFlags::UseViewportMenu))
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
		if (HasAnyFlags(imgui_object_flags, ImGuiObjectFlags::UseViewportMenu))
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

	void ImGuiObject::OnCloseWithEscape()
	{
	}

	void ImGuiObject::OnCloseWithCross()
	{
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

			if (HasAnyFlags(imgui_object_flags, ImGuiObjectFlags::CloseWithEscape))
			{
				if (ImGui::IsWindowFocused())
				{
					if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
					{
						OnCloseWithEscape();
						RequestClosing();
					}
				}
			}
		};

		// display popup (modal and non-modal)
		bool keep_alive = true;
		if (HasAnyFlags(imgui_object_flags, ImGuiObjectFlags::PopupWindow | ImGuiObjectFlags::PopupModalWindow))
		{
			// open the popup
			if (!opened_popup)
			{
				opened_popup = true;
				ImGui::OpenPopup(GetName());
			}

			// prepare the window placement
			if (placement.has_value())
				placement.value().PrepareNextWindowPlacement();

			// display the popup
			bool* keep_alive_ptr = (HasAnyFlags(imgui_object_flags, ImGuiObjectFlags::CloseWithCross)) ? &keep_alive : nullptr;

			bool open_succes = (HasAnyFlags(imgui_object_flags, ImGuiObjectFlags::PopupWindow)) ? // select modal or non modal open function
				ImGui::BeginPopup(GetName(), imgui_window_flags) :
				ImGui::BeginPopupModal(GetName(), keep_alive_ptr, imgui_window_flags);

			if (open_succes)
			{
				display_func();
				ImGui::EndPopup();
			}
			// check whether popup has been closed
			if (!ImGui::IsPopupOpen(GetName()))
				RequestClosing();
			// check whether the cross has been clicked
			if (!keep_alive)
				OnCloseWithCross();
		}
		// display fullscreen window
		else if (HasAnyFlags(imgui_object_flags, ImGuiObjectFlags::FullViewport))
		{
			keep_alive = ImGuiTools::FullViewportWindow(GetName(), imgui_window_flags, display_func);
		}
		// display floating window (default case)
		else
		{
			// prepare the window placement
			if (placement.has_value())
				placement.value().PrepareNextWindowPlacement();

			// display the window
			bool* keep_alive_ptr = (HasAnyFlags(imgui_object_flags, ImGuiObjectFlags::CloseWithCross)) ? &keep_alive : nullptr;
			if (ImGui::Begin(GetName(), keep_alive_ptr, imgui_window_flags))
			{
				display_func();
				ImGui::End();
			}
			// check whether the cross has been clicked
			if (!keep_alive)
				OnCloseWithCross();
		}
		// request for closing if necessary
		if (!keep_alive)
			RequestClosing();
	}

	ImGuiObjectFlags ImGuiObject::GetImGuiObjectFlags() const
	{
		return imgui_object_flags;
	}

	void ImGuiObject::SetImGuiObjectFlags(ImGuiObjectFlags in_flags)
	{
		// check flags coherency
		assert(AreValidFlags(in_flags));
		imgui_object_flags = in_flags;
	}

	void ImGuiObject::SetImGuiWindowFlags(int in_imgui_flags)
	{
		in_imgui_flags = in_imgui_flags;
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

	void ImGuiObject::OnAddedToWindow(Window* in_window)
	{
	}

	void ImGuiObject::OnRemovedFromWindow(Window* in_window)
	{
		if (opened_popup)
		{
			// hack to close the window: call BeginXXX() with a false flag
			bool close_window = false;
			if (HasAnyFlags(imgui_object_flags, ImGuiObjectFlags::PopupModalWindow | ImGuiObjectFlags::PopupWindow))
				if (ImGui::IsPopupOpen(GetName()))
					ImGui::BeginPopupModal(GetName(), &close_window, 0);
			opened_popup = false;
		}
	}

	void ImGuiObject::SetWindowPlacement(std::optional<ImGuiWindowPlacement> in_placement)
	{
		if (!in_placement.has_value())
			placement.reset();
		else
			placement = in_placement.value();
	}

}; // namespace chaos