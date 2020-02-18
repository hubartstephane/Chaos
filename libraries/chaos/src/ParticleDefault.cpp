#include <chaos/ParticleDefault.h>

#include <chaos/GPUProgramGenerator.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/GPURenderMaterialLoader.h>
#include <chaos/ParticleTools.h>

namespace chaos
{
    namespace ParticleDefault
    {
        void ParticleTrait::ParticleToPrimitives(Particle const& particle, TrianglePairOutput<Vertex>& output)
        {
            ParticleToPrimitive(particle, output.AddPrimitive());
        }

        void ParticleTrait::ParticleToPrimitives(Particle const& particle, QuadOutput<Vertex>& output)
        {
            ParticleToPrimitive(particle, output.AddPrimitive());
        }

        void ParticleTrait::ParticleToPrimitive(Particle const& particle, QuadPrimitive<Vertex>& primitive)
        {
            // generate particle corners and texcoords
            ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
            // copy the color in all triangles vertex
            for (size_t i = 0; i < primitive.count; ++i)
                primitive[i].color = particle.color;
        }

        void ParticleTrait::ParticleToPrimitive(Particle const& particle, TrianglePairPrimitive<Vertex>& primitive)
        {
            // generate particle corners and texcoords
            ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
            // copy the color in all triangles vertex
            for (size_t i = 0; i < primitive.count; ++i)
                primitive[i].color = particle.color;
        }

        void GetTypedVertexDeclaration(GPUVertexDeclaration * result, boost::mpl::identity<Vertex>)
        {
			result->Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
            result->Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT3);
            result->Push(chaos::SEMANTIC_COLOR, 0, chaos::TYPE_FLOAT4);
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
			vs_texcoord = texcoord;
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

    }; // namespace ParticleDefault

}; // namespace chaos

