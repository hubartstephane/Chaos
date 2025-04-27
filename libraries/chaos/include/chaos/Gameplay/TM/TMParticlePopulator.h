namespace chaos
{

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(ParticleLog, "Particle")

	// =====================================
	// TMParticlePopulator : utility class to generate particles for a layer with a cache
	// =====================================

	class CHAOS_API TMParticlePopulator
	{
		static size_t const PARTICLE_BUFFER_SIZE = 256;

	public:

		/** default constructor */
		TMParticlePopulator() = default;
		/** copy constructor (do not copy everything) */
		TMParticlePopulator(TMParticlePopulator const& src);

		/** initialize the object */
		bool Initialize(TMLayerInstance* in_layer_instance);
		/** insert a new particle */
		bool AddParticle(char const* bitmap_name, Hotpoint hotpoint, box2 particle_box, glm::vec4 const& color, float rotation, int particle_flags, int gid, bool keep_aspect_ratio);
		/** flush remaining particles */
		bool FlushParticles();

		/** get the final bounding box */
		box2 const& GetBoundingBox() const { return bounding_box; }
		/** get the particle allocation */
		ParticleAllocationBase* GetParticleAllocation() { return allocation; }

		/** copy operator (do not copy cached particles) */
		TMParticlePopulator& operator = (TMParticlePopulator const& src);

	protected:

		/** 'copy' the cached particle into the allocation (with type conversion) */
		bool FlushCachedParticlesToAllocation();

	protected:

		/** the concerned layer instance */
		TMLayerInstance* layer_instance = nullptr;
		/** the level owning this layer */
		TMLevel* level = nullptr;

		/** the texture atlas required */
		BitmapAtlas::GPUAtlas const* texture_atlas = nullptr;
		/** the folder containing the bitmaps */
		BitmapAtlas::FolderInfo const* folder_info = nullptr;

		/** the allocation for all those particles */
		ParticleAllocationBase* allocation = nullptr;

		/** a cache of particles */
		TMParticle particles[PARTICLE_BUFFER_SIZE];
		/** the cached number of particles */
		size_t particle_count = 0;
		/** a bounding box */
		box2 bounding_box;
	};

#endif

}; // namespace chaos