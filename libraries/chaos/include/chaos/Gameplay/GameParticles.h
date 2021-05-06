#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ParticleBackground;
	class ParticleBackgroundLayerTrait;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{

	// ===========================================================================
	// Background particle system
	// ===========================================================================

	class ParticleBackground
	{
	public:

		glm::vec4 color;
	};

	class ParticleBackgroundLayerTrait : public ParticleLayerTrait<ParticleBackground, VertexDefault>
	{
	public:

		/** constructor */
		ParticleBackgroundLayerTrait(){ dynamic_particles = dynamic_vertices = false;}
		/** copy constructor */
		ParticleBackgroundLayerTrait(ParticleBackgroundLayerTrait const& src) = default;
	};

	/** output primitive */
	void ParticleToPrimitives(ParticleBackground const& particle, PrimitiveOutput<VertexDefault>& output);
	/** generates 1 quad from one particle */
	void ParticleToPrimitive(ParticleBackground const& particle, QuadPrimitive<VertexDefault>& primitive);
	/** generates 1 triangle pair from one particle */
	void ParticleToPrimitive(ParticleBackground const& particle, TrianglePairPrimitive<VertexDefault>& primitive);

	CHAOS_REGISTER_CLASS1(ParticleBackground);

}; //namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
