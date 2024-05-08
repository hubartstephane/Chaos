#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	/**
	 * ImGuiObjectProxy implementation
	 */

	ImGuiObjectProxy::~ImGuiObjectProxy()
	{
		assert(owner == nullptr);
	}

	void ImGuiObjectProxy::DrawImGui()
	{
		if (imgui_object != nullptr)
		{
			imgui_object->DrawImGui(name.c_str(), ImGuiDrawFlags::NONE);
			if (imgui_object->IsClosingRequested()) // transfert closing request
				RequestClosing();
		}
	}

	bool ImGuiObjectProxy::IsVisible() const
	{
		return visible;
	}

	void ImGuiObjectProxy::Show(bool show)
	{
		if (show)
		{
			if (!visible)
			{
				assert(imgui_object == nullptr);
				imgui_object = imgui_object_create_function();
			}
		}
		else
		{
			if (visible)
			{
				assert(imgui_object != nullptr);
				imgui_object = nullptr;

			}
		}
		visible = show;
	}

	char const* ImGuiObjectProxy::GetName() const
	{
		return name.c_str();
	}

	void ImGuiObjectProxy::SubReference()
	{
		// the proxy does not belong to any owner: handle as usual
		if (owner == nullptr)
			Object::SubReference();
		// the last reference is the one from the owner. Destroy it
		else if (--shared_count == 1)
			owner->UnregisterImGuiObjectProxy(this);
	}

	void ImGuiObjectProxy::RequestClosing()
	{
		closing_request = true;
	}

	bool ImGuiObjectProxy::IsClosingRequested() const
	{
		return closing_request;
	}

	/**
	 * ImGuiObjectOwnerInterface implementation
	 */

	ImGuiObjectOwnerInterface::~ImGuiObjectOwnerInterface()
	{
		while (proxies.size() > 0)
		{
			if (ImGuiObjectProxy* proxy = proxies[proxies.size() - 1].get())
				proxy->owner = nullptr;
			proxies.pop_back();
		}
	}

	ImGuiObjectProxy* ImGuiObjectOwnerInterface::FindImGuiObjectProxy(char const* name)
	{
		for (auto& proxy : proxies)
			if (StringTools::Stricmp(name, proxy->GetName()) == 0)
				return proxy.get();
		return nullptr;
	}

	void ImGuiObjectOwnerInterface::UnregisterImGuiObjectProxy(ImGuiObjectProxy* proxy)
	{
		auto it = std::ranges::find_if(proxies, [proxy](auto& other_proxy) { return other_proxy.get() == proxy; });
		if (it != std::ranges::end(proxies))
		{
			proxy->owner = nullptr; // order is important, next line may destroy it
			proxies.erase(it);
		}
	}

	ImGuiObjectProxy * ImGuiObjectOwnerInterface::RegisterImGuiObjectProxy(char const* name, std::function<ImGuiObject* ()> imgui_object_create_function)
	{
		if (ImGuiObjectProxy* proxy = FindImGuiObjectProxy(name))
		{
			Log::Warning("ImGuiObjectOwnerInterface::RegisterImGuiObjectProxy: proxy named %s already exists", name);
		}
		else if (ImGuiObjectProxy* result = new ImGuiObjectProxy())
		{
			result->name = name;
			result->imgui_object_create_function = std::move(imgui_object_create_function);
			result->owner = this;
			proxies.push_back(result);
			return result;
		}
		return nullptr;
	}

	ImGuiObjectProxy * ImGuiObjectOwnerInterface::RegisterImGuiObjectProxy(char const* name, SubClassOf<ImGuiObject> drawable_class)
	{
		return RegisterImGuiObjectProxy(name, [drawable_class]()
		{
			return drawable_class.CreateInstance();
		});
	}

	void ImGuiObjectOwnerInterface::DrawImGuiObjectsMenu(BeginImGuiMenuFunc begin_menu_func)
	{
		if (proxies.size() > 0)
		{
			begin_menu_func([this]()
			{
				if (ImGui::BeginMenu("Widgets"))
				{
					for (auto& proxy : proxies)
						if (ImGui::MenuItem(proxy->GetName(), nullptr, proxy->IsVisible(), true))
							proxy->Show(!proxy->IsVisible());
					ImGui::EndMenu();
				}
			});
		}
	}

	void ImGuiObjectOwnerInterface::DrawImGuiObjects()
	{
		if (proxies.size() > 0)
		{
			// draw the drawables
			for (auto& proxy : proxies)
			{
				proxy->DrawImGui();
				if (proxy->IsClosingRequested())
				{
					proxy->Show(false);
					proxy->closing_request = false; // reset the request
				}
			}
		}
	}

}; // namespace chaos