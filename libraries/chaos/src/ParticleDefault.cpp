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
			in int  flags;

			out vec2 vs_position;
			out vec3 vs_texcoord;
			out vec4 vs_color;
			out flat int vs_flags;

			uniform vec2 offset;
			uniform mat4 camera_transform;
			uniform vec4 camera_box;

			uniform sampler2DArray material; // texture required in VS for Half pixel correction

			void main()
			{
				vec2 pos = position + offset;

				vs_position = pos;
				vs_texcoord = HalfPixelCorrection(texcoord, flags, material);				
				vs_flags    = ExtractFragmentFlags(flags);
				vs_color    = color;

				vec4 transformed_pos = camera_transform * vec4(pos.x, pos.y, 0.0, 1.0);

				gl_Position.xy = transformed_pos.xy / camera_box.zw;
				gl_Position.z = 0.0;
				gl_Position.w = 1.0;
			}									
		)VERTEXSHADERCODE";

		char const* pixel_shader_source = R"PIXELSHADERCODE(
			out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

			in vec2 vs_position;
			in vec3 vs_texcoord;
			in vec4 vs_color;
			in flat int  vs_flags;

			uniform sampler2DArray material;

			void main()
			{
				vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
				if (vs_texcoord.x >= 0.0 && vs_texcoord.y >= 0.0)
				{
					// Using texel interpolation
					if ((vs_flags & HEIGHT_BITS_MODE) == 0)
					{
						color = texture(material, vs_texcoord);
					}
					// Not using texel interpolation
					else
					{					
						ivec3 size = textureSize(material, 0);
						ivec3 coord;
						coord.xy = ivec2((vs_texcoord.xy * vec2(size.xy)));
						coord.z = int(vs_texcoord.z);	
						color = texelFetch(material, coord, 0);
					}
				}
				// compute final color
				output_color.xyz = color.xyz * vs_color.xyz;
				output_color.a   = vs_color.a * color.a;
			};
		)PIXELSHADERCODE";

		chaos::GPUProgramGenerator program_generator;
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

