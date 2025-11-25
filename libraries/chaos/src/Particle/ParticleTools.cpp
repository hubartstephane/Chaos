#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
    namespace ParticleTools
    {
        ParticleDefault* GetParticle(ParticleAllocationBase* allocation, size_t index)
        {
            if (allocation == nullptr)
                return nullptr;
            if (index >= allocation->GetParticleCount())
                return nullptr;

            ParticleAccessor<ParticleDefault> particles = allocation->GetParticleAccessor();
            if (particles.GetDataCount() == 0)
                return nullptr;
            return &particles[index];
        }

        ParticleDefault const* GetParticle(ParticleAllocationBase const* allocation, size_t index)
        {
            if (allocation == nullptr)
                return nullptr;
            if (index >= allocation->GetParticleCount())
                return nullptr;

            ParticleConstAccessor<ParticleDefault> particles = allocation->GetParticleAccessor();
            if (particles.GetDataCount() == 0)
                return nullptr;
            return &particles[index];
        }

        std::optional<glm::vec2> GetParticlePosition(ParticleAllocationBase const* allocation, size_t index)
        {
            ParticleDefault const* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return {};
            return particle->bounding_box.position;
        }

        std::optional<box2> GetParticleBox(ParticleAllocationBase const* allocation, size_t index)
        {
            ParticleDefault const* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return {};
            return particle->bounding_box;
        }


            // shu47

        std::optional<float> GetParticleRotation(ParticleAllocationBase const* allocation, size_t index)
        {
            ParticleDefault const* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return 0.0f;
            return particle->rotation;
        }











        bool SetParticlePosition(ParticleAllocationBase* allocation, size_t index, glm::vec2 const& position)
        {
            ParticleDefault* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return false;
            particle->bounding_box.position = position;
            return true;
        }

        bool SetParticleBox(ParticleAllocationBase* allocation, size_t index, box2 const& b)
        {
            ParticleDefault* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return false;
            particle->bounding_box = b;
            return true;
        }


            // shu47

        bool SetParticleRotation(ParticleAllocationBase* allocation, size_t index, float rotation)
        {
            ParticleDefault* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return false;
            particle->rotation = rotation;
            return true;
        }


    }; // namespace ParticleTools

}; // namespace chaos

