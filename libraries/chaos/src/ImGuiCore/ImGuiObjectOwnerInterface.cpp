#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	AutoCastable<ImGuiObject> ImGuiObjectOwnerInterface::FindImGuiObject(ObjectRequest request)
	{
		return request.FindObject(imgui_objects);
	}

	AutoConstCastable<ImGuiObject> ImGuiObjectOwnerInterface::FindImGuiObject(ObjectRequest request) const
	{
		return request.FindObject(imgui_objects);
	}

	bool ImGuiObjectOwnerInterface::IsKnownImGuiObjectVisible(char const* name) const
	{
		return (FindImGuiObject(name) != nullptr);
	}

	bool ImGuiObjectOwnerInterface::SetKnownImGuiObjectVisibility(char const* name, bool visible)
	{
		return EnumerateKnownImGuiObjects([this, name, visible](char const* imgui_object_name, char const * imgui_menu_path, CreateImGuiObjectFunc create_func)
		{
			// is it the window we are searching?
			if (StringTools::Stricmp(name, imgui_object_name) != 0)
				return false;
			SetImGuiObjectInternalVisibility(visible, imgui_object_name, create_func);
			return true; // stop the search
		});
	}

	bool ImGuiObjectOwnerInterface::EnumerateKnownImGuiObjects(EnumerateKnownImGuiObjectFunc func)
	{
		char const* gpu_menu_path = "GPU";

		if (func("FPS", gpu_menu_path, ImGuiRenderingFPSStatObject::GetStaticClass()))
			return true;

		if (func("Draw calls", gpu_menu_path, ImGuiRenderingDrawCallsStatObject::GetStaticClass()))
			return true;

		if (func("Vertices", gpu_menu_path, ImGuiRenderingVerticesStatObject::GetStaticClass()))
			return true;

		char const* imgui_menu_path = "ImGui";

		if (func("System Information", imgui_menu_path, ImGuiSystemInformationObject::GetStaticClass()))
			return true;

		if (func("Window Information", imgui_menu_path, ImGuiWindowInformationObject::GetStaticClass()))
			return true;

		char const* input_menu_path = "Inputs";

		if (func("Input Actions", input_menu_path, ImGuiInputActionObject::GetStaticClass()))
			return true;

		return false;
	}

	void ImGuiObjectOwnerInterface::SetImGuiObjectInternalVisibility(bool visible, char const* name, CreateImGuiObjectFunc create_func)
	{
		ImGuiObject* existing_imgui_object = FindImGuiObject(name);

		if (visible)
		{
			if (existing_imgui_object == nullptr)
			{
				if (ImGuiObject* imgui_object = create_func())
				{
					imgui_object->SetName(name);
					imgui_object->SetImGuiObjectFlags(ImGuiObjectFlags::FloatingWindow | ImGuiObjectFlags::CloseWithCross);
					AddImGuiObject(imgui_object);
				}
			}
		}
		else
		{
			if (existing_imgui_object != nullptr)
				RemoveImGuiObject(existing_imgui_object);
		}
	}

	void ImGuiObjectOwnerInterface::AddImGuiObject(ImGuiObject* imgui_object)
	{
		assert(imgui_object != nullptr);
		assert(FindImGuiObject(imgui_object->GetName()) == nullptr);

		Window * window = auto_cast(this);
		assert(window != nullptr); // being a Window is mandatory

#if _DEBUG
		// check whether the object is not already in list here
		auto it = std::ranges::find_if(imgui_objects, [imgui_object](shared_ptr<ImGuiObject> const& ptr)
		{
			return (ptr == imgui_object);
		});
		assert(it == imgui_objects.end());
#endif
		assert(!StringTools::IsEmpty(imgui_object->GetName())); // window mush have a name !
		imgui_objects.push_back(imgui_object);
		imgui_object->OnAddedToWindow(window);
	}

	void ImGuiObjectOwnerInterface::RemoveImGuiObject(ImGuiObject* imgui_object)
	{
		assert(imgui_object != nullptr);

		Window * window = auto_cast(this);
		assert(window != nullptr); // being a Window is mandatory
	
		auto it = std::ranges::find_if(imgui_objects, [imgui_object](shared_ptr<ImGuiObject> const& ptr)
		{
			return (ptr == imgui_object);
		});

		if (it != imgui_objects.end())
		{
			shared_ptr<ImGuiObject> keep_alive = imgui_object;
			imgui_objects.erase(it);
			keep_alive->OnRemovedFromWindow(window);
		}
	}

	void ImGuiObjectOwnerInterface::OnDrawImGuiObjectOwnerContent()
	{
		Window * window = auto_cast(this);
		assert(window != nullptr); // being a Window is mandatory

		// create a weak copy of imgui objects to avoid 
		// bugs if some elements are destroyed during the loop
		std::vector<weak_ptr<ImGuiObject>> weak_imgui_objects;
		weak_imgui_objects.reserve(imgui_objects.size());
		for (shared_ptr<ImGuiObject> const& imgui_object : imgui_objects)
			weak_imgui_objects.emplace_back(imgui_object.get());

		// draw all imgui objects
		for (weak_ptr<ImGuiObject> imgui_object : weak_imgui_objects)
		{
			if (imgui_object != nullptr)
			{
				shared_ptr<ImGuiObject> prevent_destruction = imgui_object;
				imgui_object->DrawImGui(window);
				if (imgui_object->IsClosingRequested()) // closing requested ?
					RemoveImGuiObject(imgui_object.get());
			}
		}
	}

	void ImGuiObjectOwnerInterface::OnDrawImGuiObjectOwnerMenu(ImGuiMenuBuilder const & menu_builder)
	{
		EnumerateKnownImGuiObjects([this, &menu_builder](char const* name, char const * imgui_menu_path, CreateImGuiObjectFunc create_func)
		{
			if (imgui_menu_path != nullptr)
			{
				menu_builder.WithMenu([this, imgui_menu_path , &create_func, name]()
				{
					if (ImGui::BeginMenu(imgui_menu_path))
					{
						bool imgui_object_exists = (FindImGuiObject(name) != nullptr);
						if (ImGui::MenuItem(name, nullptr, imgui_object_exists, true))
							SetImGuiObjectInternalVisibility(!imgui_object_exists, name, create_func);
						ImGui::EndMenu();
					}
				});
			}
			return false; // don't stop the loop
		});
	}

}; // namespace chaos