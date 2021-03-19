#include <chaos/Chaos.h>

namespace chaos
{
	void GenerateVertexPositionAttributes(box2 const& bounding_box, float rotation, glm::vec2* vertex_positions) // in order BL, BR, TR, TL
	{
		std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(bounding_box);

		glm::vec2 const & bottomleft = corners.first;
		glm::vec2 const & topright   = corners.second;

		// compute the vertices
		vertex_positions[0] = glm::vec2(bottomleft.x, bottomleft.y);
		vertex_positions[1] = glm::vec2(topright.x, bottomleft.y);
		vertex_positions[2] = glm::vec2(topright.x, topright.y);
		vertex_positions[3] = glm::vec2(bottomleft.x, topright.y);

		// apply the rotation
		if (rotation != 0.0f)
		{
			glm::vec2 center_position = (bottomleft + topright) * 0.5f;

			float c = std::cos(rotation);
			float s = std::sin(rotation);
			for (int i = 0; i < 4; ++i)
				vertex_positions[i] = GLMTools::Rotate(vertex_positions[i] - center_position, c, s) + center_position;
		}
	}

	void GenerateVertexTextureAttributes(ParticleTexcoords const& texcoords, int flags, glm::vec3* vertex_texcoords) // in order BL, BR, TR, TL
	{
		int bitmap_index = texcoords.bitmap_index;

		// compute the vertices
		vertex_texcoords[0] = glm::vec3(texcoords.bottomleft.x, texcoords.bottomleft.y, bitmap_index);
		vertex_texcoords[1] = glm::vec3(texcoords.topright.x, texcoords.bottomleft.y, bitmap_index);
		vertex_texcoords[2] = glm::vec3(texcoords.topright.x, texcoords.topright.y, bitmap_index);
		vertex_texcoords[3] = glm::vec3(texcoords.bottomleft.x, texcoords.topright.y, bitmap_index);

		// apply texture symetries
		if ((flags & ParticleFlags::TEXTURE_DIAGONAL_FLIP) != 0)
		{
			std::swap(vertex_texcoords[0], vertex_texcoords[2]);
		}
		if ((flags & ParticleFlags::TEXTURE_HORIZONTAL_FLIP) != 0)
		{
			std::swap(vertex_texcoords[0], vertex_texcoords[1]);
			std::swap(vertex_texcoords[2], vertex_texcoords[3]);
		}
		if ((flags & ParticleFlags::TEXTURE_VERTICAL_FLIP) != 0)
		{
			std::swap(vertex_texcoords[0], vertex_texcoords[3]);
			std::swap(vertex_texcoords[1], vertex_texcoords[2]);
		}
	}

	void GenerateVertexFlagAttributes(int flags, int* vertex_flags) // in order BL, BR, TR, TL
	{
		// just keep the HEIGHT_BITS_MODE flag (VertexFlags::HEIGHT_BITS_MODE == ParticleFlags::HEIGHT_BITS_MODE)
		int output_flags = (flags & ParticleFlags::HEIGHT_BITS_MODE);

		vertex_flags[0] = VertexFlags::BOTTOM_LEFT | output_flags;
		vertex_flags[1] = VertexFlags::BOTTOM_RIGHT | output_flags;
		vertex_flags[2] = VertexFlags::TOP_RIGHT | output_flags;
		vertex_flags[3] = VertexFlags::TOP_LEFT | output_flags;
	}

	void ParticleToPrimitives(ParticleDefault const& particle, PrimitiveOutput<VertexDefault>& output)
	{
		ParticleToPrimitive(particle, output.AddQuads());
	}

	void ParticleToPrimitive(ParticleDefault const& particle, QuadPrimitive<VertexDefault>& primitive)
	{
		// in order BL, BR, TR, TL
		glm::vec2 vertex_positions[4];
		GenerateVertexPositionAttributes(particle.bounding_box, particle.rotation, vertex_positions);

		glm::vec3 vertex_texcoords[4];
		GenerateVertexTextureAttributes(particle.texcoords, particle.flags, vertex_texcoords);

		int vertex_flags[4];
		GenerateVertexFlagAttributes(particle.flags, vertex_flags);

		VertexDefault& v0 = primitive[0];
		VertexDefault& v1 = primitive[1];
		VertexDefault& v2 = primitive[2];
		VertexDefault& v3 = primitive[3];

		v0.position = vertex_positions[0];
		v0.texcoord = vertex_texcoords[0];
		v0.flags = vertex_flags[0];
		v0.color = particle.color;

		v1.position = vertex_positions[1];
		v1.texcoord = vertex_texcoords[1];
		v1.flags = vertex_flags[1];
		v1.color = particle.color;

		v2.position = vertex_positions[2];
		v2.texcoord = vertex_texcoords[2];
		v2.flags = vertex_flags[2];
		v2.color = particle.color;

		v3.position = vertex_positions[3];
		v3.texcoord = vertex_texcoords[3];
		v3.flags = vertex_flags[3];
		v3.color = particle.color;
	}

	void ParticleToPrimitive(ParticleDefault const& particle, TrianglePairPrimitive<VertexDefault>& primitive)
	{
		// in order BL, BR, TR, TL
		glm::vec2 vertex_positions[4];
		GenerateVertexPositionAttributes(particle.bounding_box, particle.rotation, vertex_positions);

		glm::vec3 vertex_texcoords[4];
		GenerateVertexTextureAttributes(particle.texcoords, particle.flags, vertex_texcoords);

		int vertex_flags[4];
		GenerateVertexFlagAttributes(particle.flags, vertex_flags);

		VertexDefault& v0 = primitive[0];
		VertexDefault& v1 = primitive[1];
		VertexDefault& v2 = primitive[2];
		VertexDefault& v3 = primitive[3];
		VertexDefault& v4 = primitive[4];
		VertexDefault& v5 = primitive[5];

		v0.position = vertex_positions[0];
		v0.texcoord = vertex_texcoords[0];
		v0.flags = vertex_flags[0];
		v0.color = particle.color;

		v1.position = vertex_positions[1];
		v1.texcoord = vertex_texcoords[1];
		v1.flags = vertex_flags[1];
		v1.color = particle.color;

		v2.position = vertex_positions[2];
		v2.texcoord = vertex_texcoords[2];
		v2.flags = vertex_flags[2];
		v2.color = particle.color;

		v3.position = vertex_positions[0];
		v3.texcoord = vertex_texcoords[0];
		v3.flags = vertex_flags[0];
		v3.color = particle.color;

		v4.position = vertex_positions[2];
		v4.texcoord = vertex_texcoords[2];
		v4.flags = vertex_flags[2];
		v4.color = particle.color;

		v5.position = vertex_positions[3];
		v5.texcoord = vertex_texcoords[3];
		v5.flags = vertex_flags[3];
		v5.color = particle.color;
	}

	void GetTypedVertexDeclaration(GPUVertexDeclaration* result, boost::mpl::identity<VertexDefault>)
	{
		result->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT2, "position");
		result->Push(VertexAttributeSemantic::TEXCOORD, 0, VertexAttributeType::FLOAT3, "texcoord");
		result->Push(VertexAttributeSemantic::COLOR, 0, VertexAttributeType::FLOAT4, "color");
		result->Push(VertexAttributeSemantic::NONE, -1, VertexAttributeType::INT1, "flags");
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

