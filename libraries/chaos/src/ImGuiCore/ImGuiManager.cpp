#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<ImGuiWindowStyle> const ImGuiWindowStyle_metadata =
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
		if (context == ReadConfigurablePropertiesContext::Initialization) // do not accept hot-reload
		{
			JSONTools::GetAttribute(config, "font_faces", font_faces);
			JSONTools::GetAttribute(config, "window_style", window_style);

			// load all files. Remove the faces that are failing
			auto it = std::remove_if(font_faces.begin(), font_faces.end(), [](ImGuiFontFace& font_face)
			{
				font_face.file_buffer = FileTools::LoadFile(font_face.path);
				if (!font_face.file_buffer)
				{
					ImGuiLog::Error("ImGuiManager::BuildAtlas: fails to load file %s", font_face.path.string().c_str());
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
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::Initialization, false))
			return false;
		return true;
	}

	bool ImGuiManager::DoStopManager()
	{
		font_faces.clear();
		return ResourceManager::DoStopManager();
	}

	ImFontAtlas* ImGuiManager::BuildFontAtlas() const
	{
		if (font_faces.size() == 0)
		{
			ImGuiLog::Error("ImGuiManager::BuildAtlas: no font defined");
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

}; // namespace chaos