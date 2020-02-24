#include "Ludum40SpriteManager.h"

#include <chaos/MathTools.h>
#include <chaos/GPUProgramGenerator.h>
#include <chaos/GLTools.h>
#include <chaos/ParticleTools.h>

namespace chaos
{
	char const * SpriteManager::vertex_shader_source = R"VERTEXSHADERCODE(
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
	)VERTEXSHADERCODE";

	char const * SpriteManager::pixel_shader_source = R"PIXELSHADERCODE(
    in vec3 vs_texcoord;
    in vec3 vs_color;

    out vec4 output_color;

    uniform sampler2DArray material;

    void main()
    {
	vec4 color = (vs_texcoord.x < 0.0 || vs_texcoord.y < 0.0)? 
		vec4(1.0, 1.0, 1.0, 1.0) : 
		texture(material, vs_texcoord);
      output_color.xyz = color.xyz * vs_color;
      output_color.a   = color.a;
    };
	)PIXELSHADERCODE";

	void SpriteManager::Finalize()
	{
		program = nullptr;
		vertex_array = nullptr;
		vertex_buffer = nullptr;
		atlas = nullptr;

		declaration.Clear();
		sprites.clear();
	}

	void SpriteManager::ClearSprites()
	{
		sprites.clear();
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
		if (params.program != nullptr)
			program = params.program;
		else
		{
			GPUProgramGenerator program_generator;
			program_generator.AddShaderSource(GL_VERTEX_SHADER, vertex_shader_source);
			program_generator.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);

			program = program_generator.GenProgramObject();
			if (program == nullptr)
				return false;
		}

		// prepare the vertex declaration
		declaration.Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT2);
		declaration.Push(VertexAttributeSemantic::TEXCOORD, 0, VertexAttributeType::FLOAT3);
		declaration.Push(VertexAttributeSemantic::COLOR, 0, VertexAttributeType::FLOAT3);

		// Generate Vertex Array and Buffer
		if (!GLTools::GenerateVertexAndIndexBuffers(&vertex_array, &vertex_buffer, nullptr, true, false))  // vertex_buffer is dynamic
			return false;

		GPUProgramData const & program_data = program->GetProgramData();
		program_data.BindAttributes(vertex_array->GetResourceID(), declaration, nullptr);

		return true;
	}

	void SpriteManager::AddSpriteCharacter(BitmapAtlas::CharacterInfo const * info, glm::vec2 const & position, glm::vec2 const & size, HotpointType hotpoint_type, glm::vec3 const & color)
	{
		ParticleCorners corners = ParticleTools::GetParticleCorners(position, size, hotpoint_type);
		AddSpriteImpl(info, corners, color);
	}

	void SpriteManager::AddSpriteBitmap(BitmapAtlas::BitmapInfo const * info, glm::vec2 const & position, glm::vec2 const & size, HotpointType hotpoint_type)
	{
		static glm::vec3 const color(1.0f, 1.0f, 1.0f);

		ParticleCorners corners = ParticleTools::GetParticleCorners(position, size, hotpoint_type);
		AddSpriteImpl(info, corners, color);
	}

	void SpriteManager::AddSpriteImpl(BitmapAtlas::BitmapLayout const * layout, ParticleCorners const & corners, glm::vec3 const & color)
	{
		ParticleTexcoords texcoords = ParticleTools::GetParticleTexcoords(*layout);

		// extend the array
		size_t count = sprites.size();
		sprites.insert(sprites.end(), 6, SpriteVertex());
		SpriteVertex * new_vertices = &sprites[count];
		// fill the particles position and textures
		ParticleTools::GenerateBoxParticle(corners, texcoords, new_vertices);
		// fix the colour
		for (size_t i = 0; i < 6; ++i)
			new_vertices[i].color = color;
	}


	void SpriteManager::Display(GPUProgramProviderBase * uniform_provider)
	{
		if (sprites.size() == 0)
			return;

		UpdateGPUBuffer();

		// context states
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		// Initialize the vertex array
		glBindVertexArray(vertex_array->GetResourceID());

		GPUProgramProvider main_uniform_provider;
		main_uniform_provider.AddVariableTexture("material", atlas->GetTexture());
		if (uniform_provider != nullptr)
			main_uniform_provider.AddVariableProvider(uniform_provider);

		program->UseProgram(&main_uniform_provider);

		// The drawing   
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)sprites.size());

		// restore states
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glBindVertexArray(0);
	}

	void SpriteManager::UpdateGPUBuffer()
	{
		// fill GPU buffer
		size_t count = sprites.size();
		if (count > 0)
		{
			GLuint binding_index = 0;
			glNamedBufferData(vertex_buffer->GetResourceID(), count * sizeof(SpriteVertex), &sprites[0], GL_STATIC_DRAW);
			glVertexArrayVertexBuffer(vertex_array->GetResourceID(), binding_index, vertex_buffer->GetResourceID(), 0, declaration.GetVertexSize());
		}
	}
}; // namespace chaos
