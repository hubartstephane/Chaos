#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// =====================================
	// TMLevel implementation
	// =====================================

	TMLevel::TMLevel()
	{
		level_instance_class = TMLevelInstance::GetStaticClass();
	}

	bool TMLevel::Initialize(TiledMap::Map * in_tiled_map)
	{
		assert(in_tiled_map != nullptr);
		assert(tiled_map == nullptr);
		// keep a copy of the tiled object
		tiled_map = in_tiled_map;
		// get level title (if specified)
		std::string const* in_level_title = in_tiled_map->FindPropertyString("LEVEL_TITLE");
		if (in_level_title != nullptr)
			level_title = *in_level_title;
		// get the level time
		level_timeout = in_tiled_map->GetPropertyValueFloat("LEVEL_TIMEOUT", level_timeout);
		return true;
	}

	ObjectRequest TMLevel::GetFolderInfoRequest() const
	{
		return "sprites";
	}

	ParticleLayerBase* TMLevel::DoCreateParticleLayer(TMLayerInstance* layer_instance)
	{
		return new ParticleLayer<TMParticleLayerTrait>();
	}

	TMObjectFactory TMLevel::DoGetObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object)
	{
		// player start
		if (TMTools::IsPlayerStart(in_typed_object))
			return CHAOS_TM_MAKE_OBJECT_FACTORY(return DoCreatePlayerStart(););
		// camera
		if (TMTools::IsCameraTemplate(in_typed_object))
			return CHAOS_TM_MAKE_OBJECT_FACTORY(return DoCreateCameraTemplate(););
		// path
		if (TiledMap::GeometricObjectPolygon const * polygon = auto_cast(in_typed_object))
			return CHAOS_TM_MAKE_OBJECT_FACTORY(return DoCreatePath(););
		if (TiledMap::GeometricObjectPolyline const* polyline = auto_cast(in_typed_object))
			return CHAOS_TM_MAKE_OBJECT_FACTORY(return DoCreatePath(););
		// other kind of objects
		if (TMTools::IsFinishTrigger(in_typed_object))
			return CHAOS_TM_MAKE_OBJECT_FACTORY(return DoCreateChangeLevelTrigger(););
		if (TMTools::IsCheckpointTrigger(in_typed_object))
			return CHAOS_TM_MAKE_OBJECT_FACTORY(return DoCreateCheckpointTrigger(););
		if (TMTools::IsNotificationTrigger(in_typed_object))
			return CHAOS_TM_MAKE_OBJECT_FACTORY(return DoCreateNotificationTrigger(););
		if (TMTools::IsSoundTrigger(in_typed_object))
			return CHAOS_TM_MAKE_OBJECT_FACTORY(return DoCreateSoundTrigger(););
		return nullptr;
	}


	TMObjectFactory TMLevel::DoGetExplicitObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object)
	{
		// search the subclass of the object (by type or classname member)
		SubClassOf<TMObject> subclass = ClassManager::GetDefaultInstance()->FindClass(in_typed_object->type.c_str());
		if (!subclass.IsValid())
		{
			if (std::string const* classname = in_typed_object->FindPropertyString("classname"))
			{
				subclass = ClassManager::GetDefaultInstance()->FindClass(classname->c_str());
			}
		}
		// failure ?
		if (!subclass.IsValid())
			return nullptr;

		// return the factory based on this subclass
		return [this, in_layer_instance, subclass](TiledMap::GeometricObject const * in_geometric_object, TMObjectReferenceSolver & in_reference_solver)
		{
			return subclass.CreateInstance();
		};
	}

	TMObjectFactory TMLevel::GetObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object)
	{
		TMObjectFactory factory = DoGetExplicitObjectFactory(in_layer_instance, in_typed_object);
		if (factory == nullptr)
		{
			// get the base factory that just create an instance of the object (based on the properties of the TypedObject of concern)
			factory = DoGetObjectFactory(in_layer_instance, in_typed_object);
			if (!factory)
				return nullptr;
		}

		// create another factory that wraps the previous (and add Initialize(...) call)
		TMObjectFactory result_factory = [in_layer_instance, factory](TiledMap::GeometricObject const * in_geometric_object, TMObjectReferenceSolver& in_reference_solver)
		{
			TMObject * result = factory(in_geometric_object, in_reference_solver);
			if (result != nullptr)
			{
				if (!result->Initialize(in_layer_instance, in_geometric_object, in_reference_solver))
				{
					delete result;
					result = nullptr;
				}
			}
			return result;
		};
		return result_factory;
	}

	TMPath* TMLevel::DoCreatePath()
	{
		return new TMPath();
	}

	TMCameraTemplate* TMLevel::DoCreateCameraTemplate()
	{
		return new TMCameraTemplate();
	}
	TMChangeLevelTrigger* TMLevel::DoCreateChangeLevelTrigger()
	{
		return new TMChangeLevelTrigger();
	}
	TMCheckpointTrigger* TMLevel::DoCreateCheckpointTrigger()
	{
		return new TMCheckpointTrigger();
	}
	TMNotificationTrigger* TMLevel::DoCreateNotificationTrigger()
	{
		return new TMNotificationTrigger();
	}
	TMSoundTrigger* TMLevel::DoCreateSoundTrigger()
	{
		return new TMSoundTrigger();
	}
	TMPlayerStart* TMLevel::DoCreatePlayerStart()
	{
		return new TMPlayerStart();
	}

	TMLayerInstance* TMLevel::DoCreateLayerInstance()
	{
		return new TMLayerInstance();
	}


	// shulayer

	TMLayerInstance* TMLevel::CreateLayerInstance(TMLevelInstance* in_level_instance, TiledMap::LayerBase* in_layer, TMLayerInstance * in_parent_layer, TMObjectReferenceSolver& reference_solver)
	{
		TMLayerInstance* result = DoCreateLayerInstance();
		if (result == nullptr)
			return nullptr;
		if (!result->Initialize(in_level_instance, in_layer, in_parent_layer, reference_solver))
		{
			delete result;
			return nullptr;
		}
		return result;
	}

	bool TMLevel::FinalizeLayerParticles(TMLayerInstance* layer_instance, ParticleAllocationBase* allocation)
	{
		return true;
	}

}; // namespace chaos
