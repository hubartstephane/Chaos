#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    class GameParticleCreator;

}; // namespace chaos

#else

namespace chaos
{
	class GameParticleCreator
	{
	public:

		/** initialization of the particle creator */
		bool Initialize(chaos::ParticleManager * in_particle_manager, chaos::ParticleTextGenerator::Generator * in_particle_text_generator, chaos::BitmapAtlas::TextureArrayAtlas * in_texture_atlas);

		/** create some particle of the given type (Spawn + Initialization) */
		chaos::ParticleAllocationBase * SpawnParticles(chaos::ObjectRequest layer_id, chaos::ObjectRequest bitmap_request, size_t count, bool new_allocation) const;

        /** spawn + user initialization methods */
        template<typename INIT_PARTICLE_FUNC>
        chaos::ParticleAllocationBase* SpawnParticles(chaos::ObjectRequest layer_id, chaos::ObjectRequest bitmap_request, size_t count, bool new_allocation, INIT_PARTICLE_FUNC init_func) const
        {
            chaos::ParticleAllocationBase* result = SpawnParticles(layer_id, bitmap_request, count, new_allocation);
            // call user initialization function
            if (result != nullptr)
            {
                size_t allocation_count = result->GetParticleCount();
                init_func(result->GetParticleAccessor(allocation_count - count, count));  // partial accessor, take the last particles in the array
            }
            return result;
        }

		/** create a text particle system */
		chaos::ParticleAllocationBase * SpawnTextParticles(chaos::ObjectRequest layer_id, char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params) const;

        /** spawn + user initialization methods */
        template<typename INIT_PARTICLE_FUNC>
        chaos::ParticleAllocationBase* SpawnTextParticles(chaos::ObjectRequest layer_id, char const* text, chaos::ParticleTextGenerator::GeneratorParams const& params, INIT_PARTICLE_FUNC init_func) const
        {
            chaos::ParticleAllocationBase* result = SpawnTextParticles(layer_id, text, params);
            // call user initialization function
            if (result != nullptr)
            {
                size_t allocation_count = result->GetParticleCount();
                init_func(result->GetParticleAccessor(allocation_count - count, count));  // partial accessor, take the last particles in the array
            }
            return result;
        }

		/** get the bitmap info used for given bitmap_name */
		chaos::BitmapAtlas::BitmapInfo const * FindBitmapInfo(chaos::ObjectRequest bitmap_request) const;

	protected:

		/** the particle manager */
		chaos::shared_ptr<chaos::ParticleManager> particle_manager;
		/** the text generator */
		chaos::shared_ptr<chaos::ParticleTextGenerator::Generator> particle_text_generator;
		/** the texture atlas */
		chaos::shared_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;
	};

}; //namespace chaos

#endif // CHAOS_FORWARD_DECLARATION