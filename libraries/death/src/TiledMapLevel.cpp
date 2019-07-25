#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/LayerInstanceParticlePopulator.h>
#include <death/Player.h>

#include <chaos/CollisionFramework.h>
#include <chaos/TiledMapTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GPUProgramGenerator.h>
#include <chaos/StringTools.h>

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

			glm::vec2 target_bottomleft = GetBoxExtremums(in_target_box).first;
			glm::vec2 scissor_bottomleft = GetBoxExtremums(in_scissor_box).first;

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
			// keep a copy of the tiled object
			tiled_map = in_tiled_map;
			// get level title (if specified)
			std::string const * in_level_title = in_tiled_map->FindPropertyString("LEVEL_TITLE");
			if (in_level_title != nullptr)
				level_title = *in_level_title;
			
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

		chaos::ParticleLayerBase * Level::CreateParticleLayer(LayerInstance * layer_instance)
		{
			return new chaos::ParticleLayer<death::TiledMap::TileParticleTrait>();
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
			char const * vertex_shader_source = R"VERTEXSHADERCODE(
			in vec2 position;
			in vec3 texcoord;
			in vec4 color;

			out vec2 vs_position;
			out vec3 vs_texcoord;
			out vec4 vs_color;

			uniform vec2 offset;
			uniform mat4 camera_transform;
			uniform vec2 view_half_size;

			void main()
			{
				vec2 pos = position + offset;

				vs_position = pos;
				vs_texcoord = texcoord;
				vs_color = color;

				vec4 transformed_pos = camera_transform * vec4(pos.x, pos.y, 0.0, 1.0);

				gl_Position.xy = transformed_pos.xy / view_half_size;
				gl_Position.z = 0.0;
				gl_Position.w = 1.0;
			}									
		)VERTEXSHADERCODE";

			char const * pixel_shader_source = R"PIXELSHADERCODE(
			out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

			in vec2 vs_position;
			in vec3 vs_texcoord;
			in vec4 vs_color;

			uniform sampler2DArray material;

			void main()
			{
				vec4 color = (vs_texcoord.x < 0.0 || vs_texcoord.y < 0.0)? 
					vec4(1.0, 1.0, 1.0, 1.0) : 
					texture(material, vs_texcoord);
				output_color.xyz = color.xyz * vs_color.xyz;
				output_color.a   = vs_color.a * color.a;
			};
		)PIXELSHADERCODE";

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

		bool Level::OnPlayerTileCollision(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, TileParticle * particle)
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
			outside_box_factor = geometric_object->FindPropertyFloat("OUTSIDE_BOX_FACTOR", 1.0f);
		
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

		bool TriggerSurfaceObject::IsCollisionWith(chaos::box2 const & other_box, std::vector<chaos::weak_ptr<TriggerSurfaceObject>> const * triggers) const
		{
			chaos::box2 box = GetBoundingBox(true);

			if (outside_box_factor > 1.0f) // the box is bigger when we want to go outside !
			{
				// search whether we were already colliding with
				if (triggers != nullptr)
					if (std::find(triggers->begin(), triggers->end(), this) != triggers->end()) // we where already colliding the object
						box.half_size *= outside_box_factor;
			}
			return chaos::Collide(other_box, box);
		}

		bool TriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
		{
			return true; // continue other collisions
		}

		bool TriggerSurfaceObject::OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type)
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

		Level * LayerInstance::GetTiledLevel()
		{
			if (level_instance == nullptr)
				return nullptr;
			return level_instance->GetTiledLevel();
		}

		Level const * LayerInstance::GetTiledLevel() const
		{
			if (level_instance == nullptr)
				return nullptr;
			return level_instance->GetTiledLevel();
		}

		LevelInstance * LayerInstance::GetTiledLevelInstance()
		{
			return level_instance;
		}

		LevelInstance const * LayerInstance::GetTiledLevelInstance() const
		{
			return level_instance;
		}

		chaos::box2 LayerInstance::GetBoundingBox(bool world_system) const
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

		chaos::GPURenderMaterial * LayerInstance::FindOrCreateRenderMaterial(char const * material_name)
		{
			if (material_name != nullptr && material_name[0] != 0) // unamed material ?
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
			camera_collision_enabled = layer->FindPropertyBool("CAMERA_COLLISIONS_ENABLED", false);
			tile_collisions_enabled  = layer->FindPropertyBool("TILE_COLLISIONS_ENABLED", false);

			infinite_bounding_box = layer->FindPropertyBool("INFINITE_BOUNDING_BOX", false);

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
				return FinalizeParticles();
			}

			chaos::TiledMap::ObjectLayer * object_layer = layer->GetObjectLayer();
			if (object_layer != nullptr)
			{
				if (!InitializeLayer(object_layer))
					return false;
				return FinalizeParticles();
			}

			chaos::TiledMap::TileLayer * tile_layer = layer->GetTileLayer();
			if (tile_layer != nullptr)
			{
				if (!InitializeLayer(tile_layer))
					return false;
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
			Level * level = GetTiledLevel();

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
				if (IsGeometryEmpty(explicit_bounding_box) && chaos::TiledMapTools::IsLayerBoundingBox(geometric_object))
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
					TriggerSurfaceObject * trigger_surface = nullptr;
					if (chaos::TiledMapTools::IsTriggerSurface(geometric_object))
					{
						trigger_surface = level->CreateTriggerSurface(this, geometric_object);
						if (trigger_surface != nullptr)
						{
							trigger_surfaces.push_back(trigger_surface);
							if (!trigger_surface->IsTileCreationEnabled())
								continue;
						}
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

				//		if (!tile->visible)
				//			continue;

						// search the tile information 
						chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
						if (tile_info.tiledata == nullptr)
							continue;

						// aspect ratio is to be maintained ??
						bool keep_aspect_ratio = true;

						bool * prop = tile->FindPropertyBool("KEEP_ASPECT_RATIO");
						if (prop != nullptr)
							keep_aspect_ratio = *prop;
						else 
						{
							prop = tile_info.tiledata->FindPropertyBool("KEEP_ASPECT_RATIO");
							if (prop != nullptr)
								keep_aspect_ratio = *prop;						
						}

							
						// create a simple particle
						chaos::box2 particle_box = tile->GetBoundingBox(true);
						if (trigger_surface != nullptr)
						{
							keep_aspect_ratio = false;
							particle_box = trigger_surface->GetBoundingBox(false); // shuxxx : the TILE is generated on the same layer then the surface. does it get the layer_offset ????
						}
						particle_populator.AddParticle(tile_info.tiledata->atlas_key.c_str(), particle_box, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), gid, tile->horizontal_flip, tile->vertical_flip, keep_aspect_ratio);
						continue;
					}	


					// texts
					chaos::TiledMap::GeometricObjectText * text = geometric_object->GetObjectText();
					if (text != nullptr)
					{
						// create particle layer if necessary
						if (particle_layer == nullptr)
							if (CreateParticleLayer() == nullptr) // the generate layer is of  TileParticleTrait => this works fine for Text (except a useless GID per particle)
								continue;

						Game * game = GetGame();

						chaos::ParticleTextGenerator::GeneratorResult result;
						chaos::ParticleTextGenerator::GeneratorParams params;

						params.line_height = (float)text->pixelsize;
						params.hotpoint_type = chaos::Hotpoint::TOP | chaos::Hotpoint::HMIDDLE;
						params.position = text->position;
						params.default_color = text->color;


						std::string const * font_name = text->FindPropertyString("FONT_NAME");
						if (font_name != nullptr)
							params.font_info_name = *font_name;

						game->GetTextGenerator()->Generate(text->text.c_str(), result, params);

						// create the text
						chaos::ParticleTextGenerator::CreateTextAllocation(particle_layer.get(), result);
						continue;
					}
				}
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

		bool LayerInstance::FinalizeParticles()
		{
			// no layer, nothing to do !
			if (particle_layer == nullptr)
				return true;
			// no level ?
			Level * level = GetTiledLevel();
			if (level == nullptr)
				return true;
			// initialize each allocations
			size_t allocation_count = particle_layer->GetAllocationCount();
			for (size_t i = 0 ; i < allocation_count ; ++i)
				if (!level->FinalizeLayerParticles(this, particle_layer->GetAllocation(i)))
					return false;
				
			return true;
		}
		bool LayerInstance::InitializeParticleLayer(chaos::ParticleLayerBase * in_particle_layer)
		{
			// the name
			std::string const * renderable_name = layer->FindPropertyString("RENDERABLE_NAME");
			if (renderable_name != nullptr)
				in_particle_layer->SetName(renderable_name->c_str());
			else
				in_particle_layer->SetName(layer->name.c_str());
			// the tag
			std::string const * renderable_tag = layer->FindPropertyString("RENDERABLE_TAG");
			if (renderable_tag != nullptr)
				in_particle_layer->SetTag(chaos::MakeStaticTagType(renderable_tag->c_str()));
			else
			{
				int const * layer_tag = layer->FindPropertyInt("RENDERABLE_TAG");
				if (layer_tag != nullptr)
					in_particle_layer->SetTag((chaos::TagType)*layer_tag);
			}
			// enabled renderpasses
			std::string const * enabled_renderpass = layer->FindPropertyString("ENABLED_RENDERPASS");
			if (enabled_renderpass != nullptr)
			{
				std::vector<std::string> passes = chaos::StringTools::Split(enabled_renderpass->c_str(), ';');
				for (std::string const & pass : passes)
					in_particle_layer->AddEnabledRenderPass(pass.c_str());
			}
			// disabled renderpasses
			std::string const * disabled_renderpass = layer->FindPropertyString("DISABLED_RENDERPASS");
			if (disabled_renderpass != nullptr)
			{
				std::vector<std::string> passes = chaos::StringTools::Split(disabled_renderpass->c_str(), ';');
				for (std::string const & pass : passes)
					in_particle_layer->AddDisabledRenderPass(pass.c_str());
			}

			return true;
		}

		chaos::ParticleAllocationBase * LayerInstance::CreateParticleAllocation()
		{
			// create particle layer if necessary
			if (particle_layer == nullptr)
				if (CreateParticleLayer() == nullptr)
					return nullptr;
			// create the allocation
			return particle_layer->SpawnParticles(0);
		}

		chaos::ParticleLayerBase * LayerInstance::CreateParticleLayer()
		{
			if (particle_layer == nullptr)
			{
				// find render material
				chaos::GPURenderMaterial * render_material = FindOrCreateRenderMaterial(material_name.c_str());
				if (render_material == nullptr)
					return nullptr;
				// create a particle layer
				particle_layer = GetTiledLevel()->CreateParticleLayer(this);
				if (particle_layer == nullptr)
					return false;
				// add name and tag to the particle_layer
				InitializeParticleLayer(particle_layer.get());
				// set the material
				particle_layer->SetRenderMaterial(render_material);
			}
			return particle_layer.get();
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::TileLayer * tile_layer)
		{
			Level * level = GetTiledLevel();

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

		void LayerInstance::ComputePlayerAndCameraCollision(double delta_time)
		{
			// get the game
			Game * game = GetGame();
			if (game == nullptr)
				return;

			// check player collisions
			if (ArePlayerCollisionEnabled())
			{
				// compute the collisions for all players
				size_t player_count = game->GetPlayerCount();
				for (size_t i = 0; i < player_count; ++i)
				{
					Player * player = game->GetPlayer(i);
					if (player == nullptr)
						continue;
					// get the player particle
					chaos::ParticleDefault::Particle * player_particle = player->GetPlayerParticle();
					if (player_particle == nullptr)
						continue;
					// collision with surface triggers
					if (AreTriggerSurfacesEnabled())
						if (!ComputePlayerCollisionWithSurfaceTriggers(delta_time, player, player_particle))
							continue;
					// collision with tiles
					if (AreTileCollisionsEnabled())
						if (!ComputePlayerTileCollisions(delta_time, player, player_particle))
							continue;
				}									
			}
				
			// compute collision with camera
			if (AreCameraCollisionEnabled())
			{
				chaos::box2 camera_box = game->GetLevelInstance()->GetCameraBox(0);
				if (!IsGeometryEmpty(camera_box))
				{
					if (AreTriggerSurfacesEnabled())
						ComputeCameraCollisionWithSurfaceTriggers(delta_time, camera_box);
				}
			}
		}

		PlayerAndTriggerCollisionRecord * LayerInstance::FindPlayerCollisionRecord(death::Player * player)
		{
			size_t count = collision_records.size();
			for (size_t i = count; i > 0; --i)
			{
				size_t index = i - 1;
				if (collision_records[index].player == nullptr)
					collision_records.erase(collision_records.begin() + index);
				else if (collision_records[index].player == player)
					return &collision_records[index];
			}
			return nullptr;
		}


		bool LayerInstance::ComputeCameraCollisionWithSurfaceTriggers(double delta_time, chaos::box2 const & camera_box)
		{
			// the new colliding triggers
			std::vector<chaos::weak_ptr<TriggerSurfaceObject>> triggers;

			// search all colliding triggers
			size_t surfaces_count = trigger_surfaces.size();
			for (size_t i = 0; i < surfaces_count; ++i)
			{
				TriggerSurfaceObject * trigger = trigger_surfaces[i].get();
				if (trigger == nullptr || !trigger->IsEnabled())
					continue;
				// detect collision
				if (trigger->IsCollisionWith(camera_box, &camera_collision_records))
					triggers.push_back(trigger);
			}

			// triggers collisions 
			size_t triggers_count = triggers.size();
			for (size_t i = 0; i < triggers_count; ++i)
			{
				bool already_colliding = false;
				if (std::find(camera_collision_records.begin(), camera_collision_records.end(), triggers[i]) != camera_collision_records.end()) // search in previous frame data						
					already_colliding = true;
				if (!triggers[i]->OnCameraCollisionEvent(delta_time, camera_box, (already_colliding) ? TriggerSurfaceObject::COLLISION_AGAIN : TriggerSurfaceObject::COLLISION_STARTED))
					return false;
			}

			// triggers end of collisions
			size_t previous_count = camera_collision_records.size();
			for (size_t i = 0; i < previous_count; ++i)
			{
				if (std::find(triggers.begin(), triggers.end(), camera_collision_records[i]) == triggers.end()) // no more colliding
					if (!camera_collision_records[i]->OnCameraCollisionEvent(delta_time, camera_box, TriggerSurfaceObject::COLLISION_FINISHED))
						return false;
			}

			// store the new triggers
			camera_collision_records = std::move(triggers);

			return true;
		
		}

		bool LayerInstance::ComputePlayerCollisionWithSurfaceTriggers(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle)
		{
			// the new colliding triggers
			std::vector<chaos::weak_ptr<TriggerSurfaceObject>> triggers;
			// the previous colliding triggers
			PlayerAndTriggerCollisionRecord * previous_collisions = FindPlayerCollisionRecord(player);

			// search all colliding triggers
			size_t surfaces_count = trigger_surfaces.size();
			for (size_t i = 0; i < surfaces_count; ++i)
			{
				TriggerSurfaceObject * trigger = trigger_surfaces[i].get();
				if (trigger == nullptr || !trigger->IsEnabled())
					continue;
				// detect collision
				if (trigger->IsCollisionWith(player_particle->bounding_box, (previous_collisions != nullptr)? &previous_collisions->triggers : nullptr))
					triggers.push_back(trigger);
			}

			// triggers collisions 
			size_t triggers_count = triggers.size();
			for (size_t i = 0; i < triggers_count; ++i)
			{
				bool already_colliding = false;
				if (previous_collisions != nullptr)
					if (std::find(previous_collisions->triggers.begin(), previous_collisions->triggers.end(), triggers[i]) != previous_collisions->triggers.end()) // search in previous frame data
						already_colliding = true;
				if (!triggers[i]->OnPlayerCollisionEvent(delta_time, player, player_particle, (already_colliding) ? TriggerSurfaceObject::COLLISION_AGAIN : TriggerSurfaceObject::COLLISION_STARTED))
					return false;
			}

			// triggers end of collisions
			if (previous_collisions != nullptr)
			{
				size_t previous_count = previous_collisions->triggers.size();
				for (size_t i = 0; i < previous_count; ++i)
				{
					if (std::find(triggers.begin(), triggers.end(), previous_collisions->triggers[i]) == triggers.end()) // no more colliding
						if (!previous_collisions->triggers[i]->OnPlayerCollisionEvent(delta_time, player, player_particle, TriggerSurfaceObject::COLLISION_FINISHED))
							return false;
				}
			}

			// store the record
			if (previous_collisions != nullptr)
				previous_collisions->triggers = std::move(triggers);
			else
			{
				PlayerAndTriggerCollisionRecord new_record;
				new_record.player = player;
				new_record.triggers = std::move(triggers);
				collision_records.push_back(std::move(new_record));
			}
			return true; // continue other collisions 
		}

		bool LayerInstance::ComputePlayerTileCollisions(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle)
		{
			death::TiledMap::Level * level = GetTiledLevel();

			return FindTileCollisions(player_particle->bounding_box, [this, delta_time, player, player_particle, level](TileParticle & tile_particle)
			{
				// ignore self collision
				if (player_particle == &tile_particle) 
					return true;
				// stop other collisions
				if (!level->OnPlayerTileCollision(delta_time, player, player_particle, &tile_particle)) 
					return false;
				
				return true;
			});
		}

		bool LayerInstance::DoTick(double delta_time)
		{
			// tick the game objects
#if 1
			size_t player_start_count = player_starts.size();
			for (size_t i = 0; i < player_start_count; ++i)
				player_starts[i]->Tick(delta_time);

			size_t camera_count = cameras.size();
			for (size_t i = 0; i < camera_count; ++i)
				cameras[i]->Tick(delta_time);
#endif

			size_t trigger_count = trigger_surfaces.size();
			for (size_t i = 0; i < trigger_count; ++i)
				trigger_surfaces[i]->Tick(delta_time);

			size_t typed_count = typed_objects.size();
			for (size_t i = 0; i < typed_count; ++i)
				typed_objects[i]->Tick(delta_time);

			// tick the particles
			if (particle_layer != nullptr)
				particle_layer->Tick(delta_time);
			return true;
		}
		
		int LayerInstance::DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
		{
			// early exit
			int result = 0;
			if (particle_layer == nullptr)
				return result;







			
	//	main_uniform_provider.AddVariableValue("camera_transform", camera_transform.transform);
	//		main_uniform_provider.AddVariableValue("view_half_size", camera_transform.view_half_size);






			// camera is expressed in world, so is for layer
			CameraTransform camera_transform = GetTiledLevelInstance()->GetCameraTransform(0);
			glm::vec3 initial_camera_position = camera_transform.transform[3];

			chaos::box2 layer_box  = GetBoundingBox(true);
			chaos::box2 camera_box = GetTiledLevelInstance()->GetCameraBox(0);
			chaos::box2 initial_camera_box = GetTiledLevelInstance()->GetInitialCameraBox(0);

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
			if (this == level_instance->reference_layer || IsGeometryEmpty(layer_box)) 
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


					if (draw_instance_count++ == 0)
					{
					//	final_camera_box.position = initial_camera_box.position + (camera_box.position - initial_camera_box.position) * final_ratio;


					}











					// new Provider to apply the offset for this 'instance'
					chaos::GPUProgramProviderChain instance_uniform_provider(&main_uniform_provider);
					instance_uniform_provider.AddVariableValue("offset", scissor_result.GetInstanceOffset(glm::ivec2(x, y)) + offset);
					// draw call
					result += particle_layer->Display(renderer, &instance_uniform_provider, render_params);
				}
			}
			return result;
		}

