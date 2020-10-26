#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    namespace ParticleTools
    {

    }; // namespace ParticleTools

}; // namespace chaos

#else 

namespace chaos
{

	/**
	* ParticleTools : an utility class that deserve to bring some services for particles system
	*/

	namespace ParticleTools
	{
        /** get particle by index */
        ParticleDefault* GetParticle(ParticleAllocationBase* allocation, size_t index);
        ParticleDefault const* GetParticle(ParticleAllocationBase const* allocation, size_t index);

        /** get particle position */
        std::optional<glm::vec2> GetParticlePosition(ParticleAllocationBase const* allocation, size_t index);
        /** get particle box */
        std::optional<box2> GetParticleBox(ParticleAllocationBase const* allocation, size_t index);


        // shu47

        /** get particle rotation */
        std::optional<float> GetParticleRotation(ParticleAllocationBase const* allocation, size_t index);

        /** set the particle position */
        bool SetParticlePosition(ParticleAllocationBase* allocation, size_t index, glm::vec2 const& position);
        /** set the particle box */
        bool SetParticleBox(ParticleAllocationBase* allocation, size_t index, box2 const& box);


        // shu47

        /** set the particle rotation */
        bool SetParticleRotation(ParticleAllocationBase* allocation, size_t index, float rotation);

	}; // namespace ParticleTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



