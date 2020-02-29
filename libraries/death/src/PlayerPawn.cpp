#include <death/PlayerPawn.h>
#include <death/GameCheckpoint.h>

namespace death
{
	// =============================================
	// PlayerPawn
	// =============================================

	glm::vec2 PlayerPawn::GetPosition() const
	{
		return glm::vec2(0.0f, 0.0f);
	}

	chaos::box2 PlayerPawn::GetBox() const
	{
		return chaos::box2();
	}

	bool PlayerPawn::SetPosition(glm::vec2 const& position)
	{
		return false;
	}
	bool PlayerPawn::SetBox(chaos::box2 const& box)
	{
		return false;
	}

	// =============================================
	// SimplePlayerPawn
	// =============================================

	chaos::ParticleDefault::Particle* SimplePlayerPawn::GetParticle()
	{
		return chaos::ParticleTools::GetParticle(GetAllocation(), 0);
	}

	chaos::ParticleDefault::Particle const* SimplePlayerPawn::GetParticle() const
	{
		return chaos::ParticleTools::GetParticle(GetAllocation(), 0);
	}

	glm::vec2 SimplePlayerPawn::GetPosition() const
	{
		return chaos::ParticleTools::GetParticlePosition(GetAllocation(), 0);
	}

	chaos::box2 SimplePlayerPawn::GetBox() const
	{
		return chaos::ParticleTools::GetParticleBox(GetAllocation(), 0);
	}

	bool SimplePlayerPawn::SetPosition(glm::vec2 const& position)
	{
		return chaos::ParticleTools::SetParticlePosition(GetAllocation(), 0, position);
	}
	bool SimplePlayerPawn::SetBox(chaos::box2 const& box)
	{
		return chaos::ParticleTools::SetParticleBox(GetAllocation(), 0, box);
	}

	void SimplePlayerPawn::SetAllocation(chaos::ParticleAllocationBase* in_allocation)
	{
		allocations = in_allocation;
	}

	
}; // namespace death

