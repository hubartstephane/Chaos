#include <chaos/ParticleDefault.h>
#include <chaos/GPUProgramGenerator.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPURenderMaterialLoader.h>

namespace chaos
{
	GPUVertexDeclaration ParticleDefault::GetTypedVertexDeclaration(boost::mpl::identity<ParticleDefault::Vertex>)
	{
		chaos::GPUVertexDeclaration result;
		result.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
		result.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT3);
		result.Push(chaos::SEMANTIC_COLOR, 0, chaos::TYPE_FLOAT4);
		return result;
	}

	GPUProgram * ParticleDefault::GenDefaultParticleProgram()
	{
		char const * vertex_shader_source = R"VERTEXSHADERCODE(
		in vec2 position;
		in vec3 texcoord;
		in vec4 color;

		uniform mat4 local_to_cam;

		out vec3 vs_texcoord;
		out vec4 vs_color;

		void main()
	    {
			vs_texcoord = texcoord;
			vs_color    = color;
			gl_Position = local_to_cam * vec4(position.x, position.y, 0.0, 1.0);
		};											
		)VERTEXSHADERCODE";

		char const * pixel_shader_source = R"PIXELSHADERCODE(
		in vec3 vs_texcoord;
		in vec4 vs_color;

		out vec4 output_color;

		uniform sampler2DArray material;

		void main()
		{
			vec4 color = (vs_texcoord.x < 0.0 || vs_texcoord.y < 0.0)? 
				vec4(1.0, 1.0, 1.0, 1.0) : 
				texture(material, vs_texcoord);
			output_color = color * vs_color;
		};
		)PIXELSHADERCODE";

		GPUProgramGenerator program_generator;
		program_generator.AddShaderSource(GL_VERTEX_SHADER, vertex_shader_source);
		program_generator.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);
		return program_generator.GenProgramObject();
	}

	GPURenderMaterial * ParticleDefault::GenDefaultParticleMaterial()
	{
		shared_ptr<GPUProgram> program = GenDefaultParticleProgram(); // store a temporary object for lifetime management
		return GPURenderMaterial::GenRenderMaterialObject(program.get());
	}

	chaos::ParticleDefault::Particle * ParticleDefault::GetParticle(chaos::ParticleAllocationBase * allocation, size_t index)
	{
		if (allocation == nullptr)
			return nullptr;
		if (index >= allocation->GetParticleCount())
			return nullptr;

		chaos::ParticleAccessor<chaos::ParticleDefault::Particle> particles = allocation->GetParticleAccessor();
		if (particles.GetCount() == 0)
			return nullptr;
		return &particles[index];
	}

	chaos::ParticleDefault::Particle const * ParticleDefault::GetParticle(chaos::ParticleAllocationBase const * allocation, size_t index)
	{
		if (allocation == nullptr)
			return nullptr;
		if (index >= allocation->GetParticleCount())
			return nullptr;

		chaos::ParticleConstAccessor<chaos::ParticleDefault::Particle> particles = allocation->GetParticleAccessor();
		if (particles.GetCount() == 0)
			return nullptr;
		return &particles[index];
	}

	glm::vec2 ParticleDefault::GetParticlePosition(chaos::ParticleAllocationBase const * allocation, size_t index)
	{
		return GetParticleBox(allocation, index).position;
	}

	chaos::box2 ParticleDefault::GetParticleBox(chaos::ParticleAllocationBase const * allocation, size_t index)
	{
		chaos::ParticleDefault::Particle const * particle = GetParticle(allocation, index);
		if (particle == nullptr)
			return chaos::box2();
		return particle->bounding_box;
	}

	bool ParticleDefault::SetParticlePosition(chaos::ParticleAllocationBase * allocation, size_t index, glm::vec2 const & position)
	{
		chaos::ParticleDefault::Particle * particle = GetParticle(allocation, index);
		if (particle == nullptr)
			return false;
		particle->bounding_box.position = position;
		return true;
	}

	bool ParticleDefault::SetParticleBox(chaos::ParticleAllocationBase * allocation, size_t index, chaos::box2 const & box)
	{
		chaos::ParticleDefault::Particle * particle = GetParticle(allocation, index);
		if (particle == nullptr)
			return false;
		particle->bounding_box = box;
		return true;
	}

}; // namespace chaos

