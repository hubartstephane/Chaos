#include <death/GameEntity.h>

#include <chaos/JSONTools.h>
#include <chaos/ParticleTools.h>

namespace death
{

	glm::vec2 GameEntity::GetPosition() const
	{
		if (is_particle_master)
		{
			std::optional<glm::vec2> position_opt = chaos::ParticleTools::GetParticlePosition(allocations.get(), 0);
			if (position_opt)
				return *position_opt;
		}
		return bounding_box.position;
	}

	chaos::box2 GameEntity::GetBoundingBox() const
	{
		if (is_particle_master)
		{
			std::optional<chaos::box2> box_opt = chaos::ParticleTools::GetParticleBox(allocations.get(), 0);
			if (box_opt)
				return *box_opt;
		}
		return bounding_box;
	}

	void GameEntity::SetPosition(glm::vec2 const& in_position)
	{
		if (is_particle_master)
			if (chaos::ParticleTools::SetParticlePosition(allocations.get(), 0, in_position))
				return;
		bounding_box.position = in_position;
	}
	void GameEntity::SetBoundingBox(chaos::box2 const& in_bounding_box)
	{
		if (is_particle_master)
			if (chaos::ParticleTools::SetParticleBox(allocations.get(), 0, in_bounding_box))
				return;
		bounding_box = in_bounding_box;
	}

	void GameEntity::OnBoundingBoxChanged(chaos::box2 const& new_bounding_box, chaos::box2 const& old_bounding_box)
	{
		SynchronizeData(false);
	}

	void GameEntity::SynchronizeData(bool particle_to_entity)
	{
		if (particle_to_entity)
		{
			std::optional<chaos::box2> box_opt = chaos::ParticleTools::GetParticleBox(allocations.get(), 0);
			if (box_opt)
				bounding_box = *box_opt;
		}
		else
		{
			chaos::ParticleTools::SetParticleBox(allocations.get(), 0, bounding_box);
		}
	}

	void GameEntity::SetAllocation(chaos::ParticleAllocationBase* in_allocation)
	{
		allocations = in_allocation;
	}
	
	bool GameEntity::SerializeFromJSON(nlohmann::json const& entry)
	{
		if (!chaos::JSONSerializable::SerializeFromJSON(entry))
			return false;

		chaos::box2 b;
		if (chaos::JSONTools::GetAttribute(entry, "BOUNDING_BOX", b))
			SetBoundingBox(b);

		return true;
	}

	bool GameEntity::SerializeIntoJSON(nlohmann::json& entry) const
	{
		if (!chaos::JSONSerializable::SerializeIntoJSON(entry))
			return false;
		chaos::JSONTools::SetAttribute(entry, "BOUNDING_BOX", GetBoundingBox());
		return true;
	}
	
}; // namespace death

