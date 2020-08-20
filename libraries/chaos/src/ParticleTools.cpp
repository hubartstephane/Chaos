#include <chaos/ParticleTools.h>
#include <chaos/Hotpoint.h>
#include <chaos/MathTools.h>
#include <chaos/ParticleLayer.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/GPUResourceManager.h>

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
            if (particles.GetCount() == 0)
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
            if (particles.GetCount() == 0)
                return nullptr;
            return &particles[index];
        }

        glm::vec2 GetParticlePosition(ParticleAllocationBase const* allocation, size_t index)
        {
            return GetParticleBox(allocation, index).position;
        }

        box2 GetParticleBox(ParticleAllocationBase const* allocation, size_t index)
        {
            ParticleDefault const* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return box2();
            return particle->bounding_box;
        }

        bool SetParticlePosition(ParticleAllocationBase* allocation, size_t index, glm::vec2 const& position)
        {
            ParticleDefault* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return false;
            particle->bounding_box.position = position;
            return true;
        }

        bool SetParticleBox(ParticleAllocationBase* allocation, size_t index, box2 const& box)
        {
            ParticleDefault* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return false;
            particle->bounding_box = box;
            return true;
        }

    }; // namespace ParticleTools

}; // namespace chaos

