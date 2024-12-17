#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumMetaData<ImGuiStyle> const ImGuiStyle_metadata =
	{
		{ ImGuiStyle::Dark, "Dark" },
		{ ImGuiStyle::Light, "Light" },
		{ ImGuiStyle::Classic, "Classic" }
	};
	CHAOS_IMPLEMENT_ENUM_METHOD(ImGuiStyle, &ImGuiStyle_metadata, CHAOS_API);

	bool DoSaveIntoJSON(nlohmann::json* json, ImGuiFontFace const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "path", src.path);
		JSONTools::SetAttribute(json, "range", src.range);
		JSONTools::SetAttribute(json, "height", src.height);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, ImGuiFontFace& dst)	
	{
		JSONTools::GetAttribute(config, "path", dst.path);
		JSONTools::GetAttribute(config, "range", dst.range);
		JSONTools::GetAttribute(config, "height", dst.height);
		return true;
	}

	bool ImGuiManager::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		if (context == ReadConfigurablePropertiesContext::INITIALIZATION) // do not accept hot-reload
		{
			JSONTools::GetAttribute(config, "font_faces", font_faces);
			JSONTools::GetAttribute(config, "style", style);

			// load all files. Remove the faces that are failing
			auto it = std::remove_if(font_faces.begin(), font_faces.end(), [](ImGuiFontFace& font_face)
			{
				font_face.file_buffer = FileTools::LoadFile(font_face.path);
				if (!font_face.file_buffer)
				{
					ImGuiManagerLog::Error("ImGuiManager::BuildAtlas: fails to load file %s", font_face.path.string().c_str());
					return true;
				}
				return false;
			});
			if (it != font_faces.end())
				font_faces.erase(it, font_faces.end());
		}
		return true;
	}

	bool ImGuiManager::DoStartManager()
	{
		// super method
		if (!ResourceManager::DoStartManager())
			return false;
		// read the properties
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::INITIALIZATION, false))
			return false;

		return true;
	}

	ImFontAtlas* ImGuiManager::BuildAtlas() const
	{
		if (font_faces.size() == 0)
		{
			ImGuiManagerLog::Error("ImGuiManager::BuildAtlas: no font defined");
			return nullptr;
		}

		ImFontAtlas* result = new ImFontAtlas;

		if (result != nullptr)
		{
			ImFontConfig font_config;
			font_config.MergeMode = true;

			ImFontConfig const* font_config_ptr = nullptr;

			for (ImGuiFontFace const& font_face : font_faces)
			{
				// prepare font parameters
				float height = font_face.height.value_or(16.0f);

				ImWchar range[3] = { 0, 0, 0 };
				if (font_face.range.has_value())
				{
					range[0] = font_face.range.value().first;
					range[1] = font_face.range.value().second;
				}
				else
				{
					ImWchar const* default_range = result->GetGlyphRangesDefault();
					range[0] = default_range[0];
					range[1] = default_range[1];
				}

				// add the font
				result->AddFontFromMemoryTTF(font_face.file_buffer.data, (int)font_face.file_buffer.bufsize, height, font_config_ptr);

				// for next font, use merge mode
				font_config_ptr = &font_config;
			}
		}
		return result;
	}

	void ImGuiManager::InitializeImGuiContext(Window * window) const
	{
		assert(window->GetImGuiContext() == ImGui::GetCurrentContext());

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;     // Disable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls		
		io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;  // Do not capture keyboard during navigation
		
		SetImGuiContextMouseFlag(window->IsImGuiMenuEnabled());

		switch (style)
		{
		case ImGuiStyle::Dark: ImGui::StyleColorsDark(); break;
		case ImGuiStyle::Light: ImGui::StyleColorsLight(); break;
		case ImGuiStyle::Classic: ImGui::StyleColorsClassic(); break;
		}
	}

	void ImGuiManager::FinalizeImGuiContext(Window* window) const
	{
		assert(window->GetImGuiContext() == ImGui::GetCurrentContext());
	}

	void ImGuiManager::OnImGuiMenuEnabledChanged(Window* window, bool enabled) const
	{
		assert(window->GetImGuiContext() == ImGui::GetCurrentContext());
		SetImGuiContextMouseFlag(enabled);
	}

	void ImGuiManager::SetImGuiContextMouseFlag(bool enabled) const
	{
		ImGuiIO& io = ImGui::GetIO();
		if (enabled)
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse; // need mouse
		else
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse; // don't want mouse (elsewhere imgui can react to an invisible cursor)
	}

	void ImGuiManager::OnDrawImGuiMenu(BeginImGuiMenuFunc begin_menu_func)
	{
		begin_menu_func([this]()
		{
			if (ImGui::BeginMenu("Managers"))
			{
				if (ImGui::BeginMenu("ImGui"))
				{
					if (ImGui::MenuItem("Reload"))
					{
						ReloadDefaultPropertiesFromFile(true, true);
					}
					if (ImGui::BeginMenu("Show atlas"))
					{
						ImGuiIO const& io = ImGui::GetIO();
						ImGui::Image(io.Fonts->TexID, {512, 512});
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
		});
	}

}; // namespace chaos