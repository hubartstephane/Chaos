#include <chaos/Chaos.h>

namespace chaos
{

	glm::vec2 GameEntity::GetPosition() const
	{
		if (is_particle_master)
		{
			std::optional<glm::vec2> position_opt = ParticleTools::GetParticlePosition(allocations.get(), 0);
			if (position_opt)
				return *position_opt;
		}
		return bounding_box.position;
	}

	box2 GameEntity::GetBoundingBox() const
	{
		if (is_particle_master)
		{
			std::optional<box2> box_opt = ParticleTools::GetParticleBox(allocations.get(), 0);
			if (box_opt)
				return *box_opt;
		}
		return bounding_box;
	}



	// shu47

	obox2 GameEntity::GetOBox() const
	{
		box2 ob = GetBoundingBox();

		obox2 result;
		result.position = ob.position;
		result.half_size = ob.half_size;
		result.rotator = GetRotation();

		return result;
	}

	float GameEntity::GetRotation() const
	{
		if (is_particle_master)
		{
			std::optional<float> rotation_opt = ParticleTools::GetParticleRotation(allocations.get(), 0);
			if (rotation_opt)
				return *rotation_opt;
		}
		return rotation;

	}












	void GameEntity::SetPosition(glm::vec2 const& in_position)
	{
		if (is_particle_master)
			if (ParticleTools::SetParticlePosition(allocations.get(), 0, in_position))
				return;
		bounding_box.position = in_position;
	}
	void GameEntity::SetBoundingBox(box2 const& in_bounding_box)
	{
		if (is_particle_master)
			if (ParticleTools::SetParticleBox(allocations.get(), 0, in_bounding_box))
				return;
		bounding_box = in_bounding_box;
	}


	// shu47 

	void GameEntity::SetRotation(float in_rotation)
	{
		if (is_particle_master)
			if (ParticleTools::SetParticleRotation(allocations.get(), 0, in_rotation))
				return;
		rotation = in_rotation;
	}



	














	// shu47

	void GameEntity::SynchronizeData(bool particle_to_entity)
	{
		if (particle_to_entity)
		{
			std::optional<box2> box_opt = ParticleTools::GetParticleBox(allocations.get(), 0);
			if (box_opt)
				bounding_box = *box_opt;

			std::optional<float> rotation_opt = ParticleTools::GetParticleRotation(allocations.get(), 0);
			if (rotation_opt)
				rotation = *rotation_opt;
		}
		else
		{
			ParticleTools::SetParticleBox(allocations.get(), 0, bounding_box);
			ParticleTools::SetParticleRotation(allocations.get(), 0, rotation);
		}
	}

	void GameEntity::SetAllocation(ParticleAllocationBase* in_allocation)
	{
		allocations = in_allocation;
	}
	
	bool GameEntity::SerializeFromJSON(nlohmann::json const& entry)
	{
		if (!JSONSerializable::SerializeFromJSON(entry))
			return false;

		box2 b;
		if (JSONTools::GetAttribute(entry, "BOUNDING_BOX", b))
			SetBoundingBox(b);

		return true;
	}

	bool GameEntity::SerializeIntoJSON(nlohmann::json& entry) const
	{
		if (!JSONSerializable::SerializeIntoJSON(entry))
			return false;
		JSONTools::SetAttribute(entry, "BOUNDING_BOX", GetBoundingBox());
		return true;
	}
	
}; // namespace chaos