#define DEATH_FIND_OBJECT(result_type, func_name, member_vector, constness)\
		result_type constness * LayerInstance::func_name(char const * name) constness\
		{\
			if (name == nullptr && member_vector.size() > 0)\
				return member_vector[0].get();\
			return NamedObject::FindNamedObject(member_vector, name);\
		}
		DEATH_FIND_OBJECT(GeometricObject, FindTypedObject, typed_objects, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(GeometricObject, FindTypedObject, typed_objects, const);
		DEATH_FIND_OBJECT(TriggerSurfaceObject, FindTriggerSurface, trigger_surfaces, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(TriggerSurfaceObject, FindTriggerSurface, trigger_surfaces, const);
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, player_starts, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, player_starts, const);
		DEATH_FIND_OBJECT(CameraObject, FindCamera, cameras, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(CameraObject, FindCamera, cameras, const);

#undef DEATH_FIND_OBJECT

		
		size_t LayerInstance::GetTriggerSurfaceCount() const
		{
			return trigger_surfaces.size();
		}
		
		TriggerSurfaceObject * LayerInstance::GetTriggerSurface(size_t index)
		{
			if (index >= trigger_surfaces.size())
				return nullptr;
			return trigger_surfaces[index].get();		
		}
		
		TriggerSurfaceObject const * LayerInstance::GetTriggerSurface(size_t index) const
		{
			if (index >= trigger_surfaces.size())
				return nullptr;
			return trigger_surfaces[index].get();				
		}

		// =====================================
		// LevelInstance implementation
		// =====================================

		chaos::TiledMap::Map * LevelInstance::GetTiledMap()
		{
			Level * level = GetTiledLevel();
			if (level == nullptr)
				return nullptr;
			return level->GetTiledMap();
		}

		chaos::TiledMap::Map const * LevelInstance::GetTiledMap() const 
		{
			Level const * level = GetTiledLevel();
			if (level == nullptr)
				return nullptr;
			return level->GetTiledMap();
		}

		Level * LevelInstance::GetTiledLevel()
		{
			return auto_cast(GetLevel());
		}

		Level const * LevelInstance::GetTiledLevel() const
		{
			return auto_cast(GetLevel());
		}

		void LevelInstance::ComputePlayerAndCameraCollision(double delta_time)
		{
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				layer_instances[i]->ComputePlayerAndCameraCollision(delta_time);
		}

		bool LevelInstance::DoTick(double delta_time)
		{
			death::GameLevelInstance::DoTick(delta_time);

			// tick the particle manager
			if (particle_manager != nullptr)
				particle_manager->Tick(delta_time);
			// tick all layer instances
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				layer_instances[i]->Tick(delta_time);
			// compute the collisions with the player
			ComputePlayerAndCameraCollision(delta_time);

			return true;
		}

		int LevelInstance::DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
		{
			int result = 0;

			// display particle manager
			if (particle_manager != nullptr)
				result += particle_manager->Display(renderer, uniform_provider, render_params);
			// draw the layer instances0
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count ; ++i)
				result += layer_instances[i]->Display(renderer, uniform_provider, render_params);
			
			return result;
		}

		bool LevelInstance::Initialize(Game * in_game, GameLevel * in_level)
		{
			if (!GameLevelInstance::Initialize(in_game, in_level))
				return false;
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
			Level * level = GetTiledLevel();

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
				default_material = GetTiledLevel()->GetDefaultRenderMaterial(); // create material and cache
			return default_material.get();
		}

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
		DEATH_FIND_OBJECT(GeometricObject, FindTypedObject, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(GeometricObject, FindTypedObject, const);
		DEATH_FIND_OBJECT(TriggerSurfaceObject, FindTriggerSurface, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(TriggerSurfaceObject, FindTriggerSurface, const);
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, const);
		DEATH_FIND_OBJECT(CameraObject, FindCamera, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(CameraObject, FindCamera, const);

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

		void LevelInstance::CreateCameras()
		{
			Level * level = GetTiledLevel();

			// search CAMERA NAME
			std::string const * camera_name = level->GetTiledMap()->FindPropertyString("CAMERA_NAME");

			// search the CAMERA
			TiledMap::CameraObject * camera_object = nullptr;
			if (camera_name != nullptr)
			{
				camera_object = FindCamera(camera_name->c_str()); // first, if a name is given, use it
			}
			if (camera_object == nullptr)
			{
				camera_object = FindCamera(nullptr); // try to find the very first one otherwise
				if (camera_object == nullptr)
					return;
			}

			// compute the surface
			chaos::TiledMap::GeometricObjectSurface * camera_surface = camera_object->GetGeometricObject()->GetObjectSurface();
			if (camera_surface == nullptr)
				return;
			chaos::box2 camera_box = chaos::AlterBoxToAspect(camera_surface->GetBoundingBox(true), 16.0f / 9.0f, true);

			// create the real camera
			Camera * camera = new Camera(this);
			if (camera == nullptr)
				return;
			cameras.push_back(camera);

			// initialize the camera
			camera->SetCameraBox(camera_box);
		}

		void LevelInstance::OnPlayerEntered(Player * player)
		{
			// early exit
			if (player == nullptr)
				return;

			Level * level = GetTiledLevel();

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
				player_bounding_box = object_surface->GetBoundingBox(true);

			particle_populator.AddParticle(bitmap_name->c_str(), player_bounding_box);
			particle_populator.FlushParticles();

			// allocation
			chaos::ParticleAllocationBase * player_allocation = particle_populator.GetParticleAllocation();

			// set the player allocation
			player->SetPlayerAllocation(player_allocation);

			// XXX : while camera, is restricted so we can see player, we considere that the displacement_ratio of the layer containing the player start is the reference one
			reference_displacement_ratio = layer_instance->displacement_ratio;
			reference_layer = layer_instance;

			// shuxxx : first time FinalizeParticles(...) was called, there was no effect because the PlayerStartLayer has no particle. 
			//          call it twice as a fast fix
			layer_instance->FinalizeParticles();

			// store the current player position as a checkpoint
			death::GameInstance * game_instance = GetGameInstance();
			if (game_instance != nullptr)
				game_instance->SetCheckpointPosition(player_bounding_box.position, this);
		}

		void LevelInstance::OnPlayerLeaved(Player * player)
		{
			if (player == nullptr)
				return;
			player->SetPlayerAllocation(nullptr);
		}

	}; // namespace TiledMap

}; // namespace death
