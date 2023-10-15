#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	/**
	 * ImGuiDrawableObjectRegistration implementation
	 */

	void ImGuiDrawableObjectRegistration::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		if (drawable_object != nullptr)
			drawable_object->DrawImGui(menu_mode);
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

	/**
	 * ImGuiDrawableOwnerInterface implementation
	 */

	ImGuiDrawableObjectRegistration* ImGuiDrawableOwnerInterface::FindRegisteredDrawable(char const* name)
	{
		for (auto& drawable : registered_drawable)
			if (StringTools::Stricmp(name, drawable->GetName()))
				return drawable.get();
		return nullptr;
	}

	void ImGuiDrawableOwnerInterface::RemoveRegisteredDrawable(char const* name)
	{
		auto it = std::ranges::find_if(registered_drawable, [name](auto & drawable) { return chaos::StringTools::Stricmp(drawable->GetName(), name) == 0; });
		if (it != std::ranges::end(registered_drawable))
			registered_drawable.erase(it);
	}

	ImGuiDrawableObjectRegistration * ImGuiDrawableOwnerInterface::RegisteredDrawable(char const* name, std::function<ImGuiDrawableObject* ()> drawable_creation_function)
	{
		if (ImGuiDrawableObjectRegistration* registration = FindRegisteredDrawable(name))
		{
			Log::Warning("ImGuiDrawableOwnerInterface::RegisteredDrawable: %s already existing", name);
		}
		else if (ImGuiDrawableObjectRegistration* result = new ImGuiDrawableObjectRegistration())
		{
			result->name = name;
			result->creation_function = drawable_creation_function;
			registered_drawable.push_back(result);
			return result;
		}
		return nullptr;
	}

	ImGuiDrawableObjectRegistration * ImGuiDrawableOwnerInterface::RegisteredDrawable(char const* name, SubClassOf<ImGuiDrawableObject> drawable_class)
	{
		return RegisteredDrawable(name, [drawable_class]()
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
				if (ImGui::BeginMenu("Windows"))
				{
					for (auto& drawable : registered_drawable)
						if (ImGui::MenuItem(drawable->GetName(), nullptr, drawable->IsVisible(), true))
							drawable->Show(!drawable->IsVisible());
					ImGui::EndMenu();
				}
			});

			// draw the drawables
			for (auto& drawable : registered_drawable)
				drawable->DrawImGui(menu_mode);
		}
	}

}; // namespace chaos