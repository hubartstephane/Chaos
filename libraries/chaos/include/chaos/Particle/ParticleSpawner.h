namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

    class ParticleSpawner;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

    class CHAOS_API ParticleSpawner : public Object
    {
    public:

        /** constructor */
        ParticleSpawner(BitmapAtlas::GPUAtlas const* in_atlas = nullptr, ParticleLayerBase* in_particle_layer = nullptr, ObjectRequest bitmap_request = ObjectRequest::None(), ObjectRequest folder_request = "sprites");
        /** constructor */
        ParticleSpawner(ParticleSpawner const& src) = default;

        /** get the texture atlas */
        BitmapAtlas::GPUAtlas const* GetTextureAtlas() const { return atlas; }

        /** change the bitmap info */
        bool SetBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request = "sprites"); // folder "sprites" by default
        /** find the bitmap info */
        BitmapAtlas::BitmapInfo const* FindBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request = "sprites") const;

        /** spawn particles */
        SpawnParticleResult SpawnParticles(size_t count, bool new_allocation = true);

        /** spawn a new text */
        SpawnParticleResult SpawnText(char const* in_text, bool new_allocation, ParticleTextGenerator::GeneratorParams const& params, ParticleTextGenerator::CreateTextAllocationParams const& allocation_params);

        /** gets the bitmap info used for spawned particles */
        BitmapAtlas::BitmapInfo const* GetBitmapInfo() const { return bitmap_info; }

        /** returns whether the spawner is valid */
        bool IsValid() const { return (particle_layer != nullptr); }
        /** returns whether the spawner has a bitmap info */
        bool HasBitmap() const { return (bitmap_info != nullptr); }

    protected:

        /** the layer instance where to spawn the particles */
        ParticleLayerBase* particle_layer = nullptr;
        /** the bitmap info to use for the spawned particles */
        BitmapAtlas::BitmapInfo const* bitmap_info = nullptr;
        /** the atlas used for the spawner */
        BitmapAtlas::GPUAtlas const* atlas = nullptr;
    };

#endif

}; // namespace chaos