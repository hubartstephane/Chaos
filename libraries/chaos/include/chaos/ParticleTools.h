#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Class.h>
#include <chaos/PrimitiveOutput.h>
#include <chaos/ParticleDefault.h>
#include <chaos/Hotpoint.h>

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

        /** set the particle position */
        bool SetParticlePosition(ParticleAllocationBase* allocation, size_t index, glm::vec2 const& position);
        /** set the particle box */
        bool SetParticleBox(ParticleAllocationBase* allocation, size_t index, box2 const& box);

	}; // namespace ParticleTools

}; // namespace chaos

