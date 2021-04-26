#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class TMParticle;
};

#else

namespace chaos
{
	// =====================================
	// TMParticle
	// =====================================

	class TMParticle : public ParticleDefault
	{
	public:

		// the ID for the "tile" (0 if the particle is not a tile)
		int gid = 0;
		// the bitmap for the particle
		BitmapAtlas::BitmapInfo const* bitmap_info = nullptr; 
	};

	CHAOS_REGISTER_CLASS2(TMParticle, ParticleDefault);

	// =====================================
	// TMParticleLayerTrait
	// =====================================

	class TMParticleLayerTrait : public ParticleLayerTrait<TMParticle, VertexDefault, GameEntity const *>
	{

	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION