#include <death/GameEntity.h>

#include <chaos/JSONTools.h>
#include <chaos/ParticleTools.h>

namespace death
{

	glm::vec2 GameEntity::GetPosition() const
	{
		return chaos::ParticleTools::GetParticlePosition(allocations.get(), 0);


		//return bounding_box.position;
	}

	chaos::box2 GameEntity::GetBoundingBox() const
	{
		return chaos::ParticleTools::GetParticleBox(allocations.get(), 0);
		//return bounding_box;	
	}

	void GameEntity::SetPosition(glm::vec2 const& in_position)
	{

		chaos::ParticleTools::SetParticlePosition(allocations.get(), 0, in_position);

#if 0

		if (bounding_box.position != in_position)
		{
			chaos::box2 old_bounding_box = bounding_box;
			bounding_box.position = in_position;
			OnBoundingBoxChanged(bounding_box, old_bounding_box);
		}
#endif
	}
	void GameEntity::SetBoundingBox(chaos::box2 const& in_bounding_box)
	{
		chaos::ParticleTools::SetParticleBox(allocations.get(), 0, in_bounding_box);

#if 0
		if (bounding_box != in_bounding_box)
		{
			chaos::box2 old_bounding_box = bounding_box;
			bounding_box = in_bounding_box;
			OnBoundingBoxChanged(bounding_box, old_bounding_box);
		}		
#endif
	}

	void GameEntity::OnBoundingBoxChanged(chaos::box2 const& new_bounding_box, chaos::box2 const& old_bounding_box)
	{
		SynchronizeData(false);
	}

	void GameEntity::SynchronizeData(bool particle_to_entity)
	{
		if (allocations == nullptr)
			return;
		if (particle_to_entity)
			bounding_box = chaos::ParticleTools::GetParticleBox(allocations.get(), 0);
		else
			chaos::ParticleTools::SetParticleBox(allocations.get(), 0, bounding_box);
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

