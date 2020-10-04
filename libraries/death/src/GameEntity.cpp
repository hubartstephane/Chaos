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



	// shu47

	chaos::obox2 GameEntity::GetOBox() const
	{
		chaos::box2 ob = GetBoundingBox();

		chaos::obox2 result;
		result.position = ob.position;
		result.half_size = ob.half_size;
		result.rotator = GetRotation();

		return result;
	}

	float GameEntity::GetRotation() const
	{
		if (is_particle_master)
		{
			std::optional<float> rotation_opt = chaos::ParticleTools::GetParticleRotation(allocations.get(), 0);
			if (rotation_opt)
				return *rotation_opt;
		}
		return rotation;

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


	// shu47 

	void GameEntity::SetRotation(float in_rotation)
	{
		if (is_particle_master)
			if (chaos::ParticleTools::SetParticleRotation(allocations.get(), 0, in_rotation))
				return;
		rotation = in_rotation;
	}



	














	// shu47

	void GameEntity::SynchronizeData(bool particle_to_entity)
	{
		if (particle_to_entity)
		{
			std::optional<chaos::box2> box_opt = chaos::ParticleTools::GetParticleBox(allocations.get(), 0);
			if (box_opt)
				bounding_box = *box_opt;

			std::optional<float> rotation_opt = chaos::ParticleTools::GetParticleRotation(allocations.get(), 0);
			if (rotation_opt)
				rotation = *rotation_opt;
		}
		else
		{
			chaos::ParticleTools::SetParticleBox(allocations.get(), 0, bounding_box);
			chaos::ParticleTools::SetParticleRotation(allocations.get(), 0, rotation);
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

