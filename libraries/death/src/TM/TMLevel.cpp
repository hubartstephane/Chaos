#include <death/TM.h>
#include <death/Game.h>
#include <death/Player.h>
#include <death/SoundContext.h>
#include <death/CollisionMask.h>

#include <chaos/CollisionFramework.h>
#include <chaos/TiledMapTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GPUProgramGenerator.h>
#include <chaos/StringTools.h>
#include <chaos/JSONTools.h>
#include <chaos/HelpText.h>
#include <chaos/RepeatedBoxScissor.h>


namespace death
{
	// =====================================
	// TMObject implementation
	// =====================================

	chaos::box2 TMObject::GetBoundingBox(bool world_system) const
	{
		chaos::box2 result = bounding_box;
		if (world_system)
			result.position += layer_instance->GetLayerOffset();
		return result;
	}

	bool TMObject::Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object)
	{
		// ensure not already initialized
		assert(in_layer_instance != nullptr);
		assert(in_geometric_object != nullptr);
		assert(layer_instance == nullptr);

		layer_instance = in_layer_instance;

		// get some data from the geometric object
		name = in_geometric_object->name;
		id = in_geometric_object->GetObjectID();
		particle_ownership = in_geometric_object->GetPropertyValueBool("PARTICLE_OWNERSHIP", particle_ownership);

		// extract the bounding box
		chaos::TiledMap::GeometricObjectSurface const * surface = in_geometric_object->GetObjectSurface();
		if (surface != nullptr)
			bounding_box = surface->GetBoundingBox(false);  // make our own correction for world system because the LayerInstance can change its offset

		return true;
	}

	bool TMObject::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!chaos::JSONSerializable::SerializeFromJSON(json))
			return false;
		chaos::JSONTools::GetAttribute(json, "NAME", name);
		chaos::JSONTools::GetAttribute(json, "OBJECT_ID", id);
		chaos::JSONTools::GetAttribute(json, "BOUNDING_BOX", bounding_box);
		chaos::JSONTools::GetAttribute(json, "PARTICLE_OWNERSHIP", particle_ownership);
		return true;
	}

	bool TMObject::SerializeIntoJSON(nlohmann::json & json) const
	{
		if (!chaos::JSONSerializable::SerializeIntoJSON(json))
			return false;
		chaos::JSONTools::SetAttribute(json, "NAME", name);
		chaos::JSONTools::SetAttribute(json, "OBJECT_ID", id);
		chaos::JSONTools::SetAttribute(json, "BOUNDING_BOX", bounding_box);
		chaos::JSONTools::SetAttribute(json, "PARTICLE_OWNERSHIP", particle_ownership);
		return true;
	}

	bool TMObject::IsParticleCreationEnabled() const
	{
		return true;
	}

	// =====================================
	// TMTrigger implementation
	// =====================================

	bool TMTrigger::Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object)
	{
		if (!TMObject::Initialize(in_layer_instance, in_geometric_object))
			return false;
		// default values are set to the one defined by default in constructor
		enabled = in_geometric_object->GetPropertyValueBool("ENABLED", enabled);
		trigger_once = in_geometric_object->GetPropertyValueBool("TRIGGER_ONCE", trigger_once);
		outside_box_factor = in_geometric_object->GetPropertyValueFloat("OUTSIDE_BOX_FACTOR", outside_box_factor);

		// enable the possibility to trigger once again the object
		enter_event_triggered = false;
		return true;
	}

	bool TMTrigger::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!TMObject::SerializeFromJSON(json))
			return false;
		chaos::JSONTools::GetAttribute(json, "ENABLED", enabled);
		chaos::JSONTools::GetAttribute(json, "TRIGGER_ONCE", trigger_once);
		chaos::JSONTools::GetAttribute(json, "OUTSIDE_BOX_FACTOR", outside_box_factor);
		chaos::JSONTools::GetAttribute(json, "ENTER_EVENT_TRIGGERED", enter_event_triggered);
		return true;
	}

	bool TMTrigger::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!TMObject::SerializeIntoJSON(json))
			return false;
		chaos::JSONTools::SetAttribute(json, "ENABLED", enabled);
		chaos::JSONTools::SetAttribute(json, "TRIGGER_ONCE", trigger_once);
		chaos::JSONTools::SetAttribute(json, "OUTSIDE_BOX_FACTOR", outside_box_factor);
		chaos::JSONTools::SetAttribute(json, "ENTER_EVENT_TRIGGERED", enter_event_triggered);
		return true;
	}

	bool TMTrigger::IsCollisionWith(chaos::box2 const& other_box, chaos::CollisionType collision_type) const
	{
		chaos::box2 box = GetBoundingBox(true);

		// the box is bigger when we want to go outside !
		if (collision_type == chaos::CollisionType::AGAIN && outside_box_factor > 1.0f) 
			box.half_size *= outside_box_factor;

		return chaos::Collide(other_box, box);
	}

	bool TMTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
	{
		return false; // do not do anything with collision
	}

	void TMTrigger::SetEnabled(bool in_enabled)
	{
		enabled = in_enabled;
	}

	void TMTrigger::SetTriggerOnce(bool in_trigger_once)
	{
		trigger_once = in_trigger_once;
	}

	// =============================================================
	// TiledMapCheckPointTriggerObject implementation
	// =============================================================

	bool TMCheckpointTrigger::Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object)
	{
		if (!TMTrigger::Initialize(in_layer_instance, in_geometric_object))
			return false;
		trigger_once = true; // force a trigger once for checkpoint
		return true;
	}

	bool TMCheckpointTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
	{
		Camera* camera = auto_cast(object);
		if (camera == nullptr)
			return false;
		
		if (event_type != chaos::CollisionType::STARTED)
			return false;

		GameInstance* game_instance = GetLayerInstance()->GetGame()->GetGameInstance();
		if (game_instance != nullptr && game_instance->GetPlayerCount() > 0)
			game_instance->CreateRespawnCheckpoint();

		return true; // collisions handled successfully
	}

	bool TMCheckpointTrigger::IsParticleCreationEnabled() const
	{
		return false;
	}

	// =====================================
	// TMPlayerStart implementation
	// =====================================

	bool TMPlayerStart::Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object)
	{
		if (!TMObject::Initialize(in_layer_instance, in_geometric_object))
			return false;
		// search the bitmap name for the player
		std::string const * in_bitmap_name = in_geometric_object->FindPropertyString("BITMAP_NAME");
		if (in_bitmap_name == nullptr)
			return false;
		bitmap_name = *in_bitmap_name;
		return true;
	}

	bool TMPlayerStart::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!TMObject::SerializeFromJSON(json))
			return false;
		chaos::JSONTools::GetAttribute(json, "BITMAP_NAME", bitmap_name);
		return true;
	}

	bool TMPlayerStart::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!TMObject::SerializeIntoJSON(json))
			return false;
		chaos::JSONTools::SetAttribute(json, "BITMAP_NAME", bitmap_name);
		return true;
	}

	// =================================================
	// TMNotificationTrigger
	// =================================================

	bool TMNotificationTrigger::IsParticleCreationEnabled() const
	{
		return false;
	}

	bool TMNotificationTrigger::Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object)
	{
		if (!TMTrigger::Initialize(in_layer_instance, in_geometric_object))
			return false;

		notification_string = in_geometric_object->GetPropertyValueString("NOTIFICATION", "");
		notification_lifetime = in_geometric_object->GetPropertyValueFloat("LIFETIME", notification_lifetime);
		stop_when_collision_over = in_geometric_object->GetPropertyValueBool("STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		player_collision = in_geometric_object->GetPropertyValueBool("PLAYER_COLLISION", player_collision);

		return true;
	}

	bool TMNotificationTrigger::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!TMTrigger::SerializeFromJSON(json))
			return false;
		chaos::JSONTools::GetAttribute(json, "NOTIFICATION", notification_string);
		chaos::JSONTools::GetAttribute(json, "LIFETIME", notification_lifetime);
		chaos::JSONTools::GetAttribute(json, "STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		chaos::JSONTools::GetAttribute(json, "PLAYER_COLLISION", player_collision);
		return true;
	}

	bool TMNotificationTrigger::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!TMTrigger::SerializeIntoJSON(json))
			return false;
		chaos::JSONTools::SetAttribute(json, "NOTIFICATION", notification_string);
		chaos::JSONTools::SetAttribute(json, "LIFETIME", notification_lifetime);
		chaos::JSONTools::SetAttribute(json, "STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		chaos::JSONTools::SetAttribute(json, "PLAYER_COLLISION", player_collision);
		return true;
	}


	bool TMNotificationTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
	{
		// check object type
		if (player_collision)
		{
			Player* player = auto_cast(object);
			if (player == nullptr)
				return false;
		}
		else
		{
			Camera* camera = auto_cast(object);
			if (camera == nullptr)
				return false;
		}

		// early exit
		if (event_type != chaos::CollisionType::STARTED && event_type != chaos::CollisionType::FINISHED) // ignore AGAIN event
			return false;
		if (event_type == chaos::CollisionType::FINISHED && !stop_when_collision_over) // ignore FINISHED if you do not want to kill the notification
			return false;
		// get some variables 
		Game* game = layer_instance->GetGame();
		if (game == nullptr)
			return false;
		death::GameHUD* hud = game->GetCurrentHUD();
		if (hud == nullptr)
			return false;
		GameHUDNotificationComponent* notification_component = hud->FindComponentByClass<GameHUDNotificationComponent>();
		if (notification_component == nullptr)
			return false;
		// show notification
		if (event_type == chaos::CollisionType::STARTED)
			notification_component->ShowNotification(notification_string.c_str(), notification_lifetime);
		// hide notification
		else if (event_type == chaos::CollisionType::FINISHED) // XXX : 'stop_when_collision_over' has already be checked
			notification_component->HideNotification();
		return true;
	}

	// =====================================
	// TMSoundTrigger implementation
	// =====================================

	bool TMSoundTrigger::Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object)
	{
		if (!TMTrigger::Initialize(in_layer_instance, in_geometric_object))
			return false;

		sound_name = in_geometric_object->GetPropertyValueString("SOUND_NAME", "");
		min_distance_ratio = in_geometric_object->GetPropertyValueFloat("MIN_DISTANCE_RATIO", min_distance_ratio);
		min_distance_ratio = std::clamp(min_distance_ratio, 0.0f, 1.0f);

		pause_timer_when_too_far = in_geometric_object->GetPropertyValueFloat("PAUSE_TIMER_WHEN_TOO_FAR", pause_timer_when_too_far);
		is_3D_sound = in_geometric_object->GetPropertyValueBool("3D_SOUND", is_3D_sound);
		looping = in_geometric_object->GetPropertyValueBool("LOOPING", looping);
		stop_when_collision_over = in_geometric_object->GetPropertyValueBool("STOP_WHEN_COLLISION_OVER", stop_when_collision_over);

		return true;
	}

	bool TMSoundTrigger::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!TMTrigger::SerializeFromJSON(json))
			return false;
		chaos::JSONTools::GetAttribute(json, "SOUND_NAME", sound_name);
		chaos::JSONTools::GetAttribute(json, "MIN_DISTANCE_RATIO", min_distance_ratio);
		chaos::JSONTools::GetAttribute(json, "PAUSE_TIMER_WHEN_TOO_FAR", pause_timer_when_too_far);
		chaos::JSONTools::GetAttribute(json, "3D_SOUND", is_3D_sound);
		chaos::JSONTools::GetAttribute(json, "LOOPING", looping);
		chaos::JSONTools::GetAttribute(json, "STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		return true;
	}

	bool TMSoundTrigger::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!TMTrigger::SerializeIntoJSON(json))
			return false;
		chaos::JSONTools::SetAttribute(json, "SOUND_NAME", sound_name);
		chaos::JSONTools::SetAttribute(json, "MIN_DISTANCE_RATIO", min_distance_ratio);
		chaos::JSONTools::SetAttribute(json, "PAUSE_TIMER_WHEN_TOO_FAR", pause_timer_when_too_far);
		chaos::JSONTools::SetAttribute(json, "3D_SOUND", is_3D_sound);
		chaos::JSONTools::SetAttribute(json, "LOOPING", looping);
		chaos::JSONTools::SetAttribute(json, "STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		return true;
	}

	chaos::Sound* TMSoundTrigger::CreateSound() const
	{
		// early exit
		if (sound_name.length() == 0)
			return nullptr;

		// create the sound
		chaos::Sound* result = nullptr;

		Game* game = layer_instance->GetGame();
		if (game != nullptr)
		{
			chaos::box2 box = GetBoundingBox(true);
			glm::vec2   position = box.position;

			chaos::PlaySoundDesc play_desc;
			play_desc.paused = false;
			play_desc.looping = looping;
			play_desc.blend_in_time = 0.0f;
			play_desc.pause_timer_when_too_far = pause_timer_when_too_far;

			play_desc.SetPosition(glm::vec3(position, 0.0f), is_3D_sound);
			play_desc.max_distance = glm::length(box.half_size);
			play_desc.min_distance = play_desc.max_distance * min_distance_ratio;

			result = game->PlaySound(sound_name.c_str(), play_desc, SoundContext::LEVEL);
		}

		return result;
	}

	bool TMSoundTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
	{
		Camera* camera = auto_cast(object);
		if (camera == nullptr)
			return false;

		if (event_type == chaos::CollisionType::STARTED)
		{
			chaos::Sound* new_sound = CreateSound();
			if (stop_when_collision_over)
				sound = new_sound;
			return true;
		}
		if (event_type == chaos::CollisionType::FINISHED)
		{
			if (stop_when_collision_over && sound != nullptr)
			{
				sound->Stop();
				sound = nullptr;
			}
			return true;
		}
		return false;
	}

	bool TMSoundTrigger::IsParticleCreationEnabled() const
	{
		return false;
	}

	// =============================================================
	// TMChangeLevelTrigger implementation
	// =============================================================

	bool TMChangeLevelTrigger::IsParticleCreationEnabled() const
	{
		return false;
	}

	bool TMChangeLevelTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
	{
		Player* player = auto_cast(object);
		if (player == nullptr)
			return false;

		if (event_type != chaos::CollisionType::STARTED)
			return false;

		Game* game = layer_instance->GetGame();
		if (game != nullptr)
		{
			death::LevelInstance* level_instance = game->GetLevelInstance();
			if (level_instance != nullptr)
				level_instance->SetLevelCompletionFlag();
		}
		return true;
	}

	bool TMChangeLevelTrigger::Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object)
	{
		if (!TMTrigger::Initialize(in_layer_instance, in_geometric_object))
			return false;
		level_name = in_geometric_object->GetPropertyValueString("LEVEL_NAME", "");
		player_start_name = in_geometric_object->GetPropertyValueString("PLAYER_START_NAME", "");
		return true;
	}

	bool TMChangeLevelTrigger::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!TMTrigger::SerializeFromJSON(json))
			return false;
		chaos::JSONTools::GetAttribute(json, "LEVEL_NAME", level_name);
		chaos::JSONTools::GetAttribute(json, "PLAYER_START_NAME", player_start_name);
		return true;
	}

	bool TMChangeLevelTrigger::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!TMTrigger::SerializeIntoJSON(json))
			return false;
		chaos::JSONTools::SetAttribute(json, "LEVEL_NAME", level_name);
		chaos::JSONTools::SetAttribute(json, "PLAYER_START_NAME", player_start_name);
		return true;
	}

	// =============================================================
	// TMChangeLevelTrigger implementation
	// =============================================================




































	// =====================================
	// TMCameraTemplate implementation
	// =====================================

	bool TMCameraTemplate::Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object)
	{
		if (!TMObject::Initialize(in_layer_instance, in_geometric_object))
			return false;
		return true;
	}

	// =====================================
	// TMLevel implementation
	// =====================================

	TMLevel::TMLevel()
	{
		level_instance_class = TMLevelInstance::GetStaticClass();
	}

	bool TMLevel::Initialize(chaos::TiledMap::Map * in_tiled_map)
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

	chaos::BitmapAtlas::TextureArrayAtlas const* TMLevel::GetTextureAtlas(TMLayerInstance* layer_instance) const
	{
		Game const* game = layer_instance->GetGame();
		if (game == nullptr)
			return nullptr;
		return game->GetTextureAtlas();
	}

	chaos::BitmapAtlas::FolderInfo const* TMLevel::GetFolderInfo(TMLayerInstance* layer_instance) const
	{
		// get the atlas
		chaos::BitmapAtlas::TextureArrayAtlas const* texture_atlas = GetTextureAtlas(layer_instance);
		if (texture_atlas == nullptr)
			return nullptr;
		// get the folder containing the sprites
		return texture_atlas->GetFolderInfo("sprites");
	}

	chaos::ParticleLayerBase* TMLevel::DoCreateParticleLayer(TMLayerInstance* layer_instance)
	{
		return new chaos::ParticleLayer<TiledMapParticleTrait>();
	}
	
	TMObjectFactory TMLevel::DoGetObjectFactory(TMLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
	{
		// player start 
		if (chaos::TiledMapTools::IsPlayerStartObject(in_typed_object))
			return DEATH_MAKE_OBJECT_FACTORY(return DoCreatePlayerStart(););
		// camera 
		if (chaos::TiledMapTools::IsCameraObject(in_typed_object))
			return DEATH_MAKE_OBJECT_FACTORY(return DoCreateCamera(););
		// other kind of objects
		if (chaos::TiledMapTools::IsFinishTrigger(in_typed_object))
			return DEATH_MAKE_OBJECT_FACTORY(return DoCreateChangeLevelTrigger(););
		if (chaos::TiledMapTools::IsCheckpointTrigger(in_typed_object))
			return DEATH_MAKE_OBJECT_FACTORY(return DoCreateCheckpointTrigger(););
		if (chaos::TiledMapTools::IsNotificationTrigger(in_typed_object))
			return DEATH_MAKE_OBJECT_FACTORY(return DoCreateNotificationTrigger(););
		if (chaos::TiledMapTools::IsSoundTrigger(in_typed_object))
			return DEATH_MAKE_OBJECT_FACTORY(return DoCreateSoundTrigger(););
		return nullptr;
	}


	TMObjectFactory TMLevel::DoGetExplicitObjectFactory(TMLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
	{	
		// get the 'classname' property
		std::string const* classname = in_typed_object->FindPropertyString("classname");
		if (classname == nullptr)
			return nullptr;
		// find wanted class
		chaos::Class const* wanted_class = chaos::Class::FindClass(classname->c_str());
		if (wanted_class == nullptr)
			return nullptr;
		// get the subclass
		chaos::SubClassOf<TMObject> subclass = { wanted_class };
		if (!subclass.IsValid())
			return nullptr;
		// return the factory based on this subclass
		return [this, in_layer_instance, subclass](chaos::TiledMap::GeometricObject const * in_geometric_object)
		{
			return subclass.CreateInstance();
		};
	}

	TMObjectFactory TMLevel::GetObjectFactory(TMLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
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
		TMObjectFactory result_factory = [in_layer_instance, factory](chaos::TiledMap::GeometricObject const * in_geometric_object)
		{
			TMObject * result = factory(in_geometric_object);
			if (result != nullptr)
			{
				if (!result->Initialize(in_layer_instance, in_geometric_object))
				{
					delete result;
					result = nullptr;
				}
			}
			return result;
		};
		return result_factory;
	}
	
	TMCameraTemplate* TMLevel::DoCreateCamera()
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

	TMLayerInstance* TMLevel::CreateLayerInstance(TMLevelInstance* in_level_instance, chaos::TiledMap::LayerBase* in_layer)
	{
		TMLayerInstance* result = DoCreateLayerInstance();
		if (result == nullptr)
			return nullptr;
		if (!result->Initialize(in_level_instance, in_layer))
		{
			delete result;
			return nullptr;
		}
		return result;
	}

	chaos::GPURenderMaterial* TMLevel::GenDefaultRenderMaterial()
	{
		return chaos::GenDefaultParticleMaterial();
	}

	bool TMLevel::FinalizeLayerParticles(TMLayerInstance* layer_instance, chaos::ParticleAllocationBase* allocation) 
	{
		if (layer_instance != nullptr && layer_instance->layer != nullptr)
		{
			// whether the collision flags must be computed 
			bool compute_collision_flags = layer_instance->layer->GetPropertyValueBool("COMPUTE_COLLISION_FLAGS", false);
			if (compute_collision_flags)
			{



				compute_collision_flags = compute_collision_flags;
			}
		}
		return true; 
	}

	bool TMLevel::FlushParticlesIntoAllocation(TMLayerInstance* layer_instance, chaos::ParticleAllocationBase * allocation, TiledMapParticle const * particles, size_t particle_count)
	{
		chaos::ParticleAccessor<TiledMapParticle> accessor = allocation->AddParticles(particle_count);

		if (!accessor.IsValid())
		{
			chaos::LogTools::Error("TMLevel::FlushParticlesIntoAllocation => invalid accessor");
			return false;
		}
		for (size_t i = 0; i < particle_count; ++i)
			accessor[i] = particles[i];
		return true;
	}













	// =====================================
	// TMLayerInstance implementation
	// =====================================

	chaos::AutoCastable<Game> TMLayerInstance::GetGame()
	{
		return level_instance->GetGame();
	}

	chaos::AutoConstCastable<Game> TMLayerInstance::GetGame() const
	{
		return level_instance->GetGame();
	}

	chaos::AutoCastable<Level> TMLayerInstance::GetLevel()
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetLevel();
	}

	chaos::AutoConstCastable<death::Level> TMLayerInstance::GetLevel() const
	{
		if (level_instance == nullptr)
			return (Level*)nullptr;
		return level_instance->GetLevel();
	}

	chaos::AutoCastable<LevelInstance> TMLayerInstance::GetLevelInstance()
	{
		return level_instance;
	}

	chaos::AutoConstCastable<LevelInstance> TMLayerInstance::GetLevelInstance() const
	{
		return level_instance;
	}

	chaos::box2 TMLayerInstance::GetBoundingBox(bool world_system) const
	{
		chaos::box2 result;
		if (!infinite_bounding_box)
		{
			result = bounding_box; // apply our own offset that can have changed during game lifetime
			if (world_system)
				result.position += offset;
		}
		else
			result = result;
		return result;
	}

	chaos::GPURenderMaterial* TMLayerInstance::FindOrCreateRenderMaterial(char const* material_name)
	{
		if (material_name != nullptr && material_name[0] != 0) // unamed material ?
		{
			// get the resource manager
			chaos::GPUResourceManager* resource_manager = chaos::MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
			if (resource_manager != nullptr)
			{
				// search declared material
				chaos::GPURenderMaterial* result = resource_manager->FindRenderMaterial(material_name);
				if (result != nullptr)
					return result;
			}
		}
		// default material else where
		return level_instance->GetDefaultRenderMaterial();
	}

	void TMLayerInstance::ComputeLayerCollisionMask(char const * mask)
	{
		assert(mask != nullptr);

		collision_mask = 0;

		std::vector<std::string> collision_names;
		chaos::NameFilter::AddNames(mask, collision_names);

		for (std::string const& name : collision_names)
			collision_mask |= GetCollisionFlagByName(name.c_str());
	}

	uint64_t TMLayerInstance::GetCollisionFlagByName(char const* name) const
	{
		assert(name != nullptr);

		if (chaos::StringTools::Stricmp(name, "PLAYER") == 0)
			return death::CollisionMask::PLAYER;
		if (chaos::StringTools::Stricmp(name, "CAMERA") == 0)
			return death::CollisionMask::CAMERA;

		return 0;
	}

	bool TMLayerInstance::Initialize(TMLevelInstance* in_level_instance, chaos::TiledMap::LayerBase const * in_layer)
	{
		// ensure not already initialized
		assert(in_level_instance != nullptr);
		assert(in_layer != nullptr);
		assert(level_instance == nullptr);
		assert(layer == nullptr);

		level_instance = in_level_instance;
		layer = in_layer;

		// get the properties of interrest
		id = layer->id;

		float const* ratio = layer->FindPropertyFloat("DISPLACEMENT_RATIO");
		if (ratio != nullptr)
		{
			displacement_ratio = glm::vec2(*ratio, *ratio);
		}
		else
		{
			displacement_ratio.x = layer->GetPropertyValueFloat("DISPLACEMENT_RATIO_X", displacement_ratio.x);
			displacement_ratio.y = layer->GetPropertyValueFloat("DISPLACEMENT_RATIO_Y", displacement_ratio.y);
		}
		wrap_x = layer->GetPropertyValueBool("WRAP_X", wrap_x);
		wrap_y = layer->GetPropertyValueBool("WRAP_Y", wrap_y);
		material_name = layer->GetPropertyValueString("MATERIAL", "");

		infinite_bounding_box = layer->GetPropertyValueBool("INFINITE_BOUNDING_BOX", infinite_bounding_box);

		autoclean_particles = layer->GetPropertyValueBool("AUTOCLEAN_PARTICLES", autoclean_particles);

		std::string collision_mask = layer->GetPropertyValueString("COLLISION_MASK", "");
		ComputeLayerCollisionMask(collision_mask.c_str());

		// copy the offset / name
		offset = layer->offset;
		name = layer->name;

		// reset the bounding box
		bounding_box = chaos::box2();
		// special initialization
		chaos::TiledMap::ImageLayer const* image_layer = layer->GetImageLayer();
		if (image_layer != nullptr)
		{
			if (!InitializeImageLayer(image_layer))
				return false;
			return FinalizeParticles(nullptr);
		}

		chaos::TiledMap::ObjectLayer const * object_layer = layer->GetObjectLayer();
		if (object_layer != nullptr)
		{
			if (!InitializeObjectLayer(object_layer))
				return false;
			return FinalizeParticles(nullptr);
		}

		chaos::TiledMap::TileLayer const* tile_layer = layer->GetTileLayer();
		if (tile_layer != nullptr)
		{
			if (!InitializeTileLayer(tile_layer))
				return false;
			return FinalizeParticles(nullptr);
		}

		return false;
	}

	void TMLayerInstance::OnRestart()
	{
		// clear allocation if required
		if (autoclean_particles && particle_layer != nullptr)
			particle_layer->ClearAllAllocations();
		// restart all objects
		size_t count = objects.size();
		for (size_t i = 0; i < count; ++i)
			objects[i]->OnLevelRestart();
	}

	bool TMLayerInstance::SerializeObjectListFromJSON(nlohmann::json const& json, char const * attribute_name, std::vector<chaos::shared_ptr<TMObject>> & result)
	{
		// in "Objects" array, read all objects, search the ID and apply the data to dedicated object
		nlohmann::json const* objects_json = chaos::JSONTools::GetStructure(json, attribute_name);
		if (objects_json != nullptr && objects_json->is_array())
		{
			for (size_t i = 0; i < objects_json->size(); ++i)
			{
				nlohmann::json const* object_json = chaos::JSONTools::GetStructureByIndex(*objects_json, i);
				if (object_json != nullptr && object_json->is_object())
				{
					int object_id = 0;
					if (chaos::JSONTools::GetAttribute(*object_json, "OBJECT_ID", object_id))
					{
						TMTrigger* trigger = FindTriggerByID(object_id);
						if (trigger != nullptr)
						{
							chaos::LoadFromJSON(*object_json, *trigger); // XXX : the indirection is important to avoid the creation of a new layer_instance
						}
						else
						{

						}
					}
				}
			}
		}
		return true;
	}
	
	bool TMLayerInstance::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!chaos::JSONSerializable::SerializeFromJSON(json))
			return false;
		SerializeObjectListFromJSON(json, "OBJECTS", objects);
		return true;
	}
	
	bool TMLayerInstance::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!chaos::JSONSerializable::SerializeIntoJSON(json))
			return false;

		chaos::JSONTools::SetAttribute(json, "LAYER_ID", GetLayerID());
		chaos::JSONTools::SetAttribute(json, "OBJECTS", objects);
		return true;
	}

	bool TMLayerInstance::InitializeImageLayer(chaos::TiledMap::ImageLayer const * image_layer)
	{
		return true;
	}




	// shuyyy



	void TMLayerInstance::CreateObjectParticles(chaos::TiledMap::GeometricObject const* in_geometric_object, TMObject* object, TMParticlePopulator& particle_populator)
	{
		chaos::TiledMap::Map* tiled_map = level_instance->GetTiledMap();

		// does the object wants the ownership of the particles		
		bool particle_ownership = false;
		if (object != nullptr)
			particle_ownership = in_geometric_object->GetPropertyValueBool("PARTICLE_OWNERSHIP", true); // by default, an object wants to have its particles

		// create additionnal particles (TEXT)
		if (chaos::TiledMap::GeometricObjectText const* text = in_geometric_object->GetObjectText())
		{
			// create particle layer if necessary
			if (CreateParticleLayer() == nullptr) // the generate layer is of  TiledMapParticleTrait => this works fine for Text (except a useless GID per particle)
				return;

			Game* game = GetGame();

			chaos::ParticleTextGenerator::GeneratorResult result;
			chaos::ParticleTextGenerator::GeneratorParams params;

			params.line_height = (float)text->pixelsize;
			params.hotpoint = chaos::Hotpoint::TOP;
			params.position = text->position;
			params.default_color = text->color;

			std::string const* font_name = text->FindPropertyString("FONT_NAME");
			if (font_name != nullptr)
				params.font_info_name = *font_name;

			// create particles
			game->GetTextGenerator()->Generate(text->text.c_str(), result, params);
			
			chaos::ParticleAllocationBase* allocation = chaos::ParticleTextGenerator::CreateTextAllocation(particle_layer.get(), result);
			if (particle_ownership)
				object->allocation = allocation;
		}
		// create additionnal particles (TILES)		
		else if (chaos::TiledMap::GeometricObjectTile const* tile = in_geometric_object->GetObjectTile())
		{
			TMParticlePopulator object_particle_populator = particle_populator;

			TMParticlePopulator* effective_particle_populator = (particle_ownership) ? &object_particle_populator : &particle_populator;

			int gid = tile->gid;

			// search the tile information 
			chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
			if (tile_info.tiledata == nullptr)
				return;
			// create a simple particle
			chaos::box2 particle_box = tile->GetBoundingBox(true);
			if (object != nullptr)
			{
				chaos::TiledMap::GeometricObjectSurface const* surface_object = in_geometric_object->GetObjectSurface();
				if (surface_object != nullptr)
					particle_box = surface_object->GetBoundingBox(false); // shuxxx : the TILE is generated on the same layer then the surface. does it get the layer_offset ????
			}

			chaos::Hotpoint hotpoint = (tile_info.tileset != nullptr) ? tile_info.tileset->object_alignment : chaos::Hotpoint::BOTTOM_LEFT;

			bool keep_aspect_ratio = false;
			effective_particle_populator->AddParticle(tile_info.tiledata->atlas_key.c_str(), hotpoint, particle_box, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), tile->rotation, tile->particle_flags, gid, keep_aspect_ratio);

			// gives the particles to the object
			if (particle_ownership)
			{
				effective_particle_populator->FlushParticles();
				object->allocation = effective_particle_populator->GetParticleAllocation();
			}
		}
	}

	CHAOS_HELP_TEXT(CMD, "-TiledGeometricObject::ForceParticleCreation");

	bool TMLayerInstance::ShouldCreateParticleForObject(chaos::TiledMap::PropertyOwner const * property_owner, TMObject* object) const
	{
#if _DEBUG
		if (chaos::Application::HasApplicationCommandLineFlag("-TiledGeometricObject::ForceParticleCreation")) // CMDLINE
			return true;
#endif			
		return property_owner->GetPropertyValueBool("PARTICLE_CREATION", (object != nullptr) ? object->IsParticleCreationEnabled() : true);
	}

	TMObjectFactory TMLayerInstance::GetObjectFactory(chaos::TiledMap::TypedObject const * in_typed_object)
	{
		TMLevel* level = GetLevel();

		// get a factory for the object (new + Initialize(...) ...)
		TMObjectFactory factory = level->GetObjectFactory(this, in_typed_object);
		if (!factory)
			return nullptr;
		// create a 'final' factory that use previous one + insert the result object in correct list
		TMObjectFactory result = [this, factory](chaos::TiledMap::GeometricObject const * geometric_object)
		{
			TMObject* result = factory(geometric_object);
			if (result != nullptr)
				objects.push_back(result);
			return result;
		};
		return result;
	}


















	// shuyyy


	bool TMLayerInstance::InitializeObjectLayer(chaos::TiledMap::ObjectLayer const * object_layer)
	{
		// search the bounding box (explicit or not)
		chaos::box2 box;
		chaos::box2 explicit_bounding_box;

		// the particle generator
		TMParticlePopulator particle_populator; // this populator is for the WHOLE LAYER
		if (!particle_populator.Initialize(this))
			return false;

		// iterate over all objects
		size_t count = object_layer->geometric_objects.size();
		for (size_t i = 0; i < count; ++i)
		{
			chaos::TiledMap::GeometricObject const * geometric_object = object_layer->geometric_objects[i].get();
			if (geometric_object == nullptr)
				continue;

			// explicit world bounding box
			if (!level_instance->has_explicit_bounding_box && chaos::TiledMapTools::IsWorldBoundingBox(geometric_object))
			{
				chaos::TiledMap::GeometricObjectSurface const* object_surface = geometric_object->GetObjectSurface();
				if (object_surface != nullptr)
				{
					level_instance->explicit_bounding_box = object_surface->GetBoundingBox(true); // in world coordinate	
					level_instance->has_explicit_bounding_box = true;
				}
			}
			// explicit layer bounding box
			if (IsGeometryEmpty(explicit_bounding_box) && chaos::TiledMapTools::IsLayerBoundingBox(geometric_object))
			{
				chaos::TiledMap::GeometricObjectSurface const* object_surface = geometric_object->GetObjectSurface();
				if (object_surface != nullptr)
					explicit_bounding_box = object_surface->GetBoundingBox(false); // in layer coordinates	
			}

			// get factory + create the object
			TMObject* object = nullptr;

			TMObjectFactory factory = GetObjectFactory(geometric_object);
			if (factory)
			{
				object = factory(geometric_object); 
				if (object == nullptr)
					continue; // we have a factory, but fails to create the object
			}

			// create tile if no object created 
			// (XXX : no way yet to know whether this a normal situation because user does not want to create object or whether an error happened)
			if (object == nullptr || ShouldCreateParticleForObject(geometric_object, object))
				CreateObjectParticles(geometric_object, object, particle_populator);
		}

		// final flush
		particle_populator.FlushParticles();
		// update the bounding box
		if (!IsGeometryEmpty(explicit_bounding_box))
			bounding_box = explicit_bounding_box;
		else
			bounding_box = box | particle_populator.GetBoundingBox();

		return true;
	}



	// shuyyy

	bool TMLayerInstance::FinalizeParticles(chaos::ParticleAllocationBase* allocation)
	{
		// no layer, nothing to do !
		if (particle_layer == nullptr)
			return true;
		// no level ?
		TMLevel* level = GetLevel();
		if (level == nullptr)
			return true;
		// initialize each allocations
		if (allocation == nullptr)
		{
			size_t allocation_count = particle_layer->GetAllocationCount();
			for (size_t i = 0; i < allocation_count; ++i)
				if (!level->FinalizeLayerParticles(this, particle_layer->GetAllocation(i)))
					return false;
		}
		// or just specified one
		else
		{
			if (!level->FinalizeLayerParticles(this, allocation))
				return false;
		}
		return true;
	}











	bool TMLayerInstance::InitializeParticleLayer(chaos::ParticleLayerBase* in_particle_layer)
	{
		// the name
		std::string const* renderable_name = layer->FindPropertyString("RENDERABLE_NAME");
		if (renderable_name != nullptr)
			in_particle_layer->SetName(renderable_name->c_str());
		else
			in_particle_layer->SetName(layer->name.c_str());
		// the tag
		std::string const* renderable_tag = layer->FindPropertyString("RENDERABLE_TAG");
		if (renderable_tag != nullptr)
			in_particle_layer->SetTag(chaos::DeclareTag(renderable_tag->c_str()));
		else
		{
			int const* layer_tag = layer->FindPropertyInt("RENDERABLE_TAG");
			if (layer_tag != nullptr)
				in_particle_layer->SetTag((chaos::TagType) * layer_tag);
		}
		// enabled renderpasses
		std::string const* enabled_renderpasses = layer->FindPropertyString("ENABLED_RENDERPASSES");
		if (enabled_renderpasses != nullptr)
			in_particle_layer->AddEnabledRenderPasses(enabled_renderpasses->c_str());
		// disabled renderpasses
		std::string const* disabled_renderpasses = layer->FindPropertyString("DISABLED_RENDERPASSES");
		if (disabled_renderpasses != nullptr)
			in_particle_layer->AddDisabledRenderPasses(disabled_renderpasses->c_str());
		return true;
	}

	chaos::ParticleAllocationBase* TMLayerInstance::SpawnParticles(size_t count, bool new_allocation)
	{
		// create particle layer if necessary
		if (CreateParticleLayer() == nullptr)
			return nullptr;
		// create the allocation
		return particle_layer->SpawnParticles(count, new_allocation);
	}

	chaos::ParticleLayerBase* TMLayerInstance::CreateParticleLayer()
	{
		if (particle_layer == nullptr)
		{
			// find render material
			chaos::GPURenderMaterial* render_material = FindOrCreateRenderMaterial(material_name.c_str());
			if (render_material == nullptr)
				return nullptr;
			// create a particle layer (it is not attached to any particle manager !)
			TMLevel* level = GetLevel();
			particle_layer = level->DoCreateParticleLayer(this);
			if (particle_layer == nullptr)
				return false;
			// add name and tag to the particle_layer
			InitializeParticleLayer(particle_layer.get());
			// set the material
			particle_layer->SetRenderMaterial(render_material);
			// set the atlas			
			particle_layer->SetTextureAtlas(GetGame()->GetTextureAtlas());
		}
		return particle_layer.get();
	}
















	// shuyyy


	bool TMLayerInstance::InitializeTileLayer(chaos::TiledMap::TileLayer const * tile_layer)
	{
		TMLevel* level = GetLevel();

		TMParticlePopulator particle_populator;
		if (!particle_populator.Initialize(this))
			return false;

		// populate the layer for each chunk
		chaos::TiledMap::Map* tiled_map = level_instance->GetTiledMap();

		bool particle_creation_success = true; // as soon as some particle creation fails, do not try to create other particles

		for (chaos::TiledMap::TileLayerChunk const& chunk : tile_layer->tile_chunks)
		{
			size_t count = chunk.tile_indices.size();
			if (count == 0)
				continue;

			for (size_t i = 0; i < count; ++i)
			{
				int gid = chunk.tile_indices[i].id;
				int particle_flags = chunk.tile_indices[i].flags;

				if (gid == 0)
					continue;

				// search the tile information 
				chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
				if (tile_info.tiledata == nullptr)
					continue;

				// prepare data for the tile/object
				glm::ivec2  tile_coord = tile_layer->GetTileCoordinate(chunk, i);
				chaos::box2 particle_box = tile_layer->GetTileBoundingBox(tile_coord, tile_info.tiledata->image_size, particle_flags, false);

				// try to create a geometric object from the tile
				TMObjectFactory factory = GetObjectFactory(tile_info.tiledata);
				if (factory)
				{
					chaos::shared_ptr<chaos::TiledMap::GeometricObjectTile> tile_object = new PropertyOwnerOverride<chaos::TiledMap::GeometricObjectTile>(nullptr, tile_info.tiledata);
					if (tile_object != nullptr)
					{
						// compute an ID base on 'tile_coord' 
						// TiledMap gives positive ID
						// we want a negative ID to avoid conflicts
						// for a 32 bits integer
						// 15 bits for X
						// 15 bits for Y
						// 1  unused
						// 1  bit for sign

						int int_bit_count = 8 * sizeof(int);
						int per_component_bit_count = (int_bit_count - 1) / 2;
						int mask = ~(-1 << per_component_bit_count);
						int idx = tile_coord.x & mask;
						int idy = tile_coord.y & mask;
						int object_id = -1 * (idx | (idy << per_component_bit_count));

						tile_object->id = object_id;
						tile_object->gid = gid;
						tile_object->type = tile_info.tiledata->type;
						tile_object->size = particle_box.half_size * 2.0f;
						tile_object->position.x = particle_box.position.x - particle_box.half_size.x;
						tile_object->position.y = particle_box.position.y - particle_box.half_size.y;

						// XXX : for player start : but this is not a great idea to process by exception
						//       We are writing int the fake object properties, not in the 'tile_info.tiledata'
						//       That means that if 'tile_info.tiledata' already has a BITMAP_NAME property, this does not
						//       interfere with that (a just in case value)
						tile_object->CreatePropertyString("BITMAP_NAME", tile_info.tiledata->atlas_key.c_str());

						TMObject* object = factory(tile_object.get());
						if (object != nullptr)
							if (ShouldCreateParticleForObject(tile_object.get(), object))
								CreateObjectParticles(tile_object.get(), object, particle_populator);
					}
					continue; // while we have a factory, let the concerned object create its particle
				}

				chaos::Hotpoint hotpoint = chaos::Hotpoint::BOTTOM_LEFT;

				// create a simple particle
				bool keep_aspect_ratio = true;

				if (particle_creation_success)
					particle_creation_success = particle_populator.AddParticle(tile_info.tiledata->atlas_key.c_str(), hotpoint, particle_box, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, particle_flags, gid, keep_aspect_ratio);
			}
		}

		// final flush
		if (particle_creation_success)
			particle_populator.FlushParticles();
		// update the bounding box
		bounding_box = particle_populator.GetBoundingBox();

		return true;
	}

























	bool TMLayerInstance::DoTick(float delta_time)
	{
		// objects
		size_t object_count = objects.size();
		for (size_t i = 0; i < object_count; ++i)
			objects[i]->Tick(delta_time);
		// tick the particles
		if (particle_layer != nullptr)
			particle_layer->Tick(delta_time);
		return true;
	}

	int TMLayerInstance::DoDisplay(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params)
	{
		// early exit
		int result = 0;
		if (particle_layer == nullptr)
			return result;

		// camera is expressed in world, so is for layer
		chaos::obox2 camera_obox = GetLevelInstance()->GetCameraOBox(0);
		chaos::obox2 initial_camera_obox = GetLevelInstance()->GetInitialCameraOBox(0);


		// XXX : we want some layers to appear further or more near the camera
		//       the displacement_ratio represent how fast this layer is moving relatively to other layers.
		//       The reference layer is the layer where the 'effective' camera (and so the PlayerStart is)
		//         => when player goes outside the screen, the camera is updated so that it is still watching the player
		//         => that why we consider the PlayerStart's layer as the reference
		//       to simulate other layer's speed, we just create at rendering time 'virtual cameras' (here this is 'final_camera_box')
		//       We only multiply 'true camera' distance from its initial position by a ratio value

		// apply the displacement to the camera

		glm::vec2 final_ratio = glm::vec2(1.0f, 1.0f);

		TMLayerInstance const* reference_layer = level_instance->reference_layer.get();
		if (reference_layer != nullptr)
		{
			if (reference_layer->displacement_ratio.x != 0.0f)
				final_ratio.x = displacement_ratio.x / reference_layer->displacement_ratio.x;
			if (reference_layer->displacement_ratio.y != 0.0f)
				final_ratio.y = displacement_ratio.y / reference_layer->displacement_ratio.y;

		}

		// shulayer
		chaos::box2 layer_box = GetBoundingBox(true);

#if 0
		chaos::box2 reference_box = reference_layer->GetBoundingBox(true);
		if (!IsGeometryEmpty(layer_box) && !IsGeometryEmpty(reference_box))
			final_ratio = layer_box.half_size / reference_box.half_size;
#endif

		chaos::obox2 final_camera_obox;
		final_camera_obox.position = initial_camera_obox.position + (camera_obox.position - initial_camera_obox.position) * final_ratio;
		final_camera_obox.half_size = initial_camera_obox.half_size + (camera_obox.half_size - initial_camera_obox.half_size) * final_ratio;

		// compute repetitions
		chaos::RepeatedBoxScissor scissor = chaos::RepeatedBoxScissor(layer_box, chaos::GetBoundingBox(final_camera_obox), wrap_x, wrap_y);

		// new provider for camera override (will be fullfill only if necessary)
		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddVariableValue("camera_transform", CameraTransform::GetCameraTransform(final_camera_obox));

		chaos::box2 final_camera_box;
		final_camera_box.position = final_camera_obox.position;
		final_camera_box.half_size = final_camera_obox.half_size;
		main_uniform_provider.AddVariableValue("camera_box", chaos::EncodeBoxToVector(final_camera_box));


		// HACK : due to bad LAYER_BOUNDING_BOX computation, the layer containing PLAYER_START may be clamped and layer hidden
		glm::ivec2 start_instance = scissor.start_instance;
		glm::ivec2 last_instance = scissor.last_instance;
		if (this == reference_layer || IsGeometryEmpty(layer_box))
		{
			start_instance = glm::ivec2(0, 0);
			last_instance = glm::ivec2(1, 1); // always see fully the layer without clamp => repetition not working
		}

		// draw instances 
		int draw_instance_count = 0;
		for (int x = start_instance.x; x < last_instance.x; ++x)
		{
			for (int y = start_instance.y; y < last_instance.y; ++y)
			{
				// new Provider to apply the offset for this 'instance'
				chaos::GPUProgramProviderChain instance_uniform_provider(&main_uniform_provider);
				glm::vec2 instance_offset = scissor.GetInstanceOffset(glm::ivec2(x, y));
				instance_uniform_provider.AddVariableValue("offset", instance_offset + offset);
				// draw call
				result += particle_layer->Display(renderer, &instance_uniform_provider, render_params);
			}
		}
		return result;
	}

