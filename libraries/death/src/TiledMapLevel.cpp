#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/LayerInstanceParticlePopulator.h>

#include <chaos/CollisionFramework.h>
#include <chaos/TiledMapTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GPUProgramGenerator.h>

namespace death
{
	namespace TiledMap
	{
		// =====================================
		// BoxScissoringWithRepetitionResult implementation
		// =====================================

		BoxScissoringWithRepetitionResult::BoxScissoringWithRepetitionResult(chaos::box2 const & in_target_box, chaos::box2 const & in_scissor_box, bool in_wrap_x, bool in_wrap_y)
		{
			// copy the parameters
			target_box  = in_target_box;
			scissor_box = in_scissor_box;
			wrap_x = in_wrap_x;
			wrap_y = in_wrap_y;

			// First,
			//   Considere that there is always wrap_x = wrap_y = true
			//   Search along X and Y, the instance index that must collides with the scissor box
			// Then,
			//   whenever there are no wrap, we can skip the whole rendering if the instancee (0, 0) is not in the visible range

			glm::vec2 target_bottomleft = in_target_box.GetCorners().first;
			glm::vec2 scissor_bottomleft = in_scissor_box.GetCorners().first;

			glm::vec2 target_size = 2.0f * in_target_box.half_size;
			glm::vec2 scissor_size = 2.0f * in_scissor_box.half_size;

			// number of time to decal the layer box, to be directly left of the scissor box
			glm::ivec2 offset_count = glm::ivec2(
				(int)chaos::MathTools::Ceil((scissor_bottomleft.x - target_bottomleft.x - target_size.x) / target_size.x),
				(int)chaos::MathTools::Ceil((scissor_bottomleft.y - target_bottomleft.y - target_size.y) / target_size.y)
			);

			// the bottomleft corner of the decaled box
			glm::vec2  virtual_target_bottomleft = target_bottomleft + chaos::GLMTools::RecastVector<glm::vec2>(offset_count) * target_size;
			// competition of the number of repetition
			glm::vec2  tmp = ((scissor_bottomleft + scissor_size - virtual_target_bottomleft) / target_size);

			glm::ivec2 repetition_count = glm::ivec2(
				(int)chaos::MathTools::Ceil(tmp.x),
				(int)chaos::MathTools::Ceil(tmp.y)
			);
			// test for unwrapped case X if no collision at all (the instance(0) is not in the visible range)
			if (!wrap_x)
			{
				if (0 < offset_count.x || 0 >= offset_count.x + repetition_count.x)
				{
					start_instance = last_instance = glm::ivec2(0, 0); // nothing to render at all
					return;
				}
				offset_count.x = 0;
				repetition_count.x = 1;
			}

			// test for unwrapped case X if no collision at all (the instance(0) is not in the visible range)
			if (!wrap_y)
			{
				if (0 < offset_count.y || 0 >= offset_count.y + repetition_count.y)
				{
					start_instance = last_instance = glm::ivec2(0, 0); // nothing to render at all
					return;
				}
				offset_count.y = 0;
				repetition_count.y = 1;
			}

			// finalize the initialization
			start_instance = offset_count;
			last_instance = offset_count + repetition_count;
		}

		glm::vec2 BoxScissoringWithRepetitionResult::GetInstanceOffset(glm::ivec2 const & index) const
		{
			return 2.0f * target_box.half_size * chaos::GLMTools::RecastVector<glm::vec2>(index);
		}

		// =====================================
		// Level implementation
		// =====================================

		Level::Level()
		{

		}

		bool Level::Initialize(chaos::TiledMap::Map * in_tiled_map)
		{
			assert(in_tiled_map != nullptr);
			assert(tiled_map == nullptr);
			tiled_map = in_tiled_map;
			return true;
		}

		GameLevelInstance * Level::DoCreateLevelInstance(Game * in_game)
		{
			return new LevelInstance;
		}

		chaos::BitmapAtlas::TextureArrayAtlas const * Level::GetTextureAtlas(LayerInstance * layer_instance) const
		{
			Game const * game = layer_instance->GetGame();
			if (game == nullptr)
				return nullptr;
			return game->GetTextureAtlas();
		}

		chaos::BitmapAtlas::FolderInfo const * Level::GetFolderInfo(LayerInstance * layer_instance) const
		{
			// get the atlas
			chaos::BitmapAtlas::TextureArrayAtlas const * texture_atlas = GetTextureAtlas(layer_instance);
			if (texture_atlas == nullptr)
				return nullptr;
			// get the folder containing the sprites
			return texture_atlas->GetFolderInfo("sprites");
		}

		chaos::ParticleLayer * Level::CreateParticleLayer(LayerInstance * layer_instance)
		{
			auto * trait = new chaos::TypedParticleLayerDesc<TileParticleTrait>;
			if (trait == nullptr)
				return nullptr;
			return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<TileParticleTrait>);
		}

#define DEATH_CREATE_OBJECT(result_type, func_name, declared_parameters, constructor_parameters)\
		result_type * Level::Do##func_name(declared_parameters)\
		{\
			return new result_type(constructor_parameters);\
		}\
		result_type * Level::func_name(declared_parameters)\
		{\
			result_type * result = Do##func_name(constructor_parameters);\
			if (result == nullptr)\
				return nullptr;\
			if (!result->Initialize())\
			{\
				delete result;\
				return nullptr;\
			}\
			return result;\
		}

		DEATH_CREATE_OBJECT(GeometricObject, CreateTypedObject, LayerInstance * in_layer_instance BOOST_PP_COMMA() chaos::TiledMap::GeometricObject * in_geometric_object, in_layer_instance BOOST_PP_COMMA() in_geometric_object);
		DEATH_CREATE_OBJECT(TriggerSurfaceObject, CreateTriggerSurface, LayerInstance * in_layer_instance BOOST_PP_COMMA() chaos::TiledMap::GeometricObject * in_geometric_object, in_layer_instance BOOST_PP_COMMA() in_geometric_object);
		DEATH_CREATE_OBJECT(CameraObject, CreateCamera, LayerInstance * in_layer_instance BOOST_PP_COMMA() chaos::TiledMap::GeometricObject * in_geometric_object, in_layer_instance BOOST_PP_COMMA() in_geometric_object);
		DEATH_CREATE_OBJECT(PlayerStartObject, CreatePlayerStart, LayerInstance * in_layer_instance BOOST_PP_COMMA() chaos::TiledMap::GeometricObject * in_geometric_object, in_layer_instance BOOST_PP_COMMA() in_geometric_object);
		DEATH_CREATE_OBJECT(LayerInstance, CreateLayerInstance, LevelInstance * in_level_instance BOOST_PP_COMMA() chaos::TiledMap::LayerBase * in_layer, in_level_instance BOOST_PP_COMMA() in_layer);

#undef DEATH_CREATE_OBJECT
#undef DEATH_DOCREATE_OBJECT

		chaos::GPUProgram * Level::GenDefaultRenderProgram()
		{
			char const * vertex_shader_source = R"SHADERCODE(
			in vec2 position;
			in vec3 texcoord;
			in vec4 color;

			out vec2 vs_position;
			out vec3 vs_texcoord;
			out vec4 vs_color;

			uniform vec4 camera_box;
			uniform vec2 offset;

			void main() 
			{
				vs_position = position + offset;
				vs_texcoord = texcoord;
				vs_color    = color;

				gl_Position.xy = (position + offset - camera_box.xy) / camera_box.zw;
				gl_Position.z  = 0.0;
				gl_Position.w  = 1.0;
			}										
		)SHADERCODE";

			char const * pixel_shader_source = R"SHADERCODE(
			out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

			in vec2 vs_position;
			in vec3 vs_texcoord;
			in vec4 vs_color;

			uniform sampler2DArray material;

