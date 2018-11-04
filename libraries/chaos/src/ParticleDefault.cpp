#include <chaos/ParticleDefault.h>
#include <chaos/GPUProgramGenerator.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPURenderMaterialLoader.h>

namespace chaos
{
	size_t ParticleDefault::ParticleTrait::ParticleToVertices(Particle const * particle, Vertex * vertices, size_t vertices_per_particle, ParticleAllocation * allocation) const
	{
		// generate particle corners and texcoords
		chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices);
		// copy the color in all triangles vertex
		for (size_t i = 0; i < 6; ++i)
			vertices[i].color = particle->color;

		return vertices_per_particle;
	}

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
		char const * vertex_shader_source = R"SHADERCODE(
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
		)SHADERCODE";

		char const * pixel_shader_source = R"SHADERCODE(
		in vec3 vs_texcoord;
		in vec4 vs_color;

		out vec4 output_color;

		uniform sampler2DArray material;

		void main()
		{
			vec4 color = texture(material, vs_texcoord);
			output_color = color * vs_color;
		};
		)SHADERCODE";

		GPUProgramGenerator program_generator;
		program_generator.AddShaderSource(GL_VERTEX_SHADER, vertex_shader_source);
		program_generator.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);
		return program_generator.GenProgramObject();
	}

	GPURenderMaterial * ParticleDefault::GenDefaultParticleMaterial()
	{
		boost::intrusive_ptr<GPUProgram> program = GenDefaultParticleProgram(); // store a temporary object for lifetime management
		return GPURenderMaterial::GenRenderMaterialObject(program.get());
	}

}; // namespace chaos

