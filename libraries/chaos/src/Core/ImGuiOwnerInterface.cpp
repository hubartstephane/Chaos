#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	/**
	 * ImGuiRegistrationObject implementation
	 */

	ImGuiRegistrationObject::~ImGuiRegistrationObject()
	{
		assert(owner == nullptr);
	}

	void ImGuiRegistrationObject::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		if (drawable_object != nullptr)
		{
			drawable_object->DrawImGui(menu_mode);
			if (drawable_object->IsClosingRequested()) // transfert closing request
				RequestClosing();
		}
	}

	bool ImGuiRegistrationObject::IsVisible() const
	{
		return visible;
	}

	void ImGuiRegistrationObject::Show(bool show)
	{
		if (show)
		{
			if (!visible)
			{
				assert(drawable_object == nullptr);
				drawable_object = creation_function();
			}
		}
		else
		{
			if (visible)
			{
				assert(drawable_object != nullptr);
				drawable_object = nullptr;

			}
		}
		visible = show;
	}

	char const* ImGuiRegistrationObject::GetName() const
	{
		return name.c_str();
	}

	void ImGuiRegistrationObject::SubReference()
	{
		// the registration does not belong to any owner: handle as usual
		if (owner == nullptr)
			Object::SubReference();
		// the last reference is the one from the owner. Destroy it
		else if (--shared_count == 1)
			owner->UnregisterDrawable(this);
	}

	/**
	 * ImGuiOwnerInterface implementation
	 */

	ImGuiOwnerInterface::~ImGuiOwnerInterface()
	{
		while (registered_drawable.size() > 0)
		{
			ImGuiRegistrationObject* last = registered_drawable[0].get();
			last->owner = nullptr;
			registered_drawable.pop_back();
		}
	}

	ImGuiRegistrationObject* ImGuiOwnerInterface::FindRegisteredDrawable(char const* name)
	{
		for (auto& drawable : registered_drawable)
			if (StringTools::Stricmp(name, drawable->GetName()) == 0)
				return drawable.get();
		return nullptr;
	}

	void ImGuiOwnerInterface::UnregisterDrawable(ImGuiRegistrationObject* registration)
	{
		auto it = std::ranges::find_if(registered_drawable, [registration](auto& drawable) { return drawable == registration; });
		if (it != std::ranges::end(registered_drawable))
		{
			registration->owner = nullptr; // order is important, next line may destroy it
			registered_drawable.erase(it);
		}
	}

	ImGuiRegistrationObject * ImGuiOwnerInterface::RegisterDrawable(char const* name, std::function<ImGuiObject* ()> const& drawable_creation_function)
	{
		if (ImGuiRegistrationObject* registration = FindRegisteredDrawable(name))
		{
			Log::Warning("ImGuiOwnerInterface::RegisteredDrawable: %s already existing", name);
		}
		else if (ImGuiRegistrationObject* result = new ImGuiRegistrationObject())
		{
			result->name = name;
			result->creation_function = drawable_creation_function;
			result->owner = this;
			registered_drawable.push_back(result);
			return result;
		}
		return nullptr;
	}

	ImGuiRegistrationObject * ImGuiOwnerInterface::RegisterDrawable(char const* name, SubClassOf<ImGuiObject> drawable_class)
	{
		return RegisterDrawable(name, [drawable_class]()
		{
			return drawable_class.CreateInstance();
		});
	}

	void ImGuiOwnerInterface::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		if (registered_drawable.size() > 0)
		{
			// draw the menu
			chaos::ImGuiInterface::MenuBar(chaos::ImGuiDrawMenuMode::FullWindow, [this]()
			{
				if (ImGui::BeginMenu("Widgets"))
				{
					for (auto& drawable : registered_drawable)
						if (ImGui::MenuItem(drawable->GetName(), nullptr, drawable->IsVisible(), true))
							drawable->Show(!drawable->IsVisible());
					ImGui::EndMenu();
				}
			});

			// draw the drawables
			for (auto& drawable : registered_drawable)
			{
				drawable->DrawImGui(menu_mode);
				if (drawable->IsClosingRequested())
				{
					drawable->Show(false);
					drawable->closing_request = false; // reset the request
				}
			}
		}
	}

}; // namespace chaos