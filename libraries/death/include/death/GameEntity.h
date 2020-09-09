#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
#include <chaos/Tickable.h>
#include <chaos/ParticleManager.h>
#include <chaos/JSONSerializable.h>
#include <chaos/GeometryFramework.h>

namespace death
{

	// =============================================
	// GameEntity
	// =============================================

	class GameEntity : public chaos::Tickable, public chaos::JSONSerializable
	{

		CHAOS_OBJECT_DECLARE_CLASS2(GameEntity, chaos::Tickable);

	public:

		/** Get the position of the entity */
		virtual glm::vec2 GetPosition() const;
		/** Get the bounding box of the entity */
		virtual chaos::box2 GetBoundingBox() const;
		/** Set the position of the entity */
		virtual void SetPosition(glm::vec2 const& in_position);
		/** Set the bounding box of the entity */
		virtual void SetBoundingBox(chaos::box2 const& in_bounding_box);

		/** get the entity allocation */
		chaos::ParticleAllocationBase* GetAllocation() { return allocations.get(); }
		/** get the entity allocation */
		chaos::ParticleAllocationBase const* GetAllocation() const { return allocations.get(); }
		/** set the entity allocation */
		virtual void SetAllocation(chaos::ParticleAllocationBase* in_allocation);

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override;

		/** copy the bounding box from particle to entity or the opposite */
		void SynchronizeData(bool particle_to_entity);

	protected:

		/** called whenever the box is changed */
		virtual void OnBoundingBoxChanged(chaos::box2 const& new_bounding_box, chaos::box2 const& old_bounding_box);

	protected:

		/** the box for the entity */
		chaos::box2 bounding_box;
		/** the allocation for the entity */
		chaos::shared_ptr<chaos::ParticleAllocationBase> allocations;
	};

}; // namespace death
