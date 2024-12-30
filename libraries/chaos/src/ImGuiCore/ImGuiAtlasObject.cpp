#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiAtlasObject::OnDrawImGuiContent(Window* window)
	{
		ImGuiIO const& io = ImGui::GetIO();

		if (io.Fonts != nullptr && io.Fonts->Fonts.Size > 0)
		{
			ImFont const* Font = io.Fonts->Fonts[0];

			// display texture information
			ImGui::Text("Widget: %d", io.Fonts->TexWidth); ImGui::SameLine(0, 20.0f); ImGui::Text("Height: %d", io.Fonts->TexHeight);
			ImGui::Text("Glyph count: %d", Font->Glyphs.size());
			ImVec2 current_cursor = ImGui::GetCursorPos(); // keep some empty space so we can display selected glyph inside
			ImGui::Text(" ");

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
			if (ImGui::IsItemHovered())
			{
				ImVec2 mouse_pos = ImGui::GetMousePos();

				// get current image size and position
				ImVec2 rect_min = ImGui::GetItemRectMin();
				ImVec2 rect_max = ImGui::GetItemRectMax();
				ImVec2 rect_size = { rect_max.x - rect_min.x, rect_max.y - rect_min.y };

				// get mouse position relative to the image
				ImVec2 relative_pos;
				relative_pos.x = (mouse_pos.x - rect_min.x) / rect_size.x;
				relative_pos.y = (mouse_pos.y - rect_min.y) / rect_size.y;

				for (int i = 0; i < Font->Glyphs.Size; ++i)
				{
					ImFontGlyph const* glyph = &Font->Glyphs[i];

					if (relative_pos.x >= glyph->U0 &&
						relative_pos.x <= glyph->U1 &&
						relative_pos.y >= glyph->V0 &&
						relative_pos.y <= glyph->V1)
					{
						// fill the space kept above with selected glyph information
						ImVec2 saved_cursor = ImGui::GetCursorPos();
						ImGui::SetCursorPos(current_cursor);
						ImGui::Text("Glyph: 0x%X (%d)", glyph->Codepoint, glyph->Codepoint);
						ImGui::SetCursorPos(saved_cursor);

						// outline selected glyph
						ImVec2 glyph_rect_min =
						{
							rect_min.x + glyph->U0 * rect_size.x,
							rect_min.y + glyph->V0 * rect_size.y
						};
						ImVec2 glyph_rect_max =
						{
							rect_min.x + glyph->U1 * rect_size.x,
							rect_min.y + glyph->V1 * rect_size.y
						};

						ImDrawList* draw_list = ImGui::GetWindowDrawList();
						ImU32 color = IM_COL32(255, 0, 0, 255);
						float thickness = 2.0f;
						draw_list->AddRect(glyph_rect_min, glyph_rect_max, color, 0.0f, 0, thickness);

						break;
					}
				}
			}
		}
	}

}; // namespace chaos