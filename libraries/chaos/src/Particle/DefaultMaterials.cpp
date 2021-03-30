#include <chaos/Chaos.h>

namespace chaos
{

	/*
	 * DefaultParticleProgramSource implementation
	 */

	void DefaultParticleProgramSource::GetSources(GPUProgramGenerator & program_generator)
	{
		program_generator.AddShaderSource(GL_VERTEX_SHADER, vertex_shader_source);
		program_generator.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);
	}

	char const* DefaultParticleProgramSource::vertex_shader_source = R"VERTEXSHADERCODE(
			in vec2 position;
			in vec3 texcoord;
			in vec4 color;
			in int  flags;

			out vec2 vs_position;
			out vec3 vs_texcoord;
			out vec4 vs_color;
			out flat int vs_flags;

			uniform mat4 world_to_camera;
			uniform mat4 local_to_camera;
			uniform mat4 projection_matrix;

			uniform sampler2DArray material; // texture required in VS for Half pixel correction

			void main()
			{
				vs_position = position;
				vs_texcoord = HalfPixelCorrection(texcoord, flags, material);				
				vs_flags    = ExtractFragmentFlags(flags);
				vs_color    = color;

				gl_Position = projection_matrix * local_to_camera * vec4(position.x, position.y, 0.0, 1.0);
			}									
		)VERTEXSHADERCODE";

	char const* DefaultParticleProgramSource::pixel_shader_source = R"PIXELSHADERCODE(
			out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

			in vec2 vs_position;
			in vec3 vs_texcoord;
			in vec4 vs_color;
			in flat int  vs_flags;

			uniform sampler2DArray material;

			void main()
			{
				vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
				if (vs_texcoord.z >= 0.0)
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

	/*
	 * DefaultScreenSpaceProgramGenerator implementation
	 */

	void DefaultScreenSpaceProgramGenerator::GetSources(GPUProgramGenerator& program_generator)
	{
		program_generator.AddShaderSource(GL_VERTEX_SHADER, vertex_shader_source);
		program_generator.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);
	}

	char const* DefaultScreenSpaceProgramGenerator::vertex_shader_source = R"VERTEXSHADERCODE(
		in vec2 position;
		in vec3 texcoord;
		in vec4 color;
		in int  flags;

		out vec2 vs_position;
		out vec3 vs_texcoord;
		out vec4 vs_color;
		out flat int vs_flags;

		uniform vec4 canvas_box;

		uniform sampler2DArray material; // texture required in VS for Half pixel correction

		void main() 
		{
			vs_position = position;
			vs_texcoord = HalfPixelCorrection(texcoord, flags, material);
			vs_flags = ExtractFragmentFlags(flags);
			vs_color = color;

			gl_Position.xy = position / canvas_box.zw;
			gl_Position.z = 0.0;
			gl_Position.w = 1.0;
		}								
		)VERTEXSHADERCODE";

	char const* DefaultScreenSpaceProgramGenerator::pixel_shader_source = DefaultParticleProgramSource::pixel_shader_source;


}; // namespace chaos

