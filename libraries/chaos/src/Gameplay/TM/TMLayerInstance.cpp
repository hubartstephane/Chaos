#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ==========================================
	// PropertyOwnerOverride : an utility class to capture the properties of a source
	// ==========================================

	template<typename T>
	class PropertyOwnerOverride : public T
	{
	public:

		/** constructor */
		PropertyOwnerOverride(TiledMap::BaseObject* in_owner, TiledMap::PropertyOwner const* in_property_owner) :
			T(in_owner),
			property_owner(in_property_owner) {}

		/** override */
		virtual TiledMap::Property const* FindProperty(char const* name, TiledMap::PropertyType type_id) const override
		{
			TiledMap::Property const* result = nullptr;
			if (property_owner != nullptr)
				result = property_owner->FindProperty(name, type_id);
			if (result == nullptr)
				result = T::FindProperty(name, type_id);
			return result;
		}

	protected:

		/** a substitute property owner to fake the system */
		TiledMap::PropertyOwner const* property_owner = nullptr;
	};

	// =====================================
	// TMLayerInstance implementation
	// =====================================

	AutoCastable<Game> TMLayerInstance::GetGame()
	{
		return level_instance->GetGame();
	}

	AutoConstCastable<Game> TMLayerInstance::GetGame() const
	{
		return level_instance->GetGame();
	}

	AutoCastable<Level> TMLayerInstance::GetLevel()
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetLevel();
	}

	AutoConstCastable<Level> TMLayerInstance::GetLevel() const
	{
		if (level_instance == nullptr)
			return (Level*)nullptr;
		return level_instance->GetLevel();
	}

	AutoCastable<TMLevelInstance> TMLayerInstance::GetLevelInstance()
	{
		return level_instance;
	}

	AutoConstCastable<TMLevelInstance> TMLayerInstance::GetLevelInstance() const
	{
		return level_instance;
	}

	box2 TMLayerInstance::GetBoundingBox(bool world_system, bool recursive) const
	{
		box2 result;
		if (!infinite_bounding_box)
		{
			// get the layer own content bounding box
			result = content_bounding_box;

			// get child layers bounding box
			if (recursive)
				for (auto& layer : layer_instances)
					result = result | layer->GetBoundingBox(world_system, recursive);

			// apply our own offset that can have changed during game lifetime
			if (world_system)
				result.position += offset;
		}
		return result;
	}

	TMLayerInstance* TMLayerInstance::FindLayerInstanceByID(int in_id, bool recursive)
	{
		return TMTools::FindLayerInstanceByID(this, layer_instances, in_id, recursive);
	}

	TMLayerInstance const* TMLayerInstance::FindLayerInstanceByID(int in_id, bool recursive) const
	{
		return TMTools::FindLayerInstanceByID(this, layer_instances, in_id, recursive);
	}

	TMLayerInstance* TMLayerInstance::FindLayerInstance(ObjectRequest request, bool recursive)
	{
		return TMTools::FindLayerInstance(this, layer_instances, request, recursive);
	}
	TMLayerInstance const* TMLayerInstance::FindLayerInstance(ObjectRequest request, bool recursive) const
	{
		return TMTools::FindLayerInstance(this, layer_instances, request, recursive);
	}

	GPURenderMaterial* TMLayerInstance::FindOrCreateRenderMaterial(char const* material_name) const
	{
		if (material_name != nullptr && material_name[0] != 0) // unamed material ?
		{
			// get the resource manager
			GPUResourceManager* resource_manager = WindowApplication::GetGPUResourceManagerInstance();
			if (resource_manager != nullptr)
			{
				// search declared material
				GPURenderMaterial* result = resource_manager->FindRenderMaterial(material_name);
				if (result != nullptr)
					return result;
			}
		}
		// default material else where
		return DefaultParticleProgram::GetMaterial();
	}

	void TMLayerInstance::ComputeLayerCollisionMask(char const * mask)
	{
		assert(mask != nullptr);

		collision_mask = 0;

		std::vector<std::string> collision_names;
		NameFilter::AddNames(mask, collision_names);

		for (std::string const& name : collision_names)
			collision_mask |= level_instance->GetCollisionFlagByName(name.c_str());
	}

	bool TMLayerInstance::Initialize(TMLevelInstance* in_level_instance, TiledMap::LayerBase const * in_layer, TMLayerInstance* in_parent_layer, TMObjectReferenceSolver& reference_solver)
	{
		// ensure not already initialized
		assert(in_level_instance != nullptr);
		assert(in_layer != nullptr);
		assert(level_instance == nullptr);
		assert(layer == nullptr);

		level_instance = in_level_instance;
		layer = in_layer;
		parent_layer = in_parent_layer;

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
		content_bounding_box = box2();
		// special initialization
		if (TiledMap::ImageLayer const* image_layer = auto_cast(layer))
		{
			if (!InitializeImageLayer(image_layer, reference_solver))
				return false;
		}
		else if (TiledMap::ObjectLayer const* object_layer = auto_cast(layer))
		{
			if (!InitializeObjectLayer(object_layer, reference_solver))
				return false;
		}
		else if (TiledMap::TileLayer const* tile_layer = auto_cast(layer))
		{
			if (!InitializeTileLayer(tile_layer, reference_solver))
				return false;
		}
		else if (TiledMap::GroupLayer const* group_layer = auto_cast(layer))
		{
			if (!InitializeGroupLayer(group_layer, reference_solver))
				return false;
		}
		else
		{
			return false;
		}
		return FinalizeParticles(nullptr);
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

	bool TMLayerInstance::SerializeObjectListFromJSON(nlohmann::json const& json, char const * attribute_name, std::vector<shared_ptr<TMObject>> & result)
	{
		// in "Objects" array, read all objects, search the ID and apply the data to dedicated object
		nlohmann::json const* objects_json = JSONTools::GetStructure(json, attribute_name);
		if (objects_json != nullptr && objects_json->is_array())
		{
			for (size_t i = 0; i < objects_json->size(); ++i)
			{
				nlohmann::json const* object_json = JSONTools::GetStructureByIndex(*objects_json, i);
				if (object_json != nullptr && object_json->is_object())
				{
					int object_id = 0;
					if (JSONTools::GetAttribute(*object_json, "OBJECT_ID", object_id))
					{
						TMTrigger* trigger = FindObjectByID<TMTrigger>(object_id);
						if (trigger != nullptr)
						{
							LoadFromJSON(*object_json, *trigger); // XXX : the indirection is important to avoid the creation of a new layer_instance
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
		if (!JSONSerializable::SerializeFromJSON(json))
			return false;
		SerializeObjectListFromJSON(json, "OBJECTS", objects);
		TMTools::SerializeLayersFromJSON(this, json);
		return true;
	}

	bool TMLayerInstance::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!JSONSerializable::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "LAYER_ID", GetLayerID());
		JSONTools::SetAttribute(json, "OBJECTS", objects);
		JSONTools::SetAttribute(json, "LAYERS", layer_instances);
		return true;
	}

	bool TMLayerInstance::InitializeImageLayer(TiledMap::ImageLayer const * image_layer, TMObjectReferenceSolver& reference_solver)
	{
		return true;
	}

	void TMLayerInstance::CreateObjectParticles(TiledMap::GeometricObject const* in_geometric_object, TMObject* object, TMParticlePopulator& particle_populator)
	{
		TiledMap::Map* tiled_map = level_instance->GetTiledMap();

		// does the object wants the ownership of the particles
		bool particle_ownership = false;
		if (object != nullptr)
			particle_ownership = in_geometric_object->GetPropertyValueBool("PARTICLE_OWNERSHIP", true); // by default, an object wants to have its particles

		// create additionnal particles (TEXT)
		if (TiledMap::GeometricObjectText const* text = auto_cast(in_geometric_object))
		{
			// create particle layer if necessary
			if (CreateParticleLayer() == nullptr) // the generate layer is of  TMParticleLayerTrait => this works fine for Text too (except some useless extra data like GID)
				return;

			Game* game = GetGame();

			ParticleTextGenerator::GeneratorResult result;
			ParticleTextGenerator::GeneratorParams params;

			params.line_height = (float)text->pixelsize;
			params.hotpoint = Hotpoint::TOP;
			params.position = text->position;
			params.default_color = text->color;

			std::string const* font_name = text->FindPropertyString("FONT_NAME");
			if (font_name != nullptr)
				params.font_info_name = *font_name;

			// create particles
			WindowApplication* window_application = Application::GetInstance();
			if (window_application != nullptr)
				window_application->GetTextGenerator()->Generate(text->text.c_str(), result, params);

			ParticleAllocationBase* allocation = ParticleTextGenerator::CreateTextAllocation(particle_layer.get(), result);
			if (particle_ownership)
				object->allocations = allocation;
		}
		// create additionnal particles (TILES)
		else if (TiledMap::GeometricObjectTile const* tile = auto_cast(in_geometric_object))
		{
			TMParticlePopulator object_particle_populator = particle_populator;

			TMParticlePopulator* effective_particle_populator = (particle_ownership) ? &object_particle_populator : &particle_populator;

			int gid = tile->gid;

			// search the tile information
			TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
			if (tile_info.tiledata == nullptr)
				return;

			// create a simple particle
			box2 particle_box = tile->GetBoundingBox(true);
			if (object != nullptr)
			{
				TiledMap::GeometricObjectSurface const* surface_object = auto_cast(in_geometric_object);
				if (surface_object != nullptr)
					particle_box = surface_object->GetBoundingBox(false); // shuxxx : the TILE is generated on the same layer then the surface. does it get the layer_offset ????
			}

			Hotpoint hotpoint = (tile_info.tileset != nullptr) ? tile_info.tileset->object_alignment : Hotpoint::BOTTOM_LEFT;

			bool keep_aspect_ratio = false;
			effective_particle_populator->AddParticle(
				tile_info.tiledata->atlas_key.c_str(),
				hotpoint,
				particle_box,
				glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
				tile->rotation,
				tile->particle_flags,
				gid,
				keep_aspect_ratio);

			// gives the particles to the object
			if (particle_ownership)
			{
				effective_particle_populator->FlushParticles();
				object->allocations = effective_particle_populator->GetParticleAllocation();

				// whether the allocation wants to kwow about the TMObject
				GameEntity const ** allocation_data = object->allocations->GetOwnedData<GameEntity const *>();
				if (allocation_data != nullptr)
					*allocation_data = object;
			}
		}
	}

	namespace Arguments
	{
#if _DEBUG
		CHAOS_APPLICATION_ARG(bool, TMForceParticleCreation);
#endif
	};

	bool TMLayerInstance::ShouldCreateParticleForObject(TiledMap::PropertyOwner const * property_owner, TMObject* object) const
	{
#if _DEBUG
		if (Arguments::TMForceParticleCreation.Get())
			return true;
#endif
		return property_owner->GetPropertyValueBool("PARTICLE_CREATION", (object != nullptr) ? object->IsParticleCreationEnabled() : true);
	}

	TMObjectFactory TMLayerInstance::GetObjectFactory(TiledMap::TypedObject const * in_typed_object)
	{
		TMLevel* level = GetLevel();

		// get a factory for the object (new + Initialize(...) ...)
		TMObjectFactory factory = level->GetObjectFactory(this, in_typed_object);
		if (!factory)
			return nullptr;
		// create a 'final' factory that use previous one + insert the result object in correct list
		TMObjectFactory result = [this, factory](TiledMap::GeometricObject const * geometric_object, TMObjectReferenceSolver & in_reference_solver)
		{
			TMObject* result = factory(geometric_object, in_reference_solver);
			if (result != nullptr)
				objects.push_back(result);
			return result;
		};
		return result;
	}

	bool TMLayerInstance::InitializeObjectLayer(TiledMap::ObjectLayer const * object_layer, TMObjectReferenceSolver & reference_solver)
	{
		// search the bounding box (explicit or not)
		box2 explicit_bounding_box;

		// create the layer
		if (CreateParticleLayer() == nullptr)
			return false;
		// the particle generator
		TMParticlePopulator particle_populator; // this populator is for the WHOLE LAYER
		if (!particle_populator.Initialize(this))
			return false;

		// iterate over all objects
		size_t count = object_layer->geometric_objects.size();
		for (size_t i = 0; i < count; ++i)
		{
			TiledMap::GeometricObject const * geometric_object = object_layer->geometric_objects[i].get();
			if (geometric_object == nullptr)
				continue;

			// explicit world bounding box
			if (!level_instance->has_explicit_bounding_box && TMTools::IsWorldBoundingBox(geometric_object))
			{
				TiledMap::GeometricObjectSurface const* object_surface = auto_cast(geometric_object);
				if (object_surface != nullptr)
				{
					level_instance->explicit_bounding_box = object_surface->GetBoundingBox(true); // in world coordinate
					level_instance->has_explicit_bounding_box = true;
				}
			}
			// explicit layer bounding box
			if (IsGeometryEmpty(explicit_bounding_box) && TMTools::IsLayerBoundingBox(geometric_object))
			{
				TiledMap::GeometricObjectSurface const* object_surface = auto_cast(geometric_object);
				if (object_surface != nullptr)
					explicit_bounding_box = object_surface->GetBoundingBox(false); // in layer coordinates
			}

			// get factory + create the object
			TMObject* object = nullptr;

			TMObjectFactory factory = GetObjectFactory(geometric_object);
			if (factory)
			{
				object = factory(geometric_object, reference_solver);
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
			content_bounding_box = explicit_bounding_box;
		else
			content_bounding_box = particle_populator.GetBoundingBox();

		return true;
	}

	bool TMLayerInstance::FinalizeParticles(ParticleAllocationBase* allocation)
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

	bool TMLayerInstance::InitializeParticleLayer(ParticleLayerBase* in_particle_layer)
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
			in_particle_layer->SetTag(DeclareTag(renderable_tag->c_str()));
		else
		{
			int const* layer_tag = layer->FindPropertyInt("RENDERABLE_TAG");
			if (layer_tag != nullptr)
				in_particle_layer->SetTag((TagType) * layer_tag);
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

	SpawnParticleResult TMLayerInstance::SpawnParticles(size_t count, bool new_allocation)
	{
		// create particle layer if necessary
		if (CreateParticleLayer() == nullptr)
			return nullptr;
		// create the allocation
		return particle_layer->SpawnParticles(count, new_allocation);
	}

	// shu49 mieux a faire qu une fonction qui va recreer un material a chaque fois non ?


	GPURenderMaterial* TMLayerInstance::GetRenderMaterial() const
	{
		return FindOrCreateRenderMaterial(material_name.c_str());
	}

	ParticleLayerBase* TMLayerInstance::CreateParticleLayer()
	{

		// shu





		if (particle_layer == nullptr)
		{
			// get the application for atlas
			WindowApplication* window_application = Application::GetInstance();
			if (window_application == nullptr)
				return nullptr;
			// find render material
			GPURenderMaterial* render_material = GetRenderMaterial();
			if (render_material == nullptr)
				return nullptr;
			// create a particle layer (it is not attached to any particle manager !)
			TMLevel* level = GetLevel();
			particle_layer = level->DoCreateParticleLayer(this);
			if (particle_layer == nullptr)
				return nullptr;
			// add name and tag to the particle_layer
			InitializeParticleLayer(particle_layer.get());
			// set the material
			particle_layer->SetRenderMaterial(render_material);
			// set the atlas
			particle_layer->SetTextureAtlas(window_application->GetTextureAtlas());
			// set some flags for the layer
			ParticleLayerTraitBase* layer_trait = particle_layer->GetLayerTrait();
			if (layer_trait != nullptr && layer != nullptr)
			{
				layer_trait->dynamic_particles = layer->GetPropertyValueBool("DYNAMIC_PARTICLES", layer_trait->dynamic_particles);
				layer_trait->dynamic_vertices  = layer->GetPropertyValueBool("DYNAMIC_VERTICES", layer_trait->dynamic_vertices);
			}
		}
		return particle_layer.get();
	}

	bool TMLayerInstance::InitializeTileLayer(TiledMap::TileLayer const * tile_layer, TMObjectReferenceSolver& reference_solver)
	{
		TMLevel* level = GetLevel();

		// create particle layer
		if (CreateParticleLayer() == nullptr)
			return false;
		// prepare the populator
		TMParticlePopulator particle_populator;
		if (!particle_populator.Initialize(this))
			return false;

		// populate the layer for each chunk
		TiledMap::Map* tiled_map = level_instance->GetTiledMap();

		bool particle_creation_success = true; // as soon as some particle creation fails, do not try to create other particles

		for (TiledMap::TileLayerChunk const& chunk : tile_layer->tile_chunks)
		{
			size_t count = chunk.tile_indices.size();
			for (size_t i = 0; i < count; ++i)
			{
				int gid = chunk.tile_indices[i].gid;
				int particle_flags = chunk.tile_indices[i].flags;



				// shu48 recuperer le GID pour le spawn de particle



				if (gid == 0)
					continue;

				// search the tile information
				TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
				if (tile_info.tiledata == nullptr)
					continue;

				// prepare data for the tile/object
				glm::ivec2  tile_coord = tile_layer->GetTileCoordinate(chunk, i);




				// shu48
				//
				// a ce niveau, une particle ne sait pas ou elle est sur la grille












				box2 particle_box = tile_layer->GetTileBoundingBox(tile_coord, tile_info.tiledata->image_size, particle_flags, false);

				// try to create a geometric object from the tile
				TMObjectFactory factory = GetObjectFactory(tile_info.tiledata);
				if (factory)
				{
					// to avoid the creation of a TMObject, use a wrapper on the properties
					PropertyOwnerOverride<TiledMap::GeometricObjectTile> tile_object = { nullptr, tile_info.tiledata };

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
					int mask = ~((unsigned int)-1 << per_component_bit_count);
					int idx = tile_coord.x & mask;
					int idy = tile_coord.y & mask;
					int object_id = -1 * (idx | (idy << per_component_bit_count));

					tile_object.id = object_id;
					tile_object.gid = gid;
					tile_object.type = tile_info.tiledata->type;
					tile_object.size = particle_box.half_size * 2.0f;


					// shuyyy : should depend on the pivot ???


					tile_object.position.x = particle_box.position.x - particle_box.half_size.x;
					tile_object.position.y = particle_box.position.y - particle_box.half_size.y;

					// XXX : for player start : but this is not a great idea to process by exception
					//       We are writing int the fake object properties, not in the 'tile_info.tiledata'
					//       That means that if 'tile_info.tiledata' already has a BITMAP_NAME property, this does not
					//       interfere with that (a just in case value)
					tile_object.CreatePropertyString("BITMAP_NAME", tile_info.tiledata->atlas_key.c_str());

					TMObject* object = factory(&tile_object, reference_solver);
					if (object != nullptr)
						if (ShouldCreateParticleForObject(&tile_object, object))
							CreateObjectParticles(&tile_object, object, particle_populator);


					continue; // while we have a factory, let the concerned object create its particle
				}

				Hotpoint hotpoint = Hotpoint::BOTTOM_LEFT;

				// create a simple particle => as soon as there is an error, stop trying producing particles
				bool keep_aspect_ratio = true;

				if (particle_creation_success)
				{
					particle_creation_success = particle_populator.AddParticle(
						tile_info.tiledata->atlas_key.c_str(),
						hotpoint, particle_box,
						glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
						0.0f,
						particle_flags,
						gid,
						keep_aspect_ratio);
				}
			}
		}

		// final flush
		if (particle_creation_success)
			particle_populator.FlushParticles();
		// update the bounding box
		content_bounding_box = particle_populator.GetBoundingBox();

		return true;
	}

	// shulayer
	bool TMLayerInstance::InitializeGroupLayer(TiledMap::GroupLayer const* group_layer, TMObjectReferenceSolver& reference_solver)
	{
		TMLevel* level = GetLevel();
		// iterate over child layers
		for (auto& layer : group_layer->layers)
		{
			TMLayerInstance* layer_instance = level->CreateLayerInstance(level_instance, layer.get(), this, reference_solver);
			if (layer_instance != nullptr)
				layer_instances.push_back(layer_instance);
		}
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
		// tick the child layers
		for (auto& layer : layer_instances)
			if (layer != nullptr)
				layer->Tick(delta_time);
		return true;
	}

	int TMLayerInstance::DoDisplay(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
		// display this layer particles
		int result = 0;
		if (particle_layer != nullptr)
		{
			// camera is expressed in world, so is for layer
			obox2 camera_obox = GetLevelInstance()->GetCameraOBox(0);
			obox2 initial_camera_obox = GetLevelInstance()->GetInitialCameraOBox(0);

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

			obox2 final_camera_obox;
			final_camera_obox.position = initial_camera_obox.position + (camera_obox.position - initial_camera_obox.position) * final_ratio;
			final_camera_obox.half_size = initial_camera_obox.half_size + (camera_obox.half_size - initial_camera_obox.half_size) * final_ratio;



			// shu48

			// default block to render
			glm::ivec2 start_instance = glm::ivec2(0, 0);
			glm::ivec2 last_instance  = glm::ivec2(1, 1);

			RepeatedBoxScissor scissor;
			if (!infinite_bounding_box)
			{
				// compute repetitions
				bool recursive = true;
				box2 layer_box = GetBoundingBox(true, recursive);
				scissor = RepeatedBoxScissor(layer_box, chaos::GetBoundingBox(final_camera_obox), wrap_x, wrap_y);

				// HACK : due to bad LAYER_BOUNDING_BOX computation, the layer containing PLAYER_START may be clamped and layer hidden
				start_instance = scissor.start_instance;
				last_instance = scissor.last_instance;
				if (this == reference_layer || IsGeometryEmpty(layer_box))
				{
					start_instance = glm::ivec2(0, 0);
					last_instance = glm::ivec2(1, 1); // always see fully the layer without clamp => repetition not working
				}
			}

			// new provider for camera override (will be fullfill only if necessary)
			GPUProgramProviderChain main_uniform_provider(uniform_provider);
			main_uniform_provider.AddVariable("world_to_camera", CameraTools::GetCameraTransform(final_camera_obox));

			box2 final_camera_box;
			final_camera_box.position = final_camera_obox.position;
			final_camera_box.half_size = final_camera_obox.half_size;
			main_uniform_provider.AddVariable("projection_matrix", CameraTools::GetProjectionMatrix(final_camera_obox));

			glm::mat4 local_to_world = glm::translate(glm::vec3(offset.x, offset.y, 0.0f));

			// draw instances
			for (int x = start_instance.x; x < last_instance.x; ++x)
			{
				for (int y = start_instance.y; y < last_instance.y; ++y)
				{
					// new Provider to apply the offset for this 'instance'
					GPUProgramProviderChain instance_uniform_provider(&main_uniform_provider);
					glm::vec2 instance_offset = (!infinite_bounding_box) ? scissor.GetInstanceOffset(glm::ivec2(x, y)) : glm::vec2(0.0f, 0.0f);

					local_to_world[3][0] = instance_offset.x + offset.x;
					local_to_world[3][1] = instance_offset.y + offset.y;
					instance_uniform_provider.AddVariable("local_to_world", local_to_world);

					// draw call
					result += particle_layer->Display(renderer, &instance_uniform_provider, render_params);
				}
			}
		}

		// draw child layers
		for (auto& layer : layer_instances)
			if (layer != nullptr)
				result += layer->Display(renderer, uniform_provider, render_params);




		// shu 49 rendre les objets ??
		for (auto& object : objects)
			if (object != nullptr)
				result += object->Display(renderer, uniform_provider, render_params);






		return result;
	}

	size_t TMLayerInstance::GetObjectCount() const
	{
		return objects.size();
	}

	AutoCastable<TMObject> TMLayerInstance::GetObject(size_t index)
	{
		if (index >= objects.size())
			return nullptr;
		return objects[index].get();
	}

	AutoConstCastable<TMObject> TMLayerInstance::GetObject(size_t index) const
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
		// child layers
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			layer_instances[i]->OnLevelEnded();
	}

	void TMLayerInstance::OnLevelStarted()
	{
		//  objects
		size_t object_count = objects.size();
		for (size_t i = 0; i < object_count; ++i)
			objects[i]->OnLevelStarted();
		// child layers
		size_t count = layer_instances.size();
		for (size_t i = 0; i < count; ++i)
			layer_instances[i]->OnLevelStarted();
	}

}; // namespace chaos