			void main()
			{
				vec4 color = texture(material, vs_texcoord);
				output_color.xyz = color.xyz * vs_color.xyz;
				output_color.a   = vs_color.a * color.a;
			};
		)SHADERCODE";

			chaos::GPUProgramGenerator program_generator;
			program_generator.AddShaderSource(GL_VERTEX_SHADER, vertex_shader_source);
			program_generator.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);
			return program_generator.GenProgramObject();
		}

		chaos::GPURenderMaterial * Level::GetDefaultRenderMaterial()
		{
			chaos::shared_ptr<chaos::GPUProgram> program = GenDefaultRenderProgram(); // store a temporary object for lifetime management
			return chaos::GPURenderMaterial::GenRenderMaterialObject(program.get());
		}

		bool Level::OnPlayerTileCollision(double delta_time, chaos::ParticleDefault::Particle * player_particle, TileParticle * particle)
		{

			return true; // continue with other
		}

		// =====================================
		// BaseObject implementation
		// =====================================

		BaseObject::BaseObject(class LayerInstance * in_layer_instance):
			layer_instance(in_layer_instance)
		{
			assert(in_layer_instance != nullptr);
		}

		// =====================================
		// GeometricObject implementation
		// =====================================

		GeometricObject::GeometricObject(class LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) :
			BaseObject(in_layer_instance),
			geometric_object(in_geometric_object)

		{
			assert(in_geometric_object != nullptr);
		}

		bool GeometricObject::Initialize()
		{
			name = geometric_object->name;
			return true;
		}
			

		// =====================================
		// TriggerSurfaceObject implementation
		// =====================================

		TriggerSurfaceObject::TriggerSurfaceObject(class LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) :
			GeometricObject(in_layer_instance, in_geometric_object)
		{
		}

		bool TriggerSurfaceObject::Initialize()
		{
			if (!GeometricObject::Initialize())
				return false;

			enabled    = geometric_object->FindPropertyBool("ENABLED", true);
			trigger_id = geometric_object->FindPropertyInt("TRIGGER_ID", 0);
		
			return true;
		}

		chaos::box2 TriggerSurfaceObject::GetBoundingBox(bool world_system) const
		{
			chaos::TiledMap::GeometricObjectSurface * surface = geometric_object->GetObjectSurface();
			if (surface == nullptr)
				return chaos::box2();
			chaos::box2 result = surface->GetBoundingBox(false);  // make our own correction for world system because the LayerInstance can change its offset
			if (world_system)
				result.position += layer_instance->GetLayerOffset();
			return result;
		}

		bool TriggerSurfaceObject::OnPlayerCollision(double delta_time, chaos::ParticleDefault::Particle * player_particle)
		{
			return true; // continue other collisions
		}

		// =====================================
		// PlayerStartObject implementation
		// =====================================

		PlayerStartObject::PlayerStartObject(class LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) :
			GeometricObject(in_layer_instance, in_geometric_object)
		{
		}

		bool PlayerStartObject::Initialize()
		{
			if (!GeometricObject::Initialize())
				return false;
			return true;
		}

		// =====================================
		// CameraObject implementation
		// =====================================

		CameraObject::CameraObject(class LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) :
			GeometricObject(in_layer_instance, in_geometric_object)
		{
		}

		bool CameraObject::Initialize()
		{
			if (!GeometricObject::Initialize())
				return false;
			return true;
		}

		// =====================================
		// LayerInstance implementation
		// =====================================

		LayerInstance::LayerInstance(LevelInstance * in_level_instance, chaos::TiledMap::LayerBase * in_layer):
			level_instance(in_level_instance),
			layer(in_layer)
		{
			assert(in_level_instance != nullptr);
			assert(in_layer != nullptr);
		}

		Game * LayerInstance::GetGame() 
		{ 
			return level_instance->GetGame(); 
		}

		Game const * LayerInstance::GetGame() const 
		{ 
			return level_instance->GetGame(); 
		}

		Level * LayerInstance::GetTypedLevel()
		{
			if (level_instance == nullptr)
				return nullptr;
			return level_instance->GetTypedLevel();
		}

		Level const * LayerInstance::GetTypedLevel() const
		{
			if (level_instance == nullptr)
				return nullptr;
			return level_instance->GetTypedLevel();
		}

		LevelInstance * LayerInstance::GetTypedLevelInstance()
		{
			return level_instance;
		}

		LevelInstance const * LayerInstance::GetTypedLevelInstance() const
		{
			return level_instance;
		}

		chaos::box2 LayerInstance::GetBoundingBox(bool world_system) const
		{
			chaos::box2 result = bounding_box; // apply our own offset that can have changed during game lifetime
			if (world_system)
				result.position += offset;
			return result;
		}

		chaos::GPURenderMaterial * LayerInstance::FindRenderMaterial(char const * material_name)
		{
			if (material_name != nullptr && material_name[0] != 0) // unamed material
			{
				// get the resource manager
				chaos::GPUResourceManager * resource_manager = chaos::MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
				if (resource_manager != nullptr)
				{
					// search declared material
					chaos::GPURenderMaterial * result = resource_manager->FindRenderMaterial(material_name);
					if (result != nullptr)
						return result;
				}
			}
			// default material else where
			return level_instance->GetDefaultRenderMaterial();
		}

		void LayerInstance::CheckInfiniteBoundingBox()
		{
			bool infinite_bounding_box = layer->FindPropertyBool("INFINITE_BOUNDING_BOX", false);
			if (infinite_bounding_box)
				bounding_box = chaos::box2();
		}

		bool LayerInstance::Initialize()
		{
			// get the properties of interrest
			float const * ratio = layer->FindPropertyFloat("DISPLACEMENT_RATIO");
			if (ratio != nullptr)
			{
				displacement_ratio = glm::vec2(*ratio, *ratio);
			}
			else
			{
				displacement_ratio.x = layer->FindPropertyFloat("DISPLACEMENT_RATIO_X", 1.0f);
				displacement_ratio.y = layer->FindPropertyFloat("DISPLACEMENT_RATIO_Y", 1.0f);
			}
			wrap_x = layer->FindPropertyBool("WRAP_X", false);
			wrap_y = layer->FindPropertyBool("WRAP_Y", false);
			material_name = layer->FindPropertyString("MATERIAL", "");

			trigger_surfaces_enabled = layer->FindPropertyBool("TRIGGER_SURFACES_ENABLED", false);
			player_collision_enabled = layer->FindPropertyBool("PLAYER_COLLISIONS_ENABLED", false);
			tile_collisions_enabled  = layer->FindPropertyBool("TILE_COLLISIONS_ENABLED", false);

			// copy the offset / name
			offset = layer->offset;
			name = layer->name;

			// reset the bounding box
			bounding_box = chaos::box2();
			// special initialization
			chaos::TiledMap::ImageLayer * image_layer = layer->GetImageLayer();
			if (image_layer != nullptr)
			{
				if (!InitializeLayer(image_layer))
					return false;
				CheckInfiniteBoundingBox();
				return FinalizeParticles();
			}

			chaos::TiledMap::ObjectLayer * object_layer = layer->GetObjectLayer();
			if (object_layer != nullptr)
			{
				if (!InitializeLayer(object_layer))
					return false;
				CheckInfiniteBoundingBox();
				return FinalizeParticles();
			}

			chaos::TiledMap::TileLayer * tile_layer = layer->GetTileLayer();
			if (tile_layer != nullptr)
			{
				if (!InitializeLayer(tile_layer))
					return false;
				CheckInfiniteBoundingBox();
				return FinalizeParticles();
			}

			return false;
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::ImageLayer * image_layer)
		{

			return true;
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::ObjectLayer * object_layer)
		{
			Level * level = GetTypedLevel();

			// search the bounding box (explicit or not)
			chaos::box2 box;
			chaos::box2 explicit_bounding_box;

			// the particle generator
			LayerInstanceParticlePopulator particle_populator;
			if (!particle_populator.Initialize(this))
				return false;

			// get the tiled map
			chaos::TiledMap::Map * tiled_map = level_instance->GetTiledMap();

			// iterate over all objects
			size_t count = object_layer->geometric_objects.size();
			for (size_t i = 0; i < count; ++i)
			{
				chaos::TiledMap::GeometricObject * geometric_object = object_layer->geometric_objects[i].get();
				if (geometric_object == nullptr)
					continue;
				
				// explicit world bounding box
				if (!level_instance->has_explicit_bounding_box && chaos::TiledMapTools::IsWorldBoundingBox(geometric_object))
				{
					 chaos::TiledMapTools::GetExplicitWorldBoundingBox(geometric_object, level_instance->explicit_bounding_box, true); // in world coordinate				
					 level_instance->has_explicit_bounding_box = true;
				}
				// explicit layer bounding box
				if (explicit_bounding_box.IsEmpty() && chaos::TiledMapTools::IsLayerBoundingBox(geometric_object))
				{
					chaos::TiledMapTools::GetExplicitLayerBoundingBox(geometric_object, explicit_bounding_box, false); // in layer coordinates				
				}

				// player start ?
				if (chaos::TiledMapTools::IsPlayerStart(geometric_object))
				{
					PlayerStartObject * player_start = level->CreatePlayerStart(this, geometric_object);
					if (player_start != nullptr)
						player_starts.push_back(player_start);
				}

				// camera ?
				if (chaos::TiledMapTools::IsCamera(geometric_object))
				{
					CameraObject * camera = level->CreateCamera(this, geometric_object);
					if (camera != nullptr)
						cameras.push_back(camera);
				}

				// object of interests
				if (chaos::TiledMapTools::IsTypedObject(geometric_object))
				{
					GeometricObject * typed_object = level->CreateTypedObject(this, geometric_object);
					if (typed_object != nullptr)
						typed_objects.push_back(typed_object);
				}

				// zones
				chaos::TiledMap::GeometricObjectSurface * surface = geometric_object->GetObjectSurface();
				if (surface != nullptr)
				{
					// is a trigger surface
					if (chaos::TiledMapTools::IsTriggerSurface(geometric_object))
					{
						TriggerSurfaceObject * trigger_surface = level->CreateTriggerSurface(this, geometric_object);
						if (trigger_surface != nullptr)
							trigger_surfaces.push_back(trigger_surface);
					}

#if 0
					chaos::box2 surface_box = surface->GetBoundingBox();
					if (!has_explicit_bounding_box)
						box = box | surface_box;

					chaos::TiledMap::GeometricObjectRectangle * rectangle = geometric_object->GetObjectRectangle();
					chaos::TiledMap::GeometricObjectEllipse   * ellipse = geometric_object->GetObjectEllipse();
					if (rectangle != nullptr || ellipse != nullptr)
					{
						continue;
					}
#endif

					chaos::TiledMap::GeometricObjectTile * tile = geometric_object->GetObjectTile();
					if (tile != nullptr)
					{
						int gid = tile->gid;
						// search the tile information 
						chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
						if (tile_info.tiledata == nullptr)
							continue;
						// create a simple particle
						chaos::box2 particle_box = tile->GetBoundingBox(false);
						particle_populator.AddParticle(tile_info.tiledata->atlas_key.c_str(), particle_box, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), gid, tile->horizontal_flip, tile->vertical_flip);
						continue;
					}					
				}
			}
			// final flush
			particle_populator.FlushParticles();
			// update the bounding box
			if (!explicit_bounding_box.IsEmpty())
				bounding_box = explicit_bounding_box;
			else
				bounding_box = box | particle_populator.GetBoundingBox();
				
			return true;
		}

		bool LayerInstance::FinalizeParticles()
		{
			// no layer, nothing to do !
			if (particle_layer == nullptr)
				return true;
			// no level ?
			Level * level = GetTypedLevel();
			if (level == nullptr)
				return true;
			// initialize each allocations
			size_t allocation_count = particle_layer->GetAllocationCount();
			for (size_t i = 0 ; i < allocation_count ; ++i)
				if (!level->FinalizeLayerParticles(this, particle_layer->GetAllocation(i)))
					return false;
				
			return true;
		}
		bool LayerInstance::InitializeParticleLayerNameAndTag(chaos::ParticleLayer * in_particle_layer)
		{
			std::string const * renderable_name = layer->FindPropertyString("RENDERABLE_NAME");
			if (renderable_name != nullptr)
				in_particle_layer->SetName(renderable_name->c_str());
			else
				in_particle_layer->SetName(layer->name.c_str());

			std::string const * renderable_tag = layer->FindPropertyString("RENDERABLE_TAG");
			if (renderable_tag != nullptr)
				in_particle_layer->SetTag(chaos::MakeStaticTagType(renderable_tag->c_str()));
			else
			{
				int const * layer_tag = layer->FindPropertyInt("RENDERABLE_TAG");
				if (layer_tag != nullptr)
					in_particle_layer->SetTag((chaos::TagType)*layer_tag);
			}

			return true;
		}

		chaos::ParticleAllocation * LayerInstance::CreateParticleAllocation()
		{
			if (particle_layer == nullptr)
			{
				// find render material
				chaos::GPURenderMaterial * render_material = FindRenderMaterial(material_name.c_str());
				if (render_material == nullptr)
					return nullptr;

				// create a particle layer
				particle_layer = GetTypedLevel()->CreateParticleLayer(this);
				if (particle_layer == nullptr)
					return false;
				// add name and tag to the particle_layer
				InitializeParticleLayerNameAndTag(particle_layer.get());
				// set the material
				particle_layer->SetRenderMaterial(render_material);
			}

			// create the allocation
			return particle_layer->SpawnParticles(0);
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::TileLayer * tile_layer)
		{
			Level * level = GetTypedLevel();

			// early exit for empty tile_layer
			size_t count = tile_layer->tile_indices.size();
			if (count == 0)
				return false;

			LayerInstanceParticlePopulator particle_populator;
			if (!particle_populator.Initialize(this))
				return false;

			// populate the layer
			chaos::TiledMap::Map * tiled_map = level_instance->GetTiledMap();

			for (size_t i = 0; i < count; ++i)
			{
				int gid = tile_layer->tile_indices[i];
				// search the tile information 
				chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
				if (tile_info.tiledata == nullptr)
					continue;
				// create a simple particle
				glm::ivec2  tile_coord   = tile_layer->GetTileCoordinate(i);
				chaos::box2 particle_box = tile_layer->GetTileBoundingBox(tile_coord, tile_info.tiledata->image_size, false);

				bool horizontal_flip = false;
				bool vertical_flip = false;
				particle_populator.AddParticle(tile_info.tiledata->atlas_key.c_str(), particle_box, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), gid, horizontal_flip, vertical_flip);
			}

			// final flush
			particle_populator.FlushParticles();
			// update the bounding box
			bounding_box = particle_populator.GetBoundingBox();

			return true;
		}

		void LayerInstance::ComputePlayerCollision(double delta_time)
		{
			// early exit
			if (!ArePlayerCollisionEnabled())
				return;
			// get the game
			Game * game = GetGame();
			if (game == nullptr)
				return;
			// get the player particle
			chaos::ParticleDefault::Particle * player_particle = game->GetPlayerParticle();
			if (player_particle == nullptr)
				return;
			// collision with surface triggers
			if (AreTriggerSurfacesEnabled())
				ComputePlayerCollisionWithSurfaceTriggers(delta_time, player_particle);
			// collision with tiles
			if (AreTileCollisionsEnabled())
				ComputePlayerTileCollisions(delta_time, player_particle);
		}

		void LayerInstance::ComputePlayerCollisionWithSurfaceTriggers(double delta_time, chaos::ParticleDefault::Particle * player_particle)
		{
			death::TiledMap::Level * level = GetTypedLevel();

			size_t count = trigger_surfaces.size();
			for (size_t i = 0; i < count; ++i)
			{
				TriggerSurfaceObject * trigger = trigger_surfaces[i].get();
				if (trigger == nullptr || !trigger->IsEnabled())
					continue;

				chaos::box2 trigger_box = trigger->GetBoundingBox(true);

				if (chaos::Collide(player_particle->bounding_box, trigger_box))
					if (!trigger->OnPlayerCollision(delta_time, player_particle))
						break;
			}			
		}

		void LayerInstance::ComputePlayerTileCollisions(double delta_time, chaos::ParticleDefault::Particle * player_particle)
		{
			death::TiledMap::Level * level = GetTypedLevel();

			// no particle layer, no collisions
			if (particle_layer == nullptr)
				return;
			// iterate over all allocations
			size_t allocation_count = particle_layer->GetAllocationCount();
			for (size_t i = 0; i < allocation_count; ++i)
			{
				chaos::ParticleAllocation * particle_allocation = particle_layer->GetAllocation(i);
				if (particle_allocation == nullptr)
					continue;

				chaos::ParticleAccessor<TileParticle> particles = particle_allocation->GetParticleAccessor<TileParticle>();

				size_t particle_count = particles.GetCount();
				for (size_t j = 0; j < particle_count; ++j)
				{
					TileParticle & particle = particles[j];

					if (player_particle == &particle) // ignore self collision
						continue;
					if (chaos::Collide(player_particle->bounding_box, particle.bounding_box))
						if (!level->OnPlayerTileCollision(delta_time, player_particle, &particle))
							return;
				}
			}
		}

		bool LayerInstance::DoTick(double delta_time)
		{
			if (particle_layer != nullptr)
				particle_layer->Tick(delta_time);
			return true;
		}
		
		int LayerInstance::DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
		{
			// early exit
			int result = 0;
			if (particle_layer == nullptr)
				return result;

			// camera is expressed in world, so is for layer
			chaos::box2 layer_box  = GetBoundingBox(true);
			chaos::box2 camera_box = GetGame()->GetCameraBox();
			chaos::box2 initial_camera_box = GetGame()->GetInitialCameraBox();

			// XXX : we want some layers to appear further or more near the camera
			//       the displacement_ratio represent how fast this layer is moving relatively to other layers.
			//       The reference layer is the layer where the 'effective' camera (and so the PlayerStart is)
			//         => when player goes outside the screen, the camera is updated so that it is still watching the player
			//         => that why we consider the PlayerStart's layer as the reference
			//       to simulate other layer's speed, we just create at rendering time 'virtual cameras' (here this is 'final_camera_box')
			//       We only multiply 'true camera' distance from its initial position by a ratio value

			// apply the displacement to the camera
			chaos::box2 final_camera_box = camera_box;

			glm::vec2 final_ratio = glm::vec2(1.0f, 1.0f);
			if (level_instance->reference_layer != nullptr && level_instance->reference_layer != this)
			{
				if (level_instance->reference_layer->displacement_ratio.x != 0.0f)
					final_ratio.x = displacement_ratio.x / level_instance->reference_layer->displacement_ratio.x;
				if (level_instance->reference_layer->displacement_ratio.y != 0.0f)
					final_ratio.y = displacement_ratio.y / level_instance->reference_layer->displacement_ratio.y;
			}

			final_camera_box.position = initial_camera_box.position + (camera_box.position - initial_camera_box.position) * final_ratio;

			// compute repetitions
			BoxScissoringWithRepetitionResult scissor_result = BoxScissoringWithRepetitionResult(layer_box, final_camera_box, wrap_x, wrap_y);

			// HACK : due to bad LAYER_BOUNDING_BOX computation, the layer containing PLAYER_START may be clamped and layer hidden
			glm::ivec2 start_instance = scissor_result.start_instance;
			glm::ivec2 last_instance  = scissor_result.last_instance;			
			if (this == level_instance->reference_layer || layer_box.IsEmpty()) 
			{
				start_instance = glm::ivec2(0, 0);
				last_instance  = glm::ivec2(1, 1); // always see fully the layer without clamp => repetition not working
			}

			// new provider for camera override (will be fullfill only if necessary)
			chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
			
			// draw instances 
			int draw_instance_count = 0;
			for (int x = start_instance.x; x < last_instance.x; ++x)
			{
				for (int y = start_instance.y; y < last_instance.y; ++y)
				{
					// override the camera box only if there is at least one draw call
					if (draw_instance_count++ == 0)
						main_uniform_provider.AddVariableValue("camera_box", chaos::EncodeBoxToVector(final_camera_box));
					// new Provider to apply the offset for this 'instance'
					chaos::GPUProgramProviderChain instance_uniform_provider(&main_uniform_provider);
					instance_uniform_provider.AddVariableValue("offset", scissor_result.GetInstanceOffset(glm::ivec2(x, y)) + offset);
					// draw call
					result += particle_layer->Display(&instance_uniform_provider, render_params);
				}
			}
			return result;
		}


#define DEATH_EMPTY_TOKEN
#define DEATH_FIND_OBJECT(result_type, func_name, member_vector, constness)\
		result_type constness * LayerInstance::func_name(char const * name) constness\
		{\
			if (name == nullptr && member_vector.size() > 0)\
				return member_vector[0].get();\
			return NamedObject::FindNamedObject(member_vector, name);\
		}
		DEATH_FIND_OBJECT(GeometricObject, FindTypedObject, typed_objects, DEATH_EMPTY_TOKEN);
		DEATH_FIND_OBJECT(GeometricObject, FindTypedObject, typed_objects, const);
		DEATH_FIND_OBJECT(TriggerSurfaceObject, FindTriggerSurface, trigger_surfaces, DEATH_EMPTY_TOKEN);
		DEATH_FIND_OBJECT(TriggerSurfaceObject, FindTriggerSurface, trigger_surfaces, const);
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, player_starts, DEATH_EMPTY_TOKEN);
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, player_starts, const);
		DEATH_FIND_OBJECT(CameraObject, FindCamera, cameras, DEATH_EMPTY_TOKEN);
		DEATH_FIND_OBJECT(CameraObject, FindCamera, cameras, const);

#undef DEATH_EMPTY_TOKEN
#undef DEATH_FIND_OBJECT

		// =====================================
		// LevelInstance implementation
		// =====================================

		chaos::TiledMap::Map * LevelInstance::GetTiledMap()
		{
			Level * level = GetTypedLevel();
			if (level == nullptr)
				return nullptr;
			return level->GetTiledMap();
		}

		chaos::TiledMap::Map const * LevelInstance::GetTiledMap() const 
		{
			Level const * level = GetTypedLevel();
			if (level == nullptr)
				return nullptr;
			return level->GetTiledMap();
		}

		Level * LevelInstance::GetTypedLevel()
		{
			return dynamic_cast<Level*>(GetLevel());
		}

		Level const * LevelInstance::GetTypedLevel() const
		{
			return dynamic_cast<Level const *>(GetLevel());
		}

		void LevelInstance::ComputePlayerCollision(double delta_time)
		{
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				layer_instances[i]->ComputePlayerCollision(delta_time);
		}

		bool LevelInstance::DoTick(double delta_time)
		{
			// tick the particle manager
			if (particle_manager != nullptr)
				particle_manager->Tick(delta_time);
			// tick all layer instances
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				layer_instances[i]->Tick(delta_time);
			// compute the collisions with the player
			ComputePlayerCollision(delta_time);

			return true;
		}

		int LevelInstance::DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
		{
			int result = 0;

			// display particle manager
			if (particle_manager != nullptr)
				result += particle_manager->Display(uniform_provider, render_params);
			// draw the layer instances0
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count ; ++i)
				result += layer_instances[i]->Display(uniform_provider, render_params);
			
			return result;
		}

		bool LevelInstance::Initialize(Game * in_game)
		{
			assert(in_game != nullptr);

			// keep a reference on the game
			game = in_game;
			// create a the layers instances
			if (!CreateLayerInstances(in_game))
				return false;
			// create a particle manager
			if (!CreateParticleManager(in_game))
				return false;

 			return true;
		}

		bool LevelInstance::CreateParticleManager(Game * in_game)
		{
			particle_manager = new chaos::ParticleManager;
			if (particle_manager == nullptr)
				return false;
			particle_manager->SetTextureAtlas(in_game->GetTextureAtlas()); // take the atlas
			return true;
		}

		bool LevelInstance::CreateLayerInstances(Game * in_game)
		{
			Level * level = GetTypedLevel();

			chaos::TiledMap::Map * tiled_map = GetTiledMap();

			// handle layers ordered by Z-Order
			size_t count = tiled_map->GetLayerCount();
			for (size_t i = 0; i < count; ++i)
			{
				// get the chaos::LayerBase object per Z-order
				chaos::TiledMap::LayerBase * layer = tiled_map->FindLayerByZOrder(i);
				if (layer == nullptr)
					continue;
				// create and store the layer_instance
				LayerInstance * layer_instance = level->CreateLayerInstance(this, layer);
				if (layer_instance != nullptr)
					layer_instances.push_back(layer_instance);	
			}
			return true;
		}

		chaos::box2 LevelInstance::GetBoundingBox() const
		{
			// explicit bounding box
			if (has_explicit_bounding_box)
				return explicit_bounding_box;
			// depend on layers
			chaos::box2 result;
			size_t count = layer_instances.size();
			for (size_t i = 0 ; i < count ; ++i)
				result = result | layer_instances[i]->GetBoundingBox(true); // expressed in world system the bounding boxes
			return result;
		}

		chaos::GPURenderMaterial * LevelInstance::GetDefaultRenderMaterial()
		{
			if (default_material == nullptr)
				default_material = GetTypedLevel()->GetDefaultRenderMaterial(); // create material and cache
			return default_material.get();
		}


