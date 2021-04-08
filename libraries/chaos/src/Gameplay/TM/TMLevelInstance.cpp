#include <chaos/Chaos.h>

namespace chaos
{
	// =====================================
	// TMTriggerCollisionInfo implementation
	// =====================================

	bool TMTriggerCollisionInfo::FindTrigger(TMTrigger const* trigger) const
	{
		return std::find_if(triggers.begin(), triggers.end(), [trigger](weak_ptr<TMTrigger> const& p) {return p.get() == trigger; }) != triggers.end();
	}

	// =====================================
	// TMLevelInstance implementation
	// =====================================

	TiledMap::Map* TMLevelInstance::GetTiledMap()
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

	TiledMap::Map const* TMLevelInstance::GetTiledMap() const
	{
		TMLevel const* level = GetLevel();
		if (level == nullptr)
			return nullptr;
		return level->GetTiledMap();
	}

	uint64_t TMLevelInstance::GetCollisionFlagByName(char const* name) const
	{
		assert(name != nullptr);

		if (StringTools::Stricmp(name, "PLAYER") == 0)
			return CollisionMask::PLAYER;
		if (StringTools::Stricmp(name, "CAMERA") == 0)
			return CollisionMask::CAMERA;

		return 0;
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

	TMTriggerCollisionInfo* TMLevelInstance::FindTriggerCollisionInfo(Object* object)
	{
		size_t count = collision_info.size();
		for (size_t i = 0; i < count ; ++i)
			if (collision_info[i].object == object)
				return &collision_info[i];
		return nullptr;
	}

	void TMLevelInstance::HandleTriggerCollisions(float delta_time, Object* object, box2 const& box, int mask)
	{
		TMTriggerCollisionInfo* previous_collisions = FindTriggerCollisionInfo(object);

		TMTriggerCollisionInfo new_collisions;

		// search all new collisions
		for (TMTriggerCollisionIterator it = GetTriggerCollisionIterator(box, mask, true) ; it ; ++it)
		{
			TMTrigger& trigger = *it;
			// trigger only enabled trigger
			if (!trigger.IsEnabled())
				continue;
			// trigger once : do not trigger anymore entering events
			if (trigger.IsTriggerOnce() && trigger.enter_event_triggered)
				continue;
			// collision type
			CollisionType collision_type = CollisionType::STARTED;
			if (previous_collisions != nullptr)
				if (previous_collisions->FindTrigger(&trigger))
					collision_type = CollisionType::AGAIN;
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
			CollisionType collision_type = CollisionType::STARTED;
			if (previous_collisions != nullptr)
				if (previous_collisions->FindTrigger(trigger))
					collision_type = CollisionType::AGAIN;

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
					previous_collisions->triggers[i]->OnCollisionEvent(delta_time, object, CollisionType::FINISHED);
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

			box2 pawn_box = player_pawn->GetBoundingBox();
			if (IsGeometryEmpty(pawn_box))
				continue;

			HandleTriggerCollisions(delta_time, player, pawn_box, CollisionMask::PLAYER);
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

			box2 camera_box = camera->GetCameraBox();
			if (IsGeometryEmpty(camera_box))
				continue;

			HandleTriggerCollisions(delta_time, camera, camera_box, CollisionMask::CAMERA);
		}
	}

	bool TMLevelInstance::DoTick(float delta_time)
	{
		LevelInstance::DoTick(delta_time);
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

	int TMLevelInstance::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
	{
		int result = 0;
		// draw the layer instances
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			result += layer_instances[i]->Display(renderer, uniform_provider, render_params);
		return result;
	}

	bool TMLevelInstance::Initialize(Game* in_game, Level* in_level)
	{
		if (!LevelInstance::Initialize(in_game, in_level))
			return false;

		TMObjectReferenceSolver reference_solver;
		// create a the layers instances		
		if (!CreateLayerInstances(in_game, reference_solver))
			return false;
		// initialize the level instance
		TMLevel* lvl = auto_cast(in_level);
		assert(lvl != nullptr);
		if (!InitializeLevelInstance(reference_solver, lvl->tiled_map.get()))
			return false;
		// solve the references
		reference_solver.SolveReferences(this);
		return true;
	}

	bool TMLevelInstance::InitializeLevelInstance(TMObjectReferenceSolver& reference_solver, TiledMap::PropertyOwner const * property_owner)
	{
		reference_solver.DeclareReference(player_start, "PLAYER_START", property_owner);
		reference_solver.DeclareReference(main_camera, "MAIN_CAMERA", property_owner);
		return true;
	}

	bool TMLevelInstance::DoCreateLayerInstances(std::vector<shared_ptr<TiledMap::LayerBase>> const & layers, TMObjectReferenceSolver& reference_solver)
	{
		TMLevel* level = GetLevel();

		for (auto& layer : layers)
		{
			// insert the new layer
			TMLayerInstance* layer_instance = level->CreateLayerInstance(this, layer.get(), nullptr, reference_solver);
			if (layer_instance != nullptr)
				layer_instances.push_back(layer_instance);
		}
		return true;
	}

	bool TMLevelInstance::CreateLayerInstances(Game* in_game, TMObjectReferenceSolver& reference_solver)
	{
		TiledMap::Map* tiled_map = GetTiledMap();
		if (tiled_map != nullptr)
			return DoCreateLayerInstances(tiled_map->layers, reference_solver);
		return true;
	}

	box2 TMLevelInstance::GetBoundingBox() const
	{
		// explicit bounding box
		if (has_explicit_bounding_box)
			return explicit_bounding_box;
		// depend on layers
		box2 result;
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			result = result | layer_instances[i]->GetBoundingBox(true, true); // expressed in world system the bounding boxes
		return result;
	}

	TMLayerInstance* TMLevelInstance::FindLayerInstanceByID(int in_id, bool recursive)
	{
		return TMTools::FindLayerInstanceByID(this, layer_instances, in_id, recursive);
	}

	TMLayerInstance const* TMLevelInstance::FindLayerInstanceByID(int in_id, bool recursive) const
	{
		return TMTools::FindLayerInstanceByID(this, layer_instances, in_id, recursive);
	}

	TMLayerInstance* TMLevelInstance::FindLayerInstance(ObjectRequest request, bool recursive)
	{
		for (auto& layer : layer_instances)
		{
			if (layer != nullptr)
			{
				if (request.Match(*layer.get()))
					return layer.get();
				if (recursive)
					if (TMLayerInstance * result = layer->FindLayerInstance(request, recursive))
						return result;
			}
		}
		return nullptr;
	}
	TMLayerInstance const* TMLevelInstance::FindLayerInstance(ObjectRequest request, bool recursive) const
	{
		for (auto& layer : layer_instances)
		{
			if (layer != nullptr)
			{
				if (request.Match(*layer.get()))
					return layer.get();
				if (recursive)
					if (TMLayerInstance const * result = layer->FindLayerInstance(request, recursive))
						return result;
			}
		}
		return nullptr;
	}

	void TMLevelInstance::CreateCameras()
	{
		TMLevel* level = GetLevel();

		// search the MAIN CAMERA TEMPLATE
		TMCameraTemplate* camera_template = main_camera.get();
		if (camera_template == nullptr)
		{
			camera_template = FindObject<TMCameraTemplate>(ObjectRequest::Any(), true); // try to find the very first one otherwise
			if (camera_template == nullptr)
				return;
		}

		// get the aspect ratio
		float aspect_ratio = 16.0f / 9.0f;

		Game const* game = GetGame();
		if (game != nullptr)
			aspect_ratio = game->GetViewportWantedAspect();

		// compute the surface
		box2 camera_box = AlterBoxToAspect(camera_template->GetBoundingBox(true), aspect_ratio, true);

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

		// XXX : If the player start is being created in a TiledLayer through the Factory system, it has no name
		//       It cannot be though searched by name
		//       we will find take the very first PlayerStart

		TMPlayerStart* result = player_start.get();
		if (result == nullptr)
			result = FindObject<TMPlayerStart>(ObjectRequest::Any(), true); // try to find the very first one otherwise
		return result;
	}

	PlayerPawn* TMLevelInstance::CreatePlayerPawnAtPlayerStart(Player* player, TMPlayerStart* player_start)	
	{
		// create a pawn 
		PlayerPawn* result = LevelInstance::CreatePlayerPawn(player);
		if (result == nullptr)
			return nullptr;

		// check the layer instance
		TMLayerInstance* layer_instance = player_start->GetLayerInstance();
		if (layer_instance == nullptr)
			return result;

		// create a particle populator
		TMParticlePopulator particle_populator;
		if (!particle_populator.Initialize(layer_instance))
			return result;

		// create the particle
		int player_gid = 0;
		int particle_flags = 0;
		bool keep_aspect_ratio = true;

		Hotpoint hotpoint = Hotpoint::BOTTOM_LEFT;






		// shuyyy


		// shu47 => j utilise nouvellement player_start->GetRotation().  Comme l'allocation n'existe pas encore, cela va prendre la donnée en provenance de GameEntity

		particle_populator.AddParticle(
			player_start->bitmap_name.c_str(), 
			hotpoint, 
			player_start->GetBoundingBox(true), 
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
			player_start->GetRotation(),
			particle_flags, 
			player_gid, 
			keep_aspect_ratio);
		particle_populator.FlushParticles();

		// get the allocation and finalize the layer
		ParticleAllocationBase* player_allocation = particle_populator.GetParticleAllocation();
		if (player_allocation == nullptr)
			return result;
		// shuxxx : first time FinalizeParticles(...) was called, there was no effect because the PlayerStartLayer has no particle. 
		//          call it twice as a fast fix
		layer_instance->FinalizeParticles(player_allocation);
		
		// give the allocation tp the pawn
		result->SetAllocation(player_allocation);










		// get data from particle to entity
		result->SynchronizeData(true);




		return result;
	}

	PlayerPawn * TMLevelInstance::CreatePlayerPawn(Player* player)
	{
		// find the player start to use for the player
		TMPlayerStart* player_start = GetPlayerStartForPawn(player);
		if (player_start == nullptr)
			return nullptr;

		// create the pawn
		PlayerPawn* result = CreatePlayerPawnAtPlayerStart(player, player_start);
		if (result == nullptr)
			return nullptr;

		// XXX : while camera, is restricted so we can see player, we considere that the displacement_ratio of the layer containing the player start is the reference one
		reference_layer = player_start->GetLayerInstance();

		return result;
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
		LevelInstance::SetInGameMusic();
	}

	void TMLevelInstance::OnLevelEnded()
	{
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			layer_instances[i]->OnLevelEnded();

		LevelInstance::OnLevelEnded();
	}

	void TMLevelInstance::OnLevelStarted()
	{
		LevelInstance::OnLevelStarted();

		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			layer_instances[i]->OnLevelStarted();
	}

	TMTileCollisionIterator TMLevelInstance::GetTileCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry)
	{
		return TMTileCollisionIterator(this, in_collision_mask, in_collision_box, in_open_geometry);
	}

	TMTriggerCollisionIterator TMLevelInstance::GetTriggerCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry)
	{
		return TMTriggerCollisionIterator(this, in_collision_mask, in_collision_box, in_open_geometry);
	}

	TMObjectCollisionIterator TMLevelInstance::GetObjectCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry)
	{
		return TMObjectCollisionIterator(this, in_collision_mask, in_collision_box, in_open_geometry);
	}

	bool TMLevelInstance::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!LevelInstance::SerializeFromJSON(json))
			return false;
		TMTools::SerializeLayersFromJSON(this, json);
		return true;
	}

	bool TMLevelInstance::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!LevelInstance::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "LAYERS", layer_instances);
		return true;
	}

}; // namespace chaos
