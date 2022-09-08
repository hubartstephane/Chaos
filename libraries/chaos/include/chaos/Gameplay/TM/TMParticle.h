namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class TMParticle;

	using TMParticleLayerTrait = ParticleLayerTrait<TMParticle, VertexDefault, GameEntity const*>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =====================================
	// TMParticle
	// =====================================

	class CHAOS_API TMParticle : public ParticleDefault
	{
	public:

		// the ID for the "tile" (0 if the particle is not a tile)
		int gid = 0;
		// the bitmap for the particle
		BitmapAtlas::BitmapInfo const* bitmap_info = nullptr;
	};

	CHAOS_REGISTER_CLASS(TMParticle, ParticleDefault);

#endif

}; // namespace chaos
