#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumMetaData<ImGuiWindowStyle> const ImGuiWindowStyle_metadata =
	{
		{ ImGuiWindowStyle::Dark, "Dark" },
		{ ImGuiWindowStyle::Light, "Light" },
		{ ImGuiWindowStyle::Classic, "Classic" }
	};
	CHAOS_IMPLEMENT_ENUM_METHOD(ImGuiWindowStyle, &ImGuiWindowStyle_metadata, CHAOS_API);

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

	ImWchar const* ImGuiFontFace::GetGlyphRange() const
	{
		if (range.has_value())
		{
			glyph_range[0] = range.value().first;
			glyph_range[1] = range.value().second;
			// ensure range en properly sorted (if glyph 0 were to be inside range, this would cause a crash)
			if (glyph_range[0] > glyph_range[1])
				std::swap(glyph_range[0], glyph_range[1]);
			glyph_range[0] = std::max((ImWchar)1, glyph_range[0]);
			glyph_range[1] = std::max((ImWchar)1, glyph_range[1]);
			return glyph_range;
		}
		return nullptr;
	}

	bool ImGuiManager::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		if (context == ReadConfigurablePropertiesContext::INITIALIZATION) // do not accept hot-reload
		{
			JSONTools::GetAttribute(config, "font_faces", font_faces);
			JSONTools::GetAttribute(config, "window_style", window_style);

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
			font_config.MergeMode = false;

			for (ImGuiFontFace const& font_face : font_faces)
			{
				// prepare font parameters
				float height = font_face.height.value_or(16.0f);
				const ImWchar* range = font_face.GetGlyphRange();

				// add the font
				result->AddFontFromMemoryTTF(font_face.file_buffer.data, (int)font_face.file_buffer.bufsize, height, &font_config, range);

				// for next font, use merge mode
				font_config.MergeMode = true;
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

		switch (window_style)
		{
		case ImGuiWindowStyle::Dark: ImGui::StyleColorsDark(); break;
		case ImGuiWindowStyle::Light: ImGui::StyleColorsLight(); break;
		case ImGuiWindowStyle::Classic: ImGui::StyleColorsClassic(); break;
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
					if (ImGui::BeginMenu("Show atlas"))
					{
						ImGuiIO const& io = ImGui::GetIO();

						if (io.Fonts != nullptr && io.Fonts->Fonts.Size > 0)
						{
							// display texture information
							ImGui::Text("Widget: %d", io.Fonts->TexWidth); ImGui::SameLine(0, 20.0f); ImGui::Text("Height: %d", io.Fonts->TexHeight);
							ImGui::Text("Glyph count: %d", io.Fonts->Fonts[0]->Glyphs.size());
							ImGui::Separator();

							// display atlas texture (prevent texture size to be greater than 1024)
							glm::vec2 texture_size =
							{
								io.Fonts->TexWidth,
								io.Fonts->TexHeight
							};

							int major_axis = (texture_size.x > texture_size.y) ? 0 : 1;

							float max_size = 1024.0f;
							float ratio = 1.0f;
							if (texture_size[major_axis] > max_size)
								ratio = max_size / texture_size[major_axis];

							texture_size[0] = texture_size[0] * ratio;
							texture_size[1] = texture_size[1] * ratio;

							ImGui::Image(io.Fonts->TexID, { texture_size.x, texture_size.y });
							ImGui::EndMenu();
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
		});
	}

}; // namespace chaos