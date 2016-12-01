#include <chaos/SpriteManager.h>
#include <chaos/MathTools.h>
#include <chaos/GLProgramLoader.h>
#include <chaos/GLTools.h>


namespace chaos
{

	// should try string literal from C++ 11
	char const * SpriteManager::vertex_shader_source = R"SHADERCODE(
		
		
		
		
		
		
	)SHADERCODE";

	char const * sss =

		"layout (location = 0) in vec2 position; \n\
    layout (location = 1) in vec2 texcoord; \n\
    uniform vec2 position_factor; \n\
    out vec2 tex_coord; \n\
    void main() \n\
    { \n\
    tex_coord = texcoord; \n\
    vec2 pos = (position * position_factor) + vec2(-1.0, 1.0); \n\
    gl_Position = vec4(pos, 0.0, 1.0); \n\
    }";
	// should try string literal from C++ 11
	char const * SpriteManager::pixel_shader_source = 
		"out vec4 output_color; \n\
    in vec2 tex_coord; \n\
    uniform sampler2D material; \n\
    void main() \n\
    { \n\
    vec4 color = texture(material, tex_coord); \n\
    float alpha = 1.0; \n\
    if ((color.r + color.b + color.a) > 2.0) \n\
    discard; \n\
    output_color = vec4(1.0, 1.0, 1.0, alpha); \n\
    }";

	void SpriteManager::Finalize()
	{
		program       = nullptr;
		vertex_array  = nullptr;
		vertex_buffer = nullptr;

		declaration.Clear();	
	}

	bool SpriteManager::Initialize()
	{
		// create GPU-Program
		GLProgramLoader loader;
		loader.AddShaderSource(GL_VERTEX_SHADER,   vertex_shader_source);
		loader.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);

		program = loader.GenerateProgramObject();
		if (program == nullptr)
			return false;

		// prepare the vertex declaration
		declaration.Push(SEMANTIC_POSITION, 0, TYPE_FLOAT3);
		declaration.Push(SEMANTIC_TEXCOORD, 0, TYPE_FLOAT3);

		// Generate Vertex Array and Buffer
		if (!GLTools::GenerateVertexAndIndexBuffersObject(&vertex_array, &vertex_buffer, nullptr))
			return false;





		return true;
	}

	glm::vec2 SpriteManager::GetHotpointPosition(glm::vec2 const & hotpoint, glm::vec2 const & size, int initial_hotpoint_type, int final_hotpoint_type)
	{
		if (initial_hotpoint_type == final_hotpoint_type)
			return hotpoint;

		static float const offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

		int initial_h_part = (initial_hotpoint_type >> 0) & 3;
		int initial_v_part = (initial_hotpoint_type >> 2) & 3;

		int final_h_part = (final_hotpoint_type >> 0) & 3;
		int final_v_part = (final_hotpoint_type >> 2) & 3;

		assert(initial_h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
		assert(initial_v_part != 3);
		assert(final_h_part   != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
		assert(final_v_part   != 3);

		glm::vec2 result = hotpoint;
		result.x += size.x * (offset_factor[initial_h_part] - offset_factor[final_h_part]);
		result.y += size.y * (offset_factor[initial_v_part] - offset_factor[final_v_part]);
		return result;

	}

	glm::vec2 SpriteManager::GetBottomLeftHotpointPosition(glm::vec2 const & hotpoint, glm::vec2 const & size, int hotpoint_type)
	{
		static float const offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

		int h_part = (hotpoint_type >> 0) & 3;
		int v_part = (hotpoint_type >> 2) & 3;

		assert(h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
		assert(v_part != 3);

		glm::vec2 result = hotpoint;
		result.x += size.x * offset_factor[h_part];
		result.y += size.y * offset_factor[v_part];
		return result;
	}



	void SpriteManager::AddSprite(BitmapAtlas::BitmapEntry * entry, glm::vec2 const & position, glm::vec2 const & size, glm::vec2 const & handler)
	{
		glm::vec2 atlas_size = atlas.GetAtlasDimension();
		glm::vec2 pos_bottom_left = position + size * handler;
		glm::vec2 pos_top_right = pos_bottom_left + size;

		float tex_x1 = MathTools::CastAndDiv<float>(entry->x, atlas_size.x);
		float tex_y1 = MathTools::CastAndDiv<float>(entry->y, atlas_size.y);
		float tex_x2 = MathTools::CastAndDiv<float>(entry->x + entry->width, atlas_size.x);
		float tex_y2 = MathTools::CastAndDiv<float>(entry->y + entry->height, atlas_size.y);

		float bitmap_index = (float)entry->bitmap_index;

		SpriteVertex bl;
		bl.position.x = pos_bottom_left.x;
		bl.position.y = pos_bottom_left.y;
		bl.position.z = 0.0f;
		bl.texcoord.x = tex_x1;
		bl.texcoord.y = tex_y1;
		bl.texcoord.z = bitmap_index;

		SpriteVertex tr;
		tr.position.x = pos_top_right.x;
		tr.position.y = pos_top_right.y;
		tr.position.z = 0.0f;
		tr.texcoord.x = tex_x2;
		tr.texcoord.y = tex_y2;
		tr.texcoord.z = bitmap_index;

		SpriteVertex tl;
		tl.position.x = pos_bottom_left.x;
		tl.position.y = pos_top_right.y;
		tl.position.z = 0.0f;
		tl.texcoord.x = tex_x1;
		tl.texcoord.y = tex_y2;
		tl.texcoord.z = bitmap_index;

		SpriteVertex br;
		br.position.x = pos_top_right.x;
		br.position.y = pos_bottom_left.y;
		br.position.z = 0.0f;
		br.texcoord.x = tex_x2;
		br.texcoord.y = tex_y1;
		br.texcoord.z = bitmap_index;

		sprites.push_back(bl);
		sprites.push_back(br);
		sprites.push_back(tr);

		sprites.push_back(bl);
		sprites.push_back(tr);
		sprites.push_back(tl);
	}

	void SpriteManager::Display()
	{
	
	}
};