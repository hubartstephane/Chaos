#include <chaos/ParticleDefault.h>

#include <chaos/GPUProgramGenerator.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/ParticleTools.h>

namespace chaos
{
	void ParticleToPrimitives(ParticleDefault const& particle, QuadOutput<VertexDefault>& output)
	{
		ParticleToPrimitive(particle, output.AddPrimitive());
	}

	void ParticleToPrimitives(ParticleDefault const& particle, TrianglePairOutput<VertexDefault>& output)
	{
		ParticleToPrimitive(particle, output.AddPrimitive());
	}

	void ParticleToPrimitive(ParticleDefault const& particle, QuadPrimitive<VertexDefault>& primitive)
	{
		// generate particle corners and texcoords
		ParticleTools::GenerateBoxParticle(primitive, particle.bounding_box, particle.texcoords, particle.rotation, particle.flags);
		// copy the color in all triangles vertex
		for (size_t i = 0; i < primitive.count; ++i)
			primitive[i].color = particle.color;
	}

	void ParticleToPrimitive(ParticleDefault const& particle, TrianglePairPrimitive<VertexDefault>& primitive)
	{
		// generate particle corners and texcoords
		ParticleTools::GenerateBoxParticle(primitive, particle.bounding_box, particle.texcoords, particle.rotation, particle.flags);
		// copy the color in all triangles vertex
		for (size_t i = 0; i < primitive.count; ++i)
			primitive[i].color = particle.color;
	}

	void GetTypedVertexDeclaration(GPUVertexDeclaration* result, boost::mpl::identity<VertexDefault>)
	{
		result->Push(chaos::VertexAttributeSemantic::POSITION, 0, chaos::VertexAttributeType::FLOAT2, "position");
		result->Push(chaos::VertexAttributeSemantic::TEXCOORD, 0, chaos::VertexAttributeType::FLOAT3, "texcoord");
		result->Push(chaos::VertexAttributeSemantic::COLOR, 0, chaos::VertexAttributeType::FLOAT4, "color");
		result->Push(chaos::VertexAttributeSemantic::NONE, -1, chaos::VertexAttributeType::INT1, "flags");
	}

	GPUProgram* GenDefaultParticleProgram()
	{
		char const* vertex_shader_source = R"VERTEXSHADERCODE(
		in vec2 position;
		in vec3 texcoord;
		in vec4 color;

		uniform mat4 local_to_cam;

		out vec3 vs_texcoord;
		out vec4 vs_color;

		void main()
	    {
			int flags = 0;
			vs_texcoord = DecodeTexcoord(texcoord, flags);
			vs_color    = color;
			gl_Position = local_to_cam * vec4(position.x, position.y, 0.0, 1.0);
		};											
		)VERTEXSHADERCODE";

		char const* pixel_shader_source = R"PIXELSHADERCODE(
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

	GPURenderMaterial* GenDefaultParticleMaterial()
	{
		shared_ptr<GPUProgram> program = GenDefaultParticleProgram(); // store a temporary object for lifetime management
		return GPURenderMaterial::GenRenderMaterialObject(program.get());
	}

}; // namespace chaos