#define DEATH_FIND_OBJECT(result_type, func_name, constness)\
		result_type constness * TMLayerInstance::func_name(chaos::ObjectRequest request) constness\
		{\
			return auto_cast(request.FindObject(objects));\
		}\
		result_type constness* TMLayerInstance::func_name##ByID(int id) constness\
		{\
			size_t count = objects.size();\
			for (size_t i = 0; i < objects.size(); ++i)\
				if (objects[i]->GetObjectID() == id)\
					return auto_cast(objects[i].get());\
			return nullptr;\
		}
	DEATH_FIND_OBJECT(TMObject, FindObject, BOOST_PP_EMPTY());
	DEATH_FIND_OBJECT(TMObject, FindObject, const);
	DEATH_FIND_OBJECT(TMTrigger, FindTrigger, BOOST_PP_EMPTY());
	DEATH_FIND_OBJECT(TMTrigger, FindTrigger, const);
	DEATH_FIND_OBJECT(TMPlayerStart, FindPlayerStart, BOOST_PP_EMPTY());
	DEATH_FIND_OBJECT(TMPlayerStart, FindPlayerStart, const);
	DEATH_FIND_OBJECT(TMCameraTemplate, FindCameraTemplate, BOOST_PP_EMPTY());
	DEATH_FIND_OBJECT(TMCameraTemplate, FindCameraTemplate, const);

#undef DEATH_FIND_OBJECT
	
	size_t TMLayerInstance::GetObjectCount() const
	{
		return objects.size();
	}

	TMObject* TMLayerInstance::GetObject(size_t index)
	{
		if (index >= objects.size())
			return nullptr;
		return objects[index].get();
	}

	TMObject const* TMLayerInstance::GetObject(size_t index) const
	{
		if (index >= objects.size())
			return nullptr;
		return objects[index].get();
	}

	void TMLayerInstance::OnLevelEnded()
	{
		//  objects
		size_t object_count = objects.size();
		for (size_t i = 0; i < object_count; ++i)
			objects[i]->OnLevelEnded();
	}

	void TMLayerInstance::OnLevelStarted()
	{
		//  objects
		size_t object_count = objects.size();
		for (size_t i = 0; i < object_count; ++i)
			objects[i]->OnLevelStarted();
	}

	// =====================================
	// TMTriggerCollisionInfo implementation
	// =====================================

	bool TMTriggerCollisionInfo::FindTrigger(TMTrigger const* trigger) const
	{
		return std::find_if(triggers.begin(), triggers.end(), [trigger](chaos::weak_ptr<TMTrigger> const& p) {return p.get() == trigger; }) != triggers.end();
	}

	// =====================================
	// TMLevelInstance implementation
	// =====================================

	chaos::TiledMap::Map* TMLevelInstance::GetTiledMap()
	{
		TMLevel* level = GetLevel();
		if (level == nullptr)
			return nullptr;
		return level->GetTiledMap();
	}

	void TMLevelInstance::OnRestart()
	{
		LevelInstance::OnRestart();
		
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			layer_instances[i]->OnRestart();
	}

	chaos::TiledMap::Map const* TMLevelInstance::GetTiledMap() const
	{
		TMLevel const* level = GetLevel();
		if (level == nullptr)
			return nullptr;
		return level->GetTiledMap();
	}

	void TMLevelInstance::PurgeCollisionInfo()
	{
		size_t count = collision_info.size();
		for (size_t i = count; i > 0; --i)
		{
			size_t index = i - 1;
			if (collision_info[index].object == nullptr)
			{
				std::swap(collision_info[index], collision_info[collision_info.size() - 1]);
				collision_info.pop_back();
			}
		}
	}

	TMTriggerCollisionInfo* TMLevelInstance::FindTriggerCollisionInfo(chaos::Object* object)
	{
		size_t count = collision_info.size();
		for (size_t i = 0; i < count ; ++i)
			if (collision_info[i].object == object)
				return &collision_info[i];
		return nullptr;
	}

	void TMLevelInstance::HandleTriggerCollisions(float delta_time, chaos::Object* object, chaos::box2 const& box, int mask)
	{
		TMTriggerCollisionInfo* previous_collisions = FindTriggerCollisionInfo(object);

		TMTriggerCollisionInfo new_collisions;

		// search all new collisions
		for (TMTriggerCollisionIterator it = GetTriggerCollisionIterator(box, mask) ; it ; ++it)
		{
			TMTrigger& trigger = *it;
			// trigger only enabled trigger
			if (!trigger.IsEnabled())
				continue;
			// trigger once : do not trigger anymore entering events
			if (trigger.IsTriggerOnce() && trigger.enter_event_triggered)
				continue;
			// collision type
			chaos::CollisionType collision_type = chaos::CollisionType::STARTED;
			if (previous_collisions != nullptr)
				if (previous_collisions->FindTrigger(&trigger))
					collision_type = chaos::CollisionType::AGAIN;
			// check for collision (bounding box may change when wanting to go outside)
			if (trigger.IsCollisionWith(box, collision_type))
				new_collisions.triggers.push_back(&trigger);
		}


		// triggers collisions 
		size_t new_collision_count = new_collisions.triggers.size();
		for (size_t i = 0; i < new_collision_count; ++i)
		{
			TMTrigger* trigger = new_collisions.triggers[i].get();

			// search in previous frame data
			chaos::CollisionType collision_type = chaos::CollisionType::STARTED;
			if (previous_collisions != nullptr)
				if (previous_collisions->FindTrigger(trigger))
					collision_type = chaos::CollisionType::AGAIN;

			// trigger event
			if (trigger->OnCollisionEvent(delta_time, object, collision_type))
			{
				if (trigger->IsTriggerOnce() && !trigger->enter_event_triggered)
				{
					trigger->enter_event_triggered = true;
				}
			}
		}

		// triggers end of collisions
		if (previous_collisions != nullptr)
		{
			size_t previous_count = previous_collisions->triggers.size();
			for (size_t i = 0; i < previous_count; ++i)
			{
				if (std::find(new_collisions.triggers.begin(), new_collisions.triggers.end(), previous_collisions->triggers[i]) == new_collisions.triggers.end()) // no more colliding
					previous_collisions->triggers[i]->OnCollisionEvent(delta_time, object, chaos::CollisionType::FINISHED);
			}
		}

		// store the record
		if (previous_collisions != nullptr)
		{
			if (new_collisions.triggers.size() == 0)
				collision_info.erase(std::remove_if(collision_info.begin(), collision_info.end(), [object](TMTriggerCollisionInfo const& info) { return info.object == object; }), collision_info.end());
			else
				previous_collisions->triggers = std::move(new_collisions.triggers);
		}
		else if(new_collisions.triggers.size() > 0)
		{
			new_collisions.object = object;		
			collision_info.push_back(std::move(new_collisions));
		}
	}


	void TMLevelInstance::HandlePlayerTriggerCollisions(float delta_time)
	{
		// compute the collisions for all players
		size_t player_count = game->GetPlayerCount();
		for (size_t i = 0; i < player_count; ++i)
		{
			Player* player = game->GetPlayer(i);
			if (player == nullptr)
				continue;

			PlayerPawn* player_pawn = player->GetPawn();
			if (player_pawn == nullptr)
				continue;

			chaos::box2 pawn_box = player_pawn->GetBox();
			if (chaos::IsGeometryEmpty(pawn_box))
				continue;

			HandleTriggerCollisions(delta_time, player, pawn_box, death::CollisionMask::PLAYER);
		}
	}

	void TMLevelInstance::HandleCameraTriggerCollisions(float delta_time)
	{
		// compute the collisions for all cameras
		size_t camera_count = game->GetCameraCount();
		for (size_t i = 0; i < camera_count; ++i)
		{
			Camera* camera = game->GetCamera(i);
			if (camera == nullptr)
				continue;

			chaos::box2 camera_box = camera->GetCameraBox();
			if (chaos::IsGeometryEmpty(camera_box))
				continue;

			HandleTriggerCollisions(delta_time, camera, camera_box, death::CollisionMask::CAMERA);
		}
	}

	bool TMLevelInstance::DoTick(float delta_time)
	{
		death::LevelInstance::DoTick(delta_time);

		// tick the particle manager
		if (particle_manager != nullptr)
			particle_manager->Tick(delta_time);
		// tick all layer instances
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			layer_instances[i]->Tick(delta_time);
		// purge collision info for object that may have been destroyed
		PurgeCollisionInfo();
		// compute the collisions with the player
		HandlePlayerTriggerCollisions(delta_time);
		// compute the collisions with the camera
		HandleCameraTriggerCollisions(delta_time);

		return true;
	}

	int TMLevelInstance::DoDisplay(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params)
	{
		int result = 0;

		// display particle manager
		if (particle_manager != nullptr)
			result += particle_manager->Display(renderer, uniform_provider, render_params);
		// draw the layer instances
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			result += layer_instances[i]->Display(renderer, uniform_provider, render_params);

		return result;
	}

	bool TMLevelInstance::Initialize(Game* in_game, death::Level* in_level)
	{
		if (!death::LevelInstance::Initialize(in_game, in_level))
			return false;
		// create a the layers instances
		if (!CreateLayerInstances(in_game))
			return false;
		// create a particle manager
		if (!CreateParticleManager(in_game))
			return false;

		return true;
	}

	bool TMLevelInstance::CreateParticleManager(Game* in_game)
	{
		particle_manager = new chaos::ParticleManager;
		if (particle_manager == nullptr)
			return false;
		particle_manager->SetTextureAtlas(in_game->GetTextureAtlas()); // take the atlas
		return true;
	}

	bool TMLevelInstance::DoCreateLayerInstances(std::vector<chaos::shared_ptr<chaos::TiledMap::LayerBase>> const & layers)
	{
		TMLevel* level = GetLevel();

		for (auto& layer : layers)
		{
			// insert the new layer
			TMLayerInstance* layer_instance = level->CreateLayerInstance(this, layer.get());
			if (layer_instance != nullptr)
				layer_instances.push_back(layer_instance);
			// for layer group iterate over child layers
			if (chaos::TiledMap::GroupLayer const* group_layer = auto_cast(layer.get()))
				if (!DoCreateLayerInstances(group_layer->layers))
					return false;
		}
		return true;
	}

	bool TMLevelInstance::CreateLayerInstances(Game* in_game)
	{
		chaos::TiledMap::Map* tiled_map = GetTiledMap();
		if (tiled_map != nullptr)
			return DoCreateLayerInstances(tiled_map->layers);
		return true;
	}

	chaos::box2 TMLevelInstance::GetBoundingBox() const
	{
		// explicit bounding box
		if (has_explicit_bounding_box)
			return explicit_bounding_box;
		// depend on layers
		chaos::box2 result;
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			result = result | layer_instances[i]->GetBoundingBox(true); // expressed in world system the bounding boxes
		return result;
	}

	chaos::GPURenderMaterial* TMLevelInstance::GetDefaultRenderMaterial()
	{
		if (default_material == nullptr)
		{
			TMLevel* level = GetLevel();
			default_material = level->GenDefaultRenderMaterial(); // create material and cache
		}
		return default_material.get();
	}

	TMLayerInstance* TMLevelInstance::FindLayerInstanceByID(int id)
	{
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			if (layer_instances[i]->GetLayerID() == id)
				return layer_instances[i].get();
		return nullptr;
	}

	TMLayerInstance const* TMLevelInstance::FindLayerInstanceByID(int id) const
	{
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			if (layer_instances[i]->GetLayerID() == id)
				return layer_instances[i].get();
		return nullptr;
	}

#define DEATH_FIND_OBJECT(result_type, func_name, constness)\
	result_type constness * TMLevelInstance::func_name(chaos::ObjectRequest request) constness\
	{\
		size_t count = layer_instances.size();\
		for (size_t i = 0; i < count; ++i)\
		{\
			result_type constness * result = layer_instances[i]->func_name(request);\
			if (result != nullptr)\
				return result;\
		}\
		return nullptr;\
	}
	DEATH_FIND_OBJECT(TMObject, FindObject, BOOST_PP_EMPTY());
	DEATH_FIND_OBJECT(TMObject, FindObject, const);
	DEATH_FIND_OBJECT(TMTrigger, FindTrigger, BOOST_PP_EMPTY());
	DEATH_FIND_OBJECT(TMTrigger, FindTrigger, const);
	DEATH_FIND_OBJECT(TMPlayerStart, FindPlayerStart, BOOST_PP_EMPTY());
	DEATH_FIND_OBJECT(TMPlayerStart, FindPlayerStart, const);
	DEATH_FIND_OBJECT(TMCameraTemplate, FindCameraTemplate, BOOST_PP_EMPTY());
	DEATH_FIND_OBJECT(TMCameraTemplate, FindCameraTemplate, const);

#undef DEATH_FIND_OBJECT

	TMLayerInstance* TMLevelInstance::FindLayerInstance(chaos::ObjectRequest request)
	{
		return request.FindObject(layer_instances);
	}
	TMLayerInstance const* TMLevelInstance::FindLayerInstance(chaos::ObjectRequest request) const
	{
		return request.FindObject(layer_instances);
	}

	void TMLevelInstance::CreateCameras()
	{
		TMLevel* level = GetLevel();

		// search CAMERA NAME
		std::string const* camera_name = level->GetTiledMap()->FindPropertyString("CAMERA_NAME");

		// search the CAMERA
		TMCameraTemplate* camera_template = nullptr;
		if (camera_name != nullptr)
			camera_template = FindCameraTemplate(*camera_name); // first, if a name is given, use it
		if (camera_template == nullptr)
		{
			camera_template = FindCameraTemplate(chaos::ObjectRequest::Any()); // try to find the very first one otherwise
			if (camera_template == nullptr)
				return;
		}

		// get the aspect ratio
		float aspect_ratio = 16.0f / 9.0f;

		Game const* game = GetGame();
		if (game != nullptr)
			aspect_ratio = game->GetViewportWantedAspect();

		// compute the surface
		chaos::box2 camera_box = chaos::AlterBoxToAspect(camera_template->GetBoundingBox(true), aspect_ratio, true);

		// create the real camera
		Camera* camera = CreateCamera();
		if (camera == nullptr)
			return;
		cameras.push_back(camera);

		// initialize the camera
		camera->SetCameraBox(camera_box);
	}


	TMPlayerStart* TMLevelInstance::GetPlayerStartForPawn(Player* player)
	{
		TMLevel* level = GetLevel();
		if (level == nullptr)
			return nullptr;

		// search PLAYER START NAME
		std::string const* player_start_name = level->GetTiledMap()->FindPropertyString("PLAYER_START_NAME");

		// search the PLAYER START
		TMPlayerStart* result = nullptr;
		if (player_start_name != nullptr)
			result = FindPlayerStart(player_start_name->c_str()); // first, if a name is given, use it
		if (result == nullptr)
			result = FindPlayerStart(chaos::ObjectRequest::Any()); // try to find the very first one otherwise
		return result;
	}

	PlayerPawn* TMLevelInstance::CreatePlayerPawn(Player* player, TMPlayerStart* player_start, TMLayerInstance* layer_instance)	
	{
		// create a particle populator
		TMParticlePopulator particle_populator;
		if (!particle_populator.Initialize(layer_instance))
			return nullptr;

		// create the particle
		int player_gid = 0;
		int particle_flags = 0;
		bool keep_aspect_ratio = true;

		chaos::Hotpoint hotpoint = chaos::Hotpoint::BOTTOM_LEFT;

		particle_populator.AddParticle(player_start->bitmap_name.c_str(), hotpoint, player_start->GetBoundingBox(true), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, particle_flags, player_gid, keep_aspect_ratio);
		particle_populator.FlushParticles();

		// get the allocation and finalize the layer
		chaos::ParticleAllocationBase* player_allocation = particle_populator.GetParticleAllocation();
		if (player_allocation == nullptr)
			return nullptr;
		// shuxxx : first time FinalizeParticles(...) was called, there was no effect because the PlayerStartLayer has no particle. 
		//          call it twice as a fast fix
		layer_instance->FinalizeParticles(player_allocation);

		// create a pawn 
		PlayerPawn* result = LevelInstance::CreatePlayerPawn(player);
		if (result == nullptr)
			return result;

		result->SetAllocation(player_allocation);
		return result;
	}

	PlayerPawn * TMLevelInstance::CreatePlayerPawn(Player* player)
	{
		// find the player start to use for the player
		TMPlayerStart* player_start = GetPlayerStartForPawn(player);
		if (player_start == nullptr)
			return nullptr;

		// initialize some data
		TMLayerInstance* layer_instance = player_start->GetLayerInstance();
		if (layer_instance == nullptr)
			return nullptr;

		// XXX : while camera, is restricted so we can see player, we considere that the displacement_ratio of the layer containing the player start is the reference one
		reference_layer = layer_instance;

		return CreatePlayerPawn(player, player_start, layer_instance);
	}

	void TMLevelInstance::CreateBackgroundImage()
	{
		std::string const* background_material = nullptr;
		std::string const* background_texture = nullptr;

		TMLevel const* level = GetLevel();
		if (level != nullptr)
		{
			background_material = level->GetTiledMap()->FindPropertyString("BACKGROUND_MATERIAL");
			background_texture = level->GetTiledMap()->FindPropertyString("BACKGROUND_TEXTURE");
		}

		game->CreateBackgroundImage(
			(background_material == nullptr) ? nullptr : background_material->c_str(),
			(background_texture == nullptr) ? nullptr : background_texture->c_str());
	}

	void TMLevelInstance::SetInGameMusic()
	{
		TMLevel const* level = GetLevel();
		if (level != nullptr)
		{
			std::string const* level_music = level->GetTiledMap()->FindPropertyString("MUSIC");
			if (level_music != nullptr)
			{
				game->SetInGameMusic(level_music->c_str());
				return;
			}
		}
		death::LevelInstance::SetInGameMusic();
	}

	void TMLevelInstance::OnLevelEnded()
	{
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			layer_instances[i]->OnLevelEnded();

		death::LevelInstance::OnLevelEnded();
	}

	void TMLevelInstance::OnLevelStarted()
	{
		death::LevelInstance::OnLevelStarted();

		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			layer_instances[i]->OnLevelStarted();
	}

	TMTileCollisionIterator TMLevelInstance::GetTileCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask)
	{
		return TMTileCollisionIterator(this, in_collision_box, in_collision_mask);
	}

	TMTriggerCollisionIterator TMLevelInstance::GetTriggerCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask)
	{
		return TMTriggerCollisionIterator(this, in_collision_box, in_collision_mask);
	}

	TMObjectCollisionIterator TMLevelInstance::GetObjectCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask)
	{
		return TMObjectCollisionIterator(this, in_collision_box, in_collision_mask);
	}

	bool TMLevelInstance::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!LevelInstance::SerializeFromJSON(json))
			return false;

		// in "Layers" array, read all objects, search the ID and apply the data to dedicated layer instance
		nlohmann::json const* layers_json = chaos::JSONTools::GetStructure(json, "LAYERS");
		if (layers_json != nullptr && layers_json->is_array())
		{			
			for (size_t i = 0; i < layers_json->size(); ++i)
			{
				nlohmann::json const* layer_json = chaos::JSONTools::GetStructureByIndex(*layers_json, i);
				if (layer_json != nullptr && layer_json->is_object())
				{
					int layer_id = 0;
					if (chaos::JSONTools::GetAttribute(*layer_json, "LAYER_ID", layer_id))
					{
						TMLayerInstance * layer_instance = FindLayerInstanceByID(layer_id);
						if (layer_instance != nullptr)
							LoadFromJSON(*layer_json, *layer_instance); // XXX : the indirection is important to avoid the creation of a new layer_instance
					}
				}
			}
		}
		return true;
	}

	bool TMLevelInstance::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!LevelInstance::SerializeIntoJSON(json))
			return false;
		chaos::JSONTools::SetAttribute(json, "LAYERS", layer_instances);
		return true;
	}

}; // namespace death