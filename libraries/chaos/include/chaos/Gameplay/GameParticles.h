#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ParticleBackground;
	class ParticleBackgroundLayerTrait;

}; // namespace chaos

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

		/** output primitive */
        void ParticleToPrimitives(ParticleBackground const& particle, QuadOutput<VertexDefault> & output) const;
		/** output primitive */
        void ParticleToPrimitives(ParticleBackground const& particle, TrianglePairOutput<VertexDefault>& output) const;
	};

	CHAOS_REGISTER_CLASS1(ParticleBackground);

}; //namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
