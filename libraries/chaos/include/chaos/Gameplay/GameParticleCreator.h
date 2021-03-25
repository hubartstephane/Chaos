#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    class GameParticleCreator;

}; // namespace chaos

#else

namespace chaos
{
	class GameParticleCreator : public Object
	{
	public:

		/** initialization of the particle creator */
		bool Initialize(ParticleManager * in_particle_manager, ParticleTextGenerator::Generator * in_particle_text_generator, BitmapAtlas::TextureArrayAtlas * in_texture_atlas);

		/** create some particle of the given type (Spawn + Initialization) */
		ParticleAllocationBase * SpawnParticles(ObjectRequest layer_id, ObjectRequest bitmap_request, size_t count, bool new_allocation) const;

        /** spawn + user initialization methods */
        template<typename INIT_PARTICLE_FUNC>
        ParticleAllocationBase* SpawnParticles(ObjectRequest layer_id, ObjectRequest bitmap_request, size_t count, bool new_allocation, INIT_PARTICLE_FUNC init_func) const
        {
            ParticleAllocationBase* result = SpawnParticles(layer_id, bitmap_request, count, new_allocation);
            // call user initialization function
            if (result != nullptr)
            {
                size_t allocation_count = result->GetParticleCount();
                init_func(result->GetParticleAccessor(allocation_count - count, count));  // partial accessor, take the last particles in the array
            }
            return result;
        }

		/** create a text particle system */
		ParticleAllocationBase * SpawnTextParticles(ObjectRequest layer_id, char const * text, ParticleTextGenerator::GeneratorParams const & params) const;

        /** spawn + user initialization methods */
        template<typename INIT_PARTICLE_FUNC>
        ParticleAllocationBase* SpawnTextParticles(ObjectRequest layer_id, char const* text, ParticleTextGenerator::GeneratorParams const& params, INIT_PARTICLE_FUNC init_func) const
        {
            ParticleAllocationBase* result = SpawnTextParticles(layer_id, text, params);
            // call user initialization function
            if (result != nullptr)
            {
                size_t allocation_count = result->GetParticleCount();
                init_func(result->GetParticleAccessor(allocation_count - count, count));  // partial accessor, take the last particles in the array
            }
            return result;
        }

		/** get the bitmap info used for given bitmap_name */
		BitmapAtlas::BitmapInfo const * FindBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request = "sprites") const;

	protected:

		/** the particle manager */
		shared_ptr<ParticleManager> particle_manager;
		/** the text generator */
		shared_ptr<ParticleTextGenerator::Generator> particle_text_generator;
		/** the texture atlas */
		shared_ptr<BitmapAtlas::TextureArrayAtlas> texture_atlas;
	};

}; //namespace chaos

#endif // CHAOS_FORWARD_DECLARATION