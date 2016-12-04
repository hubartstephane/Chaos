#include <chaos/SpriteManager.h>
#include <chaos/MathTools.h>
#include <chaos/GLProgramLoader.h>
#include <chaos/GLTools.h>


namespace chaos
{

	// should try string literal from C++ 11
  char const * SpriteManager::vertex_shader_source = R"SHADERCODE(
    in vec2 position;
    in vec3 texcoord;
    in vec3 color;

    uniform mat4 local_to_cam;
    
    out vec3 vs_texcoord;
    out vec3 vs_color;
    
    void main()
    {
      vs_texcoord = texcoord;
      vs_color    = color;
      gl_Position = local_to_cam * vec4(position.x, position.y, 0.0, 1.0);
    };											
	)SHADERCODE";

	// should try string literal from C++ 11
  char const * SpriteManager::pixel_shader_source = R"SHADERCODE(
    in vec3 vs_texcoord;
    in vec3 vs_color;

    out vec4 output_color;

    uniform sampler2DArray material;

    void main()
    {
      vec4 color = texture(material, vs_texcoord);
      output_color.xyz = color.xyz * vs_color;
      output_color.a   = color.a;
    };
	)SHADERCODE";

	void SpriteManager::Finalize()
	{
		program       = nullptr;
		vertex_array  = nullptr;
		vertex_buffer = nullptr;
    atlas         = nullptr;

		declaration.Clear();	
	}

  bool SpriteManager::Initialize(SpriteManagerInitParams & params)
  {
    Finalize();
    if (!DoInitialize(params))
    {
      Finalize();
      return false;
    }
    return true;
  }

  bool SpriteManager::DoInitialize(SpriteManagerInitParams & params)
  {
    if (params.atlas == nullptr)
      return false;

    atlas = params.atlas;

		// create GPU-Program
		GLProgramLoader loader;
		loader.AddShaderSource(GL_VERTEX_SHADER,   vertex_shader_source);
		loader.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);

		program = loader.GenerateProgramObject();
		if (program == nullptr)
			return false;

		// prepare the vertex declaration
		declaration.Push(SEMANTIC_POSITION, 0, TYPE_FLOAT2);
		declaration.Push(SEMANTIC_TEXCOORD, 0, TYPE_FLOAT3);
    declaration.Push(SEMANTIC_COLOR,    0, TYPE_FLOAT3);

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

  void SpriteManager::AddSprite(BitmapAtlas::CharacterEntry const * entry, glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type, glm::vec3 const & color)
  {
    glm::vec2 bottom_left_position = GetBottomLeftHotpointPosition(position, size, hotpoint_type);
    AddSpriteImpl(entry, bottom_left_position, size, color);
  }

  void SpriteManager::AddSprite(BitmapAtlas::BitmapEntry const * entry, glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type)
  {
    static glm::vec3 const color(1.0f, 1.0f, 1.0f);
    glm::vec2 bottom_left_position = GetBottomLeftHotpointPosition(position, size, hotpoint_type);
    AddSpriteImpl(entry, bottom_left_position, size, color);
  }

  void SpriteManager::AddSpriteImpl(BitmapAtlas::BitmapEntry const * entry, glm::vec2 const & bottomleft_position, glm::vec2 const & size, glm::vec3 const & color)
  {
		glm::vec2 topright_position = bottomleft_position + size;
    glm::vec2 atlas_size        = atlas->GetAtlasDimension();

		float tex_x1 = MathTools::CastAndDiv<float>(entry->x, atlas_size.x);
		float tex_y1 = MathTools::CastAndDiv<float>(entry->y, atlas_size.y);
		float tex_x2 = MathTools::CastAndDiv<float>(entry->x + entry->width, atlas_size.x);
		float tex_y2 = MathTools::CastAndDiv<float>(entry->y + entry->height, atlas_size.y);

		float bitmap_index = (float)entry->bitmap_index;

		SpriteVertex bl;
		bl.position.x = bottomleft_position.x;
		bl.position.y = bottomleft_position.y;
		bl.texcoord.x = tex_x1;
		bl.texcoord.y = tex_y1;
		bl.texcoord.z = bitmap_index;
    bl.color      = color;

		SpriteVertex tr;
		tr.position.x = topright_position.x;
		tr.position.y = topright_position.y;
		tr.texcoord.x = tex_x2;
		tr.texcoord.y = tex_y2;
		tr.texcoord.z = bitmap_index;
    tr.color      = color;

		SpriteVertex tl;
		tl.position.x = bottomleft_position.x;
		tl.position.y = topright_position.y;
		tl.texcoord.x = tex_x1;
		tl.texcoord.y = tex_y2;
		tl.texcoord.z = bitmap_index;
    tl.color      = color;

		SpriteVertex br;
		br.position.x = topright_position.x;
		br.position.y = bottomleft_position.y;
		br.texcoord.x = tex_x2;
		br.texcoord.y = tex_y1;
		br.texcoord.z = bitmap_index;
    br.color      = color;

		sprites.push_back(bl);				
    sprites.push_back(br);
    sprites.push_back(tr);

		sprites.push_back(bl);				
    sprites.push_back(tr);
    sprites.push_back(tl);
	}


	void SpriteManager::Display()
	{
    if (sprites.size() == 0)
      return;

    UpdateGPUVertexBuffer();

    // context states
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // GPU-Program
    glUseProgram(program->GetResourceID());

    // Initialize the vertex array
    glBindVertexArray(vertex_array->GetResourceID());

    GLProgramData const & program_data = program->GetProgramData();
    program_data.BindAttributes(vertex_array->GetResourceID(), declaration, nullptr);
    // Texture
    glBindTextureUnit(0, atlas->GetTexture()->GetResourceID());

    GLUniformInfo const * material = program->GetProgramData().FindUniform("material");
    if (material != nullptr)
      glUniform1i(material->location, 0);
       
    // The drawing   
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)sprites.size());

    // restore states
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	}

  void SpriteManager::UpdateGPUVertexBuffer()
  {
#if 0
#define glMapNamedBuffer GLEW_GET_FUN(__glewMapNamedBuffer)
#define glMapNamedBufferRange GLEW_GET_FUN(__glewMapNamedBufferRange)
#define glNamedBufferData GLEW_GET_FUN(__glewNamedBufferData)
#define glNamedBufferStorage GLEW_GET_FUN(__glewNamedBufferStorage)
#define glNamedBufferSubData GLEW_GET_FUN(__glewNamedBufferSubData)
#endif

    // fill GPU buffer
    size_t count = sprites.size();
    glNamedBufferData(vertex_buffer->GetResourceID(), count * sizeof(SpriteVertex), &sprites[0], GL_STATIC_DRAW);

    GLuint binding_index = 0;
    glVertexArrayVertexBuffer(vertex_array->GetResourceID(), binding_index, vertex_buffer->GetResourceID(), 0, declaration.GetVertexSize());
  }
};