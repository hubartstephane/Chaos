#include <death/PlayerPawn.h>
#include <death/GameCheckpoint.h>
#include <death/GameEntityTools.h>

#include <death/GameEntityTools.h>

namespace death
{
	DEATH_GAMEENTITY_IMPLEMENTGETTERS(PlayerPawn);

	// =============================================
	// SimplePlayerPawn
	// =============================================

	chaos::ParticleDefault::Particle* PlayerPawn::GetParticle()
	{
		return chaos::ParticleTools::GetParticle(GetAllocation(), 0);
	}

	chaos::ParticleDefault::Particle const* PlayerPawn::GetParticle() const
	{
		return chaos::ParticleTools::GetParticle(GetAllocation(), 0);
	}

	glm::vec2 PlayerPawn::GetPosition() const
	{
		return chaos::ParticleTools::GetParticlePosition(GetAllocation(), 0);
	}

	chaos::box2 PlayerPawn::GetBox() const
	{
		return chaos::ParticleTools::GetParticleBox(GetAllocation(), 0);
	}

	bool PlayerPawn::SetPosition(glm::vec2 const& position)
	{
		return chaos::ParticleTools::SetParticlePosition(GetAllocation(), 0, position);
	}
	bool PlayerPawn::SetBox(chaos::box2 const& box)
	{
		return chaos::ParticleTools::SetParticleBox(GetAllocation(), 0, box);
	}

	void PlayerPawn::SetAllocation(chaos::ParticleAllocationBase* in_allocation)
	{
		allocations = in_allocation;
	}

	
}; // namespace death

