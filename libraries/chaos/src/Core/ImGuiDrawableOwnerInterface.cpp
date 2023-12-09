#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	/**
	 * ImGuiDrawableObjectRegistration implementation
	 */

	ImGuiDrawableObjectRegistration::~ImGuiDrawableObjectRegistration()
	{
		assert(owner == nullptr);
	}

	void ImGuiDrawableObjectRegistration::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		if (drawable_object != nullptr)
		{
			drawable_object->DrawImGui(menu_mode);
			if (drawable_object->IsClosingRequested()) // transfert closing request
				RequestClosing();
		}
	}

	bool ImGuiDrawableObjectRegistration::IsVisible() const
	{
		return visible;
	}

	void ImGuiDrawableObjectRegistration::Show(bool show)
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

	char const* ImGuiDrawableObjectRegistration::GetName() const
	{
		return name.c_str();
	}

	void ImGuiDrawableObjectRegistration::SubReference()
	{
		// the registration does not belong to any owner: handle as usual
		if (owner == nullptr)
			Object::SubReference();
		// the last reference is the one from the owner. Destroy it
		else if (--shared_count == 1)
			owner->UnregisterDrawable(this);
	}

	/**
	 * ImGuiDrawableOwnerInterface implementation
	 */

	ImGuiDrawableOwnerInterface::~ImGuiDrawableOwnerInterface()
	{
		while (registered_drawable.size() > 0)
		{
			ImGuiDrawableObjectRegistration* last = registered_drawable[0].get();
			last->owner = nullptr;
			registered_drawable.pop_back();
		}
	}

	ImGuiDrawableObjectRegistration* ImGuiDrawableOwnerInterface::FindRegisteredDrawable(char const* name)
	{
		for (auto& drawable : registered_drawable)
			if (StringTools::Stricmp(name, drawable->GetName()) == 0)
				return drawable.get();
		return nullptr;
	}

	void ImGuiDrawableOwnerInterface::UnregisterDrawable(ImGuiDrawableObjectRegistration* registration)
	{
		auto it = std::ranges::find_if(registered_drawable, [registration](auto& drawable) { return drawable == registration; });
		if (it != std::ranges::end(registered_drawable))
		{
			registration->owner = nullptr; // order is important, next line may destroy it
			registered_drawable.erase(it);
		}
	}

	ImGuiDrawableObjectRegistration * ImGuiDrawableOwnerInterface::RegisterDrawable(char const* name, std::function<ImGuiDrawableObject* ()> const& drawable_creation_function)
	{
		if (ImGuiDrawableObjectRegistration* registration = FindRegisteredDrawable(name))
		{
			Log::Warning("ImGuiDrawableOwnerInterface::RegisteredDrawable: %s already existing", name);
		}
		else if (ImGuiDrawableObjectRegistration* result = new ImGuiDrawableObjectRegistration())
		{
			result->name = name;
			result->creation_function = drawable_creation_function;
			result->owner = this;
			registered_drawable.push_back(result);
			return result;
		}
		return nullptr;
	}

	ImGuiDrawableObjectRegistration * ImGuiDrawableOwnerInterface::RegisterDrawable(char const* name, SubClassOf<ImGuiDrawableObject> drawable_class)
	{
		return RegisterDrawable(name, [drawable_class]()
		{
			return drawable_class.CreateInstance();
		});
	}

	void ImGuiDrawableOwnerInterface::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		if (registered_drawable.size() > 0)
		{
			// draw the menu
			chaos::ImGuiDrawableInterface::MenuBar(chaos::ImGuiDrawMenuMode::FullWindow, [this]()
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