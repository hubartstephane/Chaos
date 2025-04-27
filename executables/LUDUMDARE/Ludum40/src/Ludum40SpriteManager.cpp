#include "Ludum40PCH.h"
#include "Ludum40SpriteManager.h"
#include "Ludum40Game.h"


char const* SpriteManager::vertex_shader_source = R"VERTEX_SHADER(
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
	)VERTEX_SHADER";

char const* SpriteManager::fragment_shader_source = R"FRAGMENT_SHADER(
    in vec3 vs_texcoord;
    in vec3 vs_color;

    out vec4 output_color;

    uniform sampler2DArray material;

    void main()
    {
	vec4 color = (vs_texcoord.z < 0.0)?
		vec4(1.0, 1.0, 1.0, 1.0) :
		texture(material, vs_texcoord);
      output_color.xyz = color.xyz * vs_color;
      output_color.a   = color.a;
    };
	)FRAGMENT_SHADER";

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

bool SpriteManager::Initialize(Game * in_game, SpriteManagerInitParams& params)
{
	game = in_game;
	Finalize();
	if (!DoInitialize(params))
	{
		Finalize();
		return false;
	}
	return true;
}

bool SpriteManager::DoInitialize(SpriteManagerInitParams& params)
{
	if (params.atlas == nullptr)
		return false;

	atlas = params.atlas;

	// create GPU-Program
	if (params.program != nullptr)
		program = params.program;
	else
	{
		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSource(chaos::ShaderType::VERTEX, vertex_shader_source);
		program_generator.AddShaderSource(chaos::ShaderType::FRAGMENT, fragment_shader_source);

		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;
	}

	// prepare the vertex declaration
	declaration.Push(chaos::VertexAttributeSemantic::POSITION, 0, chaos::VertexAttributeType::FLOAT2);
	declaration.Push(chaos::VertexAttributeSemantic::TEXCOORD, 0, chaos::VertexAttributeType::FLOAT3);
	declaration.Push(chaos::VertexAttributeSemantic::COLOR, 0, chaos::VertexAttributeType::FLOAT3);


	// Generate Vertex Array and Buffer
	vertex_array = new chaos::GPUVertexArray(game->GetWindow());
	if (vertex_array == nullptr)
		return false;

	vertex_buffer = new chaos::GPUBuffer(true);
	if (vertex_buffer == nullptr)
		return false;

	chaos::GPUProgramData const& program_data = program->GetProgramData();
	program_data.BindAttributes(vertex_array->GetResourceID(), declaration, nullptr);

	return true;
}

chaos::ParticleCorners SpriteManager::GetParticleCorners(glm::vec2 const& position, glm::vec2 const& size, chaos::Hotpoint hotpoint) const
{
	chaos::ParticleCorners result;
	result.bottomleft = chaos::ConvertHotpointToBottomLeft(position, size, hotpoint);
	result.topright = result.bottomleft + size;
	return result;
}

void SpriteManager::AddSpriteCharacter(chaos::BitmapAtlas::AtlasCharacterInfo const* info, glm::vec2 const& position, glm::vec2 const& size, chaos::Hotpoint hotpoint, glm::vec3 const& color)
{
	chaos::ParticleCorners corners = GetParticleCorners(position, size, hotpoint);
	AddSpriteImpl(info, corners, color);
}

void SpriteManager::AddSpriteBitmap(chaos::BitmapAtlas::AtlasBitmapInfo const* info, glm::vec2 const& position, glm::vec2 const& size, chaos::Hotpoint hotpoint)
{
	static glm::vec3 const color(1.0f, 1.0f, 1.0f);

	chaos::ParticleCorners corners = GetParticleCorners(position, size, hotpoint);
	AddSpriteImpl(info, corners, color);
}


static void MyGenerateBoxParticle(chaos::ParticleCorners const& corners, chaos::ParticleTexcoords const& texcoords, SpriteVertex* vertices)
{
	float bitmap_index = (float)texcoords.bitmap_index;

	SpriteVertex bl;
	bl.position.x = corners.bottomleft.x;
	bl.position.y = corners.bottomleft.y;
	bl.texcoord.x = texcoords.bottomleft.x;
	bl.texcoord.y = texcoords.bottomleft.y;
	bl.texcoord.z = bitmap_index;

	SpriteVertex tr;
	tr.position.x = corners.topright.x;
	tr.position.y = corners.topright.y;
	tr.texcoord.x = texcoords.topright.x;
	tr.texcoord.y = texcoords.topright.y;
	tr.texcoord.z = bitmap_index;

	SpriteVertex tl;
	tl.position.x = corners.bottomleft.x;
	tl.position.y = corners.topright.y;
	tl.texcoord.x = texcoords.bottomleft.x;
	tl.texcoord.y = texcoords.topright.y;
	tl.texcoord.z = bitmap_index;

	SpriteVertex br;
	br.position.x = corners.topright.x;
	br.position.y = corners.bottomleft.y;
	br.texcoord.x = texcoords.topright.x;
	br.texcoord.y = texcoords.bottomleft.y;
	br.texcoord.z = bitmap_index;

	vertices[0] = bl;
	vertices[1] = br;
	vertices[2] = tr;

	vertices[3] = bl;
	vertices[4] = tr;
	vertices[5] = tl;
}

void SpriteManager::AddSpriteImpl(chaos::BitmapAtlas::AtlasBitmapLayout const* layout, chaos::ParticleCorners const& corners, glm::vec3 const& color)
{
	chaos::ParticleTexcoords texcoords = layout->GetTexcoords();

	// extend the array
	size_t count = sprites.size();
	sprites.insert(sprites.end(), 6, SpriteVertex());
	SpriteVertex* new_vertices = &sprites[count];
	// fill the particles position and textures
	MyGenerateBoxParticle(corners, texcoords, new_vertices);

	// fix the color
	for (size_t i = 0; i < 6; ++i)
		new_vertices[i].color = color;
}


void SpriteManager::Display(chaos::GPUProgramProviderBase* uniform_provider)
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

	chaos::GPUProgramProvider main_uniform_provider;
	main_uniform_provider.AddTexture("material", atlas->GetTexture());
	if (uniform_provider != nullptr)
		main_uniform_provider.AddProvider(uniform_provider);

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

