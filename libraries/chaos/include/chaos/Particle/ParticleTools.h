namespace chaos
{

	/**
	* ParticleTools : an utility class that deserve to bring some services for particles system
	*/

	namespace ParticleTools
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

        /** get particle by index */
        CHAOS_API ParticleDefault* GetParticle(ParticleAllocationBase* allocation, size_t index);
        CHAOS_API ParticleDefault const* GetParticle(ParticleAllocationBase const* allocation, size_t index);

        /** get particle position */
        CHAOS_API std::optional<glm::vec2> GetParticlePosition(ParticleAllocationBase const* allocation, size_t index);
        /** get particle box */
        CHAOS_API std::optional<box2> GetParticleBox(ParticleAllocationBase const* allocation, size_t index);


        // shu47

        /** get particle rotation */
        CHAOS_API std::optional<float> GetParticleRotation(ParticleAllocationBase const* allocation, size_t index);

        /** set the particle position */
        CHAOS_API bool SetParticlePosition(ParticleAllocationBase* allocation, size_t index, glm::vec2 const& position);
        /** set the particle box */
        CHAOS_API bool SetParticleBox(ParticleAllocationBase* allocation, size_t index, box2 const& b);


        // shu47

        /** set the particle rotation */
        CHAOS_API bool SetParticleRotation(ParticleAllocationBase* allocation, size_t index, float rotation);

#endif

	}; // namespace ParticleTools

}; // namespace chaos