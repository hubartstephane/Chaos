#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// =====================================
	// TMObject implementation
	// =====================================

	box2 TMObject::GetBoundingBox(bool world_system) const
	{
		box2 result = bounding_box;
		if (world_system)
			result.position += layer_instance->GetLayerOffset();
		return result;
	}

	bool TMObject::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
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
		rotation = in_geometric_object->rotation;
		// extract the bounding box
		bounding_box = in_geometric_object->GetBoundingBox(false);  // make our own correction for world system because the LayerInstance can change its offset
		return true;
	}

	bool TMObject::SerializeFromJSON(nlohmann::json const * json)
	{
		if (!GameEntity::SerializeFromJSON(json))
			return false;
		JSONTools::GetAttribute(json, "NAME", name);
		JSONTools::GetAttribute(json, "OBJECT_ID", id);
		JSONTools::GetAttribute(json, "PARTICLE_OWNERSHIP", particle_ownership);
		return true;
	}

	bool TMObject::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!GameEntity::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "NAME", name);
		JSONTools::SetAttribute(json, "OBJECT_ID", id);
		JSONTools::SetAttribute(json, "PARTICLE_OWNERSHIP", particle_ownership);
		return true;
	}

	bool TMObject::IsParticleCreationEnabled() const
	{
		return true;
	}

	// =====================================
	// TMPath implementation
	// =====================================

	bool TMPath::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
	{
		if (!TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
			return false;



		return true;
	}

	// =====================================
	// TMTrigger implementation
	// =====================================

	bool TMTrigger::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
	{
		if (!TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
			return false;
		// default values are set to the one defined by default in constructor
		enabled = in_geometric_object->GetPropertyValueBool("ENABLED", enabled);
		trigger_once = in_geometric_object->GetPropertyValueBool("TRIGGER_ONCE", trigger_once);
		outside_box_factor = in_geometric_object->GetPropertyValueFloat("OUTSIDE_BOX_FACTOR", outside_box_factor);

		// enable the possibility to trigger once again the object
		enter_event_triggered = false;
		return true;
	}

	bool TMTrigger::SerializeFromJSON(nlohmann::json const * json)
	{
		if (!TMObject::SerializeFromJSON(json))
			return false;
		JSONTools::GetAttribute(json, "ENABLED", enabled);
		JSONTools::GetAttribute(json, "TRIGGER_ONCE", trigger_once);
		JSONTools::GetAttribute(json, "OUTSIDE_BOX_FACTOR", outside_box_factor);
		JSONTools::GetAttribute(json, "ENTER_EVENT_TRIGGERED", enter_event_triggered);
		return true;
	}

	bool TMTrigger::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!TMObject::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "ENABLED", enabled);
		JSONTools::SetAttribute(json, "TRIGGER_ONCE", trigger_once);
		JSONTools::SetAttribute(json, "OUTSIDE_BOX_FACTOR", outside_box_factor);
		JSONTools::SetAttribute(json, "ENTER_EVENT_TRIGGERED", enter_event_triggered);
		return true;
	}

	bool TMTrigger::IsCollisionWith(box2 const& other_box, CollisionType collision_type) const
	{
		box2 box = GetBoundingBox(true);

		// the box is bigger when we want to go outside !
		if (collision_type == CollisionType::AGAIN && outside_box_factor > 1.0f)
			box.half_size *= outside_box_factor;

		return Collide(other_box, box);
	}

	bool TMTrigger::OnCollisionEvent(float delta_time, Object* object, CollisionType event_type)
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

	bool TMCheckpointTrigger::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
	{
		if (!TMTrigger::Initialize(in_layer_instance, in_geometric_object, reference_solver))
			return false;
		trigger_once = true; // force a trigger once for checkpoint
		return true;
	}

	bool TMCheckpointTrigger::OnCollisionEvent(float delta_time, Object* object, CollisionType event_type)
	{
		Camera* camera = auto_cast(object);
		if (camera == nullptr)
			return false;

		if (event_type != CollisionType::STARTED)
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

	bool TMPlayerStart::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
	{
		if (!TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
			return false;
		// search the bitmap name for the player
		std::string const* in_bitmap_name = in_geometric_object->FindPropertyString("BITMAP_NAME");
		if (in_bitmap_name == nullptr)
			return false;
		bitmap_name = *in_bitmap_name;
		return true;
	}

	bool TMPlayerStart::SerializeFromJSON(nlohmann::json const * json)
	{
		if (!TMObject::SerializeFromJSON(json))
			return false;
		JSONTools::GetAttribute(json, "BITMAP_NAME", bitmap_name);
		return true;
	}

	bool TMPlayerStart::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!TMObject::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "BITMAP_NAME", bitmap_name);
		return true;
	}

	// =================================================
	// TMNotificationTrigger
	// =================================================

	bool TMNotificationTrigger::IsParticleCreationEnabled() const
	{
		return false;
	}

	bool TMNotificationTrigger::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
	{
		if (!TMTrigger::Initialize(in_layer_instance, in_geometric_object, reference_solver))
			return false;

		notification_string = in_geometric_object->GetPropertyValueString("NOTIFICATION", "");
		notification_lifetime = in_geometric_object->GetPropertyValueFloat("LIFETIME", notification_lifetime);
		stop_when_collision_over = in_geometric_object->GetPropertyValueBool("STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		player_collision = in_geometric_object->GetPropertyValueBool("PLAYER_COLLISION", player_collision);

		return true;
	}

	bool TMNotificationTrigger::SerializeFromJSON(nlohmann::json const * json)
	{
		if (!TMTrigger::SerializeFromJSON(json))
			return false;
		JSONTools::GetAttribute(json, "NOTIFICATION", notification_string);
		JSONTools::GetAttribute(json, "LIFETIME", notification_lifetime);
		JSONTools::GetAttribute(json, "STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		JSONTools::GetAttribute(json, "PLAYER_COLLISION", player_collision);
		return true;
	}

	bool TMNotificationTrigger::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!TMTrigger::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "NOTIFICATION", notification_string);
		JSONTools::SetAttribute(json, "LIFETIME", notification_lifetime);
		JSONTools::SetAttribute(json, "STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		JSONTools::SetAttribute(json, "PLAYER_COLLISION", player_collision);
		return true;
	}


	bool TMNotificationTrigger::OnCollisionEvent(float delta_time, Object* object, CollisionType event_type)
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
		if (event_type != CollisionType::STARTED && event_type != CollisionType::FINISHED) // ignore AGAIN event
			return false;
		if (event_type == CollisionType::FINISHED && !stop_when_collision_over) // ignore FINISHED if you do not want to kill the notification
			return false;
		// get some variables 
		Game* game = layer_instance->GetGame();
		if (game == nullptr)
			return false;
		GameHUD* hud = game->GetCurrentHUD();
		if (hud == nullptr)
			return false;
		GameHUDNotificationComponent* notification_component = hud->FindComponentByClass<GameHUDNotificationComponent>();
		if (notification_component == nullptr)
			return false;
		// show notification
		if (event_type == CollisionType::STARTED)
			notification_component->ShowNotification(notification_string.c_str(), notification_lifetime);
		// hide notification
		else if (event_type == CollisionType::FINISHED) // XXX : 'stop_when_collision_over' has already be checked
			notification_component->HideNotification();
		return true;
	}

	// =====================================
	// TMSoundTrigger implementation
	// =====================================

	bool TMSoundTrigger::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
	{
		if (!TMTrigger::Initialize(in_layer_instance, in_geometric_object, reference_solver))
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

	bool TMSoundTrigger::SerializeFromJSON(nlohmann::json const * json)
	{
		if (!TMTrigger::SerializeFromJSON(json))
			return false;
		JSONTools::GetAttribute(json, "SOUND_NAME", sound_name);
		JSONTools::GetAttribute(json, "MIN_DISTANCE_RATIO", min_distance_ratio);
		JSONTools::GetAttribute(json, "PAUSE_TIMER_WHEN_TOO_FAR", pause_timer_when_too_far);
		JSONTools::GetAttribute(json, "3D_SOUND", is_3D_sound);
		JSONTools::GetAttribute(json, "LOOPING", looping);
		JSONTools::GetAttribute(json, "STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		return true;
	}

	bool TMSoundTrigger::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!TMTrigger::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "SOUND_NAME", sound_name);
		JSONTools::SetAttribute(json, "MIN_DISTANCE_RATIO", min_distance_ratio);
		JSONTools::SetAttribute(json, "PAUSE_TIMER_WHEN_TOO_FAR", pause_timer_when_too_far);
		JSONTools::SetAttribute(json, "3D_SOUND", is_3D_sound);
		JSONTools::SetAttribute(json, "LOOPING", looping);
		JSONTools::SetAttribute(json, "STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
		return true;
	}

	Sound* TMSoundTrigger::CreateSound() const
	{
		// early exit
		if (sound_name.length() == 0)
			return nullptr;

		// create the sound
		Sound* result = nullptr;

		Game* game = layer_instance->GetGame();
		if (game != nullptr)
		{
			box2 box = GetBoundingBox(true);
			glm::vec2   position = box.position;

			PlaySoundDesc play_desc;
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

	bool TMSoundTrigger::OnCollisionEvent(float delta_time, Object* object, CollisionType event_type)
	{
		Camera* camera = auto_cast(object);
		if (camera == nullptr)
			return false;

		if (event_type == CollisionType::STARTED)
		{
			Sound* new_sound = CreateSound();
			if (stop_when_collision_over)
				sound = new_sound;
			return true;
		}
		if (event_type == CollisionType::FINISHED)
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

	bool TMChangeLevelTrigger::OnCollisionEvent(float delta_time, Object* object, CollisionType event_type)
	{
		Player* player = auto_cast(object);
		if (player == nullptr)
			return false;

		if (event_type != CollisionType::STARTED)
			return false;

		Game* game = layer_instance->GetGame();
		if (game != nullptr)
		{
			LevelInstance* level_instance = game->GetLevelInstance();
			if (level_instance != nullptr)
				level_instance->SetLevelCompletionFlag();
		}
		return true;
	}

	bool TMChangeLevelTrigger::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
	{
		if (!TMTrigger::Initialize(in_layer_instance, in_geometric_object, reference_solver))
			return false;
		level_name = in_geometric_object->GetPropertyValueString("LEVEL_NAME", "");
		player_start_name = in_geometric_object->GetPropertyValueString("PLAYER_START_NAME", "");
		return true;
	}

	bool TMChangeLevelTrigger::SerializeFromJSON(nlohmann::json const * json)
	{
		if (!TMTrigger::SerializeFromJSON(json))
			return false;
		JSONTools::GetAttribute(json, "LEVEL_NAME", level_name);
		JSONTools::GetAttribute(json, "PLAYER_START_NAME", player_start_name);
		return true;
	}

	bool TMChangeLevelTrigger::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!TMTrigger::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "LEVEL_NAME", level_name);
		JSONTools::SetAttribute(json, "PLAYER_START_NAME", player_start_name);
		return true;
	}

	// =====================================
	// TMCameraTemplate implementation
	// =====================================

	bool TMCameraTemplate::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
	{
		if (!TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
			return false;
		// get the camera component classes
		if (std::string const* names = in_geometric_object->FindPropertyString("CAMERA_COMPONENTS"))
			camera_component_classes = GetSubClassesFromString<CameraComponent>(names->c_str(), ',');
		return true;
	}

}; // namespace chaos
