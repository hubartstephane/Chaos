#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	// =====================================
	// TMLevel : utility
	// =====================================

	// XXX : while we want to create GeometricObject during tile creation we will need a fake 'geometric_object' (or a geometric_object on the fly)
	//       we cannot afford to create such a costly object for each tile (considering that 99% of the tiles should not require such a fake object)
	// 
	//       that's why we use factories (even for non tile layers).
	//       for a tile we:
	//         - get a factory
	//         - create the fake geometric_object
	//         - use the factory with this geometric object on the fly
	//
	// XXX : the functions to get factories receive a : TypedObject         (because we just need some properties and a type to know which object we would like to create (and that's what we get in Tile generation method))
	//       the factories receive a                  : GeometricObjectType (because for the real object creation we need more than a TypedObject)
	//
	// XXX : beware, the CHAOS_TM_MAKE_OBJECT_FACTORY(...) macro hide the complexity of lambda parameters capture. Use with caution 
	//

	/** a functor for geometric object factory */
	using TMObjectFactory = std::function<TMObject* (TiledMap::GeometricObject const*, TMObjectReferenceSolver&)>;
	/** an helper to make a lambda inside DoGetObjectFactory */
#define CHAOS_TM_MAKE_OBJECT_FACTORY(x) [this, in_layer_instance](chaos::TiledMap::GeometricObject const *in_geometric_object, chaos::TMObjectReferenceSolver & in_reference_solver) { x }

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{
	// =====================================
	// TMLevel : a level described by a tiledmap
	// =====================================

	class TMLevel : public Level
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(TMLevel, Level);

	public:

		/** constructor */
		TMLevel();

		/** initialization from tiled_map */
		virtual bool Initialize(TiledMap::Map * in_tiled_map);

		/** get the tiled map */
		TiledMap::Map* GetTiledMap() { return tiled_map.get(); }
		/** get the tiled map */
		TiledMap::Map const* GetTiledMap() const { return tiled_map.get(); }

	protected:

		/** called to flush some particles into a layer allocation */
		virtual bool FlushParticlesIntoAllocation(TMLayerInstance* layer_instance, ParticleAllocationBase* allocation, TMParticle const* particles, size_t particle_count);

		/** create a typed object based of a 'classname' property */
		virtual TMObjectFactory DoGetExplicitObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object);
		/** create a typed object specializable method */
		virtual TMObjectFactory DoGetObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object);
		/** create a typed object 'entry point' */
		TMObjectFactory GetObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object);

		/** create a Path specializable method */
		virtual TMPath * DoCreatePath();
		/** create a Camera specializable method */
		virtual TMCameraTemplate* DoCreateCameraTemplate();
		/** create a PlayerStartObject specializable method */
		virtual TMPlayerStart* DoCreatePlayerStart();
		/** create a FinishingTriggerObject specializable method */
		virtual TMChangeLevelTrigger* DoCreateChangeLevelTrigger();
		/** create a CheckpointTriggerObject specializable method */
		virtual TMCheckpointTrigger* DoCreateCheckpointTrigger();
		/** create a NotificationTriggerObject specializable method */
		virtual TMNotificationTrigger* DoCreateNotificationTrigger();
		/** create a SoundTriggerObject specializable method */
		virtual TMSoundTrigger* DoCreateSoundTrigger();

		/** create a PlayerStartObject specializable method */
		virtual TMLayerInstance* DoCreateLayerInstance();

		/** create a layer instance 'entry point' */
		TMLayerInstance* CreateLayerInstance(TMLevelInstance* in_level_instance, TiledMap::LayerBase* in_layer, TMLayerInstance* in_parent_layer, TMObjectReferenceSolver& reference_solver);

		/** get the folder in which bitmaps are stored in Game::Atlas */
		virtual ObjectRequest GetFolderInfoRequest() const;

		/** create a particle layer */
		virtual ParticleLayerBase* DoCreateParticleLayer(TMLayerInstance* layer_instance);

		/** called after all particles of a layers has been created, so we can plug additionnal data */
		virtual bool FinalizeLayerParticles(TMLayerInstance* layer_instance, ParticleAllocationBase* allocation);

	protected:

		/** the tiled map corresponding to this level */
		shared_ptr<TiledMap::Map> tiled_map;
	};


}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION