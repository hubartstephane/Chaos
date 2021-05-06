#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ParticleCorners;
	class ParticleTexcoords;
	class ParticleDefault;
	class VertexDefault;

	/** ParticleTrait : the default trait */
	using ParticleDefaultLayerTrait = ParticleLayerTrait<ParticleDefault, VertexDefault>;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	template<typename VERTEX_TYPE>
	void ParticleToPrimitives(ParticleDefault const& particle, PrimitiveOutput<VERTEX_TYPE>& output)
	{
		QuadPrimitive<VERTEX_TYPE> quad = output.AddQuads();
		ParticleToPrimitive(particle, quad);
	}

	template<typename VERTEX_TYPE>
	void ParticleToPrimitive(ParticleDefault const& particle, QuadPrimitive<VERTEX_TYPE>& primitive)
	{
		// in order BL, BR, TR, TL
		glm::vec2 vertex_positions[4];
		GenerateVertexPositionAttributes(particle.bounding_box, particle.rotation, vertex_positions);

		glm::vec3 vertex_texcoords[4];
		GenerateVertexTextureAttributes(particle.texcoords, particle.flags, vertex_texcoords);

		int vertex_flags[4];
		GenerateVertexFlagAttributes(particle.flags, vertex_flags);

		for (int i = 0; i < 4; ++i)
		{
			VERTEX_TYPE& v = primitive[i];
			v.position = vertex_positions[i];
			v.texcoord = vertex_texcoords[i];
			v.flags = vertex_flags[i];
			v.color = particle.color;
		}
	}

	template<typename VERTEX_TYPE>
	void ParticleToPrimitive(ParticleDefault const& particle, TrianglePairPrimitive<VERTEX_TYPE>& primitive)
	{
		// in order BL, BR, TR, TL
		glm::vec2 vertex_positions[4];
		GenerateVertexPositionAttributes(particle.bounding_box, particle.rotation, vertex_positions);

		glm::vec3 vertex_texcoords[4];
		GenerateVertexTextureAttributes(particle.texcoords, particle.flags, vertex_texcoords);

		int vertex_flags[4];
		GenerateVertexFlagAttributes(particle.flags, vertex_flags);

		int const indices[] = { 0, 1, 2, 0, 2, 3 };

		for (int i = 0; i < 6; ++i)
		{
			int indice = indices[i];

			VERTEX_TYPE& v = primitive[i];
			v.position = vertex_positions[indice];
			v.texcoord = vertex_texcoords[indice];
			v.flags = vertex_flags[indice];
			v.color = particle.color;
		}
	}

}; // namespace chaos

#else 

namespace chaos
{

	// XXX : given a rectangle particle and a 4 textures coordinates, there are 8 possibilities to make them match (without twisting the geometry itself)
	//
	//       -4 rotations
	//       -1 optional symetry
	//
	//       these is given with the 3 bitfields

	namespace ParticleFlags
	{
		static constexpr int TEXTURE_HORIZONTAL_FLIP = (1 << 0);
		static constexpr int TEXTURE_VERTICAL_FLIP   = (1 << 1);
		static constexpr int TEXTURE_DIAGONAL_FLIP   = (1 << 2);
		static constexpr int HEIGHT_BITS_MODE        = (1 << 4); // whether we want to interpolate texel or have an old 8BIT MODE
	};

	namespace VertexFlags
	{
		static constexpr int BOTTOM_LEFT	= 1;
		static constexpr int BOTTOM_RIGHT	= 2;
		static constexpr int TOP_LEFT		= 3;
		static constexpr int TOP_RIGHT		= 4;
		static constexpr int CORNER_MASK    = 7;
		
		static constexpr int HEIGHT_BITS_MODE = (1 << 4);
	};

	static_assert(ParticleFlags::HEIGHT_BITS_MODE == VertexFlags::HEIGHT_BITS_MODE);

    /** ParticleCorners : represents 2 corners of a particle */
    class ParticleCorners
    {
    public:

        glm::vec2 bottomleft;
        glm::vec2 topright;
    };

	/** ParticleTexcoords : an object usefull for getting the texture coordinates of a sprite */
    class ParticleTexcoords : public ParticleCorners
    {
    public:

		int bitmap_index = 0;
    };


	/** Particle : a default particle, with simpler data */
	class ParticleDefault
	{
	public:

		/** the bounding box of the particle */
		box2 bounding_box;
		/** the four texcoords to apply to the particle vertices */
		ParticleTexcoords texcoords;
		/** the color of the particle */
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		/** the orientation of the particle */
		float rotation = 0.0f;
		/** some flags (more or less custom) */
		int flags = 0;
	};

	CHAOS_REGISTER_CLASS1(ParticleDefault);

	/** Vertex : vertex for default particle */
	class VertexDefault
	{
	public:

		/** the position of the vertex */
		glm::vec2 position;
		/** the texcoord of the vertex */
		glm::vec3 texcoord;
		/** the color of the vertex */
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		/** some flags */
		int flags = 0;
	};

	/** output primitive */
	template<typename VERTEX_TYPE>
	void ParticleToPrimitives(ParticleDefault const& particle, PrimitiveOutput<VERTEX_TYPE>& output);
	/** generates 1 quad from one particle */
	template<typename VERTEX_TYPE>
	void ParticleToPrimitive(ParticleDefault const& particle, QuadPrimitive<VERTEX_TYPE>& primitive);
	/** generates 1 triangle pair from one particle */
	template<typename VERTEX_TYPE>
	void ParticleToPrimitive(ParticleDefault const& particle, TrianglePairPrimitive<VERTEX_TYPE>& primitive);

	/** utility method to have position for a quad (in order BL, BR, TR, TL) */
	void GenerateVertexPositionAttributes(box2 const& bounding_box, float rotation, glm::vec2* vertex_positions);
	/** utility method to have texture coordinate for a quad (in order BL, BR, TR, TL) */
	void GenerateVertexTextureAttributes(ParticleTexcoords const& texcoords, int flags, glm::vec3* vertex_texcoords);
	/** utility method to have vertex flags from particle flags for a quad (in order BL, BR, TR, TL) */
	void GenerateVertexFlagAttributes(int flags, int* vertex_flags);

	/** the default vertex declaration */
	void GetTypedVertexDeclaration(GPUVertexDeclaration* result, boost::mpl::identity<VertexDefault>);

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