#define DEATH_EMPTY_TOKEN
#define DEATH_FIND_OBJECT(result_type, func_name, constness)\
		result_type constness * LevelInstance::func_name(char const * name) constness\
		{\
			size_t count = layer_instances.size();\
			for (size_t i = 0; i < count; ++i)\
			{\
				result_type constness * result = layer_instances[i]->func_name(name);\
				if (result != nullptr)\
					return result;\
			}\
			return nullptr;\
		}
		DEATH_FIND_OBJECT(GeometricObject, FindTypedObject, DEATH_EMPTY_TOKEN);
		DEATH_FIND_OBJECT(GeometricObject, FindTypedObject, const);
		DEATH_FIND_OBJECT(TriggerSurfaceObject, FindTriggerSurface, DEATH_EMPTY_TOKEN);
		DEATH_FIND_OBJECT(TriggerSurfaceObject, FindTriggerSurface, const);
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, DEATH_EMPTY_TOKEN);
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, const);
		DEATH_FIND_OBJECT(CameraObject, FindCamera, DEATH_EMPTY_TOKEN);
		DEATH_FIND_OBJECT(CameraObject, FindCamera, const);

#undef DEATH_EMPTY_TOKEN
#undef DEATH_FIND_OBJECT

		LayerInstance * LevelInstance::FindLayerInstance(char const * name)
		{
			if (name == nullptr && layer_instances.size() > 0)
				return layer_instances[0].get();
				return NamedObject::FindNamedObject(layer_instances, name);
		}
		LayerInstance const * LevelInstance::FindLayerInstance(char const * name) const
		{
			if (name == nullptr && layer_instances.size() > 0)
				return layer_instances[0].get();
			return NamedObject::FindNamedObject(layer_instances, name);
		}

		void LevelInstance::OnLevelEnded()
		{
			GameLevelInstance::OnLevelEnded();
			UnSpawnPlayer();		
		}

		void LevelInstance::OnLevelStarted()
		{
			GameLevelInstance::OnLevelStarted();
			SpawnPlayer();
			CreateCamera();
		}

		void LevelInstance::CreateCamera()
		{
			Level * level = GetTypedLevel();

			// search CAMERA NAME
			std::string const * camera_name = level->GetTiledMap()->FindPropertyString("CAMERA_NAME");

			// search the CAMERA
			TiledMap::CameraObject * camera = nullptr;
			if (camera_name != nullptr)
			{
				camera = FindCamera(camera_name->c_str()); // first, if a name is given, use it
			}
			if (camera == nullptr)
			{
				camera = FindCamera(nullptr); // try to find the very first one otherwise
				if (camera == nullptr)
					return;
			}

			// create camera
			chaos::TiledMap::GeometricObjectSurface * camera_surface = camera->GetGeometricObject()->GetObjectSurface();
			if (camera_surface == nullptr)
				return;

			chaos::box2 camera_box = chaos::AlterBoxToAspect(camera_surface->GetBoundingBox(true), 16.0f / 9.0f, true);
			game->SetCameraBox(camera_box);
			game->SetInitialCameraBox(camera_box);
		}

		void LevelInstance::UnSpawnPlayer()
		{
			game->SetPlayerAllocation(nullptr);
			game->SetCameraBox(chaos::box2()); // reset the camera
		}

		void LevelInstance::SpawnPlayer()
		{
			Level * level = GetTypedLevel();

			// search PLAYER START NAME
			std::string const * player_start_name = level->GetTiledMap()->FindPropertyString("PLAYER_START_NAME");
			
			// search the PLAYER START
			TiledMap::PlayerStartObject * player_start = nullptr;
			if (player_start_name != nullptr)
			{
				player_start = FindPlayerStart(player_start_name->c_str()); // first, if a name is given, use it
			}
			if (player_start == nullptr)
			{
				player_start = FindPlayerStart(nullptr); // try to find the very first one otherwise
				if (player_start == nullptr)
					return;
			}

			// search the bitmap name for the player
			std::string const * bitmap_name = player_start->GetGeometricObject()->FindPropertyString("BITMAP_NAME");
			if (bitmap_name == nullptr)
				return;

			// initialize some data
			TiledMap::LayerInstance * layer_instance = player_start->GetLayerInstance();
			if (layer_instance == nullptr)
				return;

			// create a particle populator
			LayerInstanceParticlePopulator particle_populator;
			if (!particle_populator.Initialize(layer_instance))
				return;

			// compute the bounding box
			chaos::box2 player_bounding_box;
			player_bounding_box.position = player_start->GetGeometricObject()->position;

			chaos::TiledMap::GeometricObjectSurface const * object_surface = player_start->GetGeometricObject()->GetObjectSurface();
			if (object_surface != nullptr)
				player_bounding_box = object_surface->GetBoundingBox(false);

			particle_populator.AddParticle(bitmap_name->c_str(), player_bounding_box);
			particle_populator.FlushParticles();

			// allocation
			chaos::ParticleAllocation * player_allocation = particle_populator.GetParticleAllocation();

			// set the player allocation
			game->SetPlayerAllocation(player_allocation);

			// XXX : while camera, is restricted so we can see player, we considere that the displacement_ratio of the layer containing the player start is the reference one
			reference_displacement_ratio = layer_instance->displacement_ratio;
			reference_layer = layer_instance;

			// shuxxx : first time FinalizeParticles(...) was called, there was no effect because the PlayerStartLayer has no particle. 
			//          call it twice as a fast fix
			layer_instance->FinalizeParticles();
		}

	}; // namespace TiledMap

}; // namespace death
