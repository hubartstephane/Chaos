#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GameEntity;

}; // namespace chaos

#else

namespace chaos
{
	// =============================================
	// GameEntity
	// =============================================

	class GameEntity : public chaos::Tickable, public chaos::JSONSerializable
	{

		CHAOS_DECLARE_OBJECT_CLASS2(GameEntity, chaos::Tickable);

	public:

		/** Get the position of the entity */
		virtual glm::vec2 GetPosition() const;
		/** Get the bounding box of the entity */
		virtual chaos::box2 GetBoundingBox() const;
		/** Set the position of the entity */
		virtual void SetPosition(glm::vec2 const& in_position);
		/** Set the bounding box of the entity */
		virtual void SetBoundingBox(chaos::box2 const& in_bounding_box);


		// shu47 


		chaos::obox2 GetOBox() const;



		/** Get the rotation of the entity */
		virtual float GetRotation() const;
		/** Set the rotation of the entity */
		virtual void SetRotation(float in_rotation);


		// shu47

		template<typename T>
		T* GetParticle(size_t index)
		{
			if (allocations == nullptr)
				return nullptr;
			chaos::ParticleAccessor<T> accessor = allocations->GetParticleAccessor<T>();
			if (accessor.GetDataCount() <= index)
				return nullptr;
			return &accessor[index];
		}

		template<typename T>
		T const * GetParticle(size_t index) const
		{
			if (allocations == nullptr)
				return nullptr;
			chaos::ParticleConstAccessor<T> accessor = allocations->GetParticleAccessor<T>();
			if (accessor.GetDataCount() <= index)
				return nullptr;
			return &accessor[index];
		}






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

		/** get the entity mode */
		bool IsParticleMaster() const { return is_particle_master; }
		/** set the entity mode */
		void SetParticleMaster(bool in_value) { is_particle_master = in_value; }

	protected:


		// shu47


		/** the rotation of the object */
		float rotation = 0.0f;





		/** whether the particle 0 data is the one to be used first */
		bool is_particle_master = false;
		/** the box for the entity */
		chaos::box2 bounding_box;
		/** the allocation for the entity */
		chaos::shared_ptr<chaos::ParticleAllocationBase> allocations;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
