#ifdef CHAOS_FORWARD_DECLARATION

class TMParticle;

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
	public:

#if 0

		void ParticleToPrimitives(TMParticle const& p, QuadOutput<VertexDefault>& output) const
		{

	
		}

		void ParticleToPrimitives(TMParticle const& p, TrianglePairOutput<VertexDefault>& output) const
		{
			auto primitive = output.AddPrimitive();
			ParticleToPrimitive(p, primitive);
			if ((p.flags & TiledMap::TileParticleFlags::NEIGHBOUR_LEFT) != 0)
				for (auto & v : primitive)
					v.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

	public:

		//template<typename T>
		glm::mat4 BeginParticlesToPrimitives(ParticleConstAccessor<TMParticle>& accessor, GameEntity const * allocation_data) const
		{
			if (allocation_data == nullptr)
				return glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
			return {};
		}

		//template<typename T>
		void ParticleToPrimitives(TMParticle const & particle, QuadOutput<VertexDefault>& output, glm::mat4 const& m, GameEntity const * allocation_trait) const
		{
			QuadPrimitive<VertexDefault> primitive = output.AddPrimitive();
			ParticleToPrimitive(particle, primitive);

			for (auto& v : primitive)
				v.color.r = 0.0f;

		}
#endif
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION