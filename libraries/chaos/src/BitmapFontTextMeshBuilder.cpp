#include <chaos/BitmapFontTextMeshBuilder.h>

namespace chaos
{

	void BitmapFontTextMeshBuilder::InsertVertexComponent(std::vector<float> & vertices, glm::vec2 const & v, glm::vec2 const & t)
	{
		vertices.push_back(v.x);
		vertices.push_back(v.y);
		vertices.push_back(t.x);
		vertices.push_back(1.0f - t.y); // the characters in the bitmap are in the direction of reading (top to bottom). That's not the texture coordinates
	}

	box2 BitmapFontTextMeshBuilder::BuildBuffer(char const * msg, BitmapFontTextMeshBuilder::Params const & params, std::vector<float> & vertices) const
	{
		assert(msg != nullptr);

		size_t initial_vertex_count = vertices.size();

		box2 result;

		int char_in_line = 0;

		float tex_size_x = 1.0f / (float)params.font_characters_per_line;
		float tex_size_y = 1.0f / (float)params.font_characters_line_count;

		float x         =  params.position.x;
		float y         =  params.position.y;
		float dx        =  params.character_size.x; 
		float dy        = -params.character_size.y; 
		float next_char =  params.character_size.x + params.spacing.x;
		float next_line = -params.character_size.y - params.spacing.y;      // we want the Y = 0 to be the TOPLEFT position of the text. To match natural OpenGL system, the remining characters are in negative Y axix

		float next_tab  =  next_char * (float)(params.tab_size);

		vertices.reserve(vertices.size() + 6 * 4 * strlen(msg)); // 6 vertex per characters, 4 float per vertex

		for (int i = 0 ; msg[i] != 0 ; ++i) // each characters of the final string
		{
			char c = msg[i];

			if (c == '\r') // ignore chariot return (UNIX/WINDOWS differences)
				continue;

			if (c == '\n') // next line
			{
				x = params.position.x;
				y = y + next_line;
				char_in_line = 0;
				continue;
			}

			if (c == '\t') // tabulations
			{
				if (params.line_limit <= 0) 
					x += next_tab;
				else
				{
					for (int j = 0 ; j < params.tab_size ; ++j)
					{
						if (++char_in_line >= params.line_limit)
						{
							x = params.position.x;
							y = y + next_line;
							char_in_line = 0;
						}
						else
							x += next_char;
					}
				}
				continue;
			}

			char const * tmp = strchr(&params.font_characters[0], c);
			if (tmp != nullptr)
			{
				size_t char_index = (tmp - &params.font_characters[0]);
				size_t char_x     = (char_index % params.font_characters_per_line);
				size_t char_y     = (char_index / params.font_characters_per_line);

				float a = ((float)char_x) * tex_size_x;
				float b = ((float)char_y) * tex_size_y;

				// compute the 4 texture coordinates
				glm::vec2 tb = glm::vec2(a, b);

				float cx = params.crop_texture.x;
				float cy = params.crop_texture.y;

				glm::vec2 t1 = tb + glm::vec2(cx             , cy);
				glm::vec2 t2 = tb + glm::vec2(tex_size_x - cx, cy);
				glm::vec2 t3 = tb + glm::vec2(tex_size_x - cx, tex_size_y - cy);
				glm::vec2 t4 = tb + glm::vec2(cx             , tex_size_y - cy);

				// compute the 4 vertex coordinates
				glm::vec2 v1 = glm::vec2(x, y);
				glm::vec2 v2 = v1 + glm::vec2(dx, 0.0f);
				glm::vec2 v3 = v2 + glm::vec2(0.0f, dy);
				glm::vec2 v4 = v1 + glm::vec2(0.0f, dy);

				// insert the triangles
				InsertVertexComponent(vertices, v1, t1);		  
				InsertVertexComponent(vertices, v3, t3);
				InsertVertexComponent(vertices, v2, t2);

				InsertVertexComponent(vertices, v1, t1);
				InsertVertexComponent(vertices, v4, t4);
				InsertVertexComponent(vertices, v3, t3);

				ExtendBox(result, v1);
				ExtendBox(result, v3);
			}

			// next character 
			x = x + next_char;

			if (params.line_limit > 0)
			{
				if (++char_in_line >= params.line_limit)
				{
					x = params.position.x;
					y = y + next_line;
					char_in_line = 0;
				}
			}
		}

		// recenter the whole box
		glm::vec2 offset(0.0f, 0.0f);

		if (((int)params.alignment_flags & (int)BitmapFontTextMeshAlign::RIGHT_FLAG) != 0)
			offset.x = -2.0f * result.half_size.x;
		if (((int)params.alignment_flags & (int)BitmapFontTextMeshAlign::MIDDLE_HORIZ_FLAG) != 0)
			offset.x = -result.half_size.x;

		if (((int)params.alignment_flags & (int)BitmapFontTextMeshAlign::BOTTOM_FLAG) != 0)
			offset.y = 2.0f * result.half_size.y;
		if (((int)params.alignment_flags & (int)BitmapFontTextMeshAlign::MIDDLE_VERT_FLAG) != 0)
			offset.y = result.half_size.y;

		if (offset.x != 0.0f || offset.y != 0.0f)
			ApplyVertexOffset(vertices, offset, initial_vertex_count, vertices.size());

		return result;
	}

	void BitmapFontTextMeshBuilder::ApplyVertexOffset(std::vector<float> & vertices, glm::vec2 const & offset, size_t start, size_t end)
	{
		int const PER_VERTEX_COMPONENT = 4;
		for (size_t i = start ; i < end ; i += PER_VERTEX_COMPONENT)
		{
			vertices[i + 0] += offset.x;
			vertices[i + 1] += offset.y;
		}
	}

}; // namespace chaos
