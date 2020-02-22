#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/LayerInstanceParticlePopulator.h>
#include <death/Player.h>
#include <death/SoundContext.h>

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
		// BaseObject implementation
		// =====================================

		BaseObject::BaseObject(class LayerInstance * in_layer_instance) :
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

        bool GeometricObject::IsParticleCreationEnabled() const 
        { 
            return true; 
        }

		// =====================================
		// TriggerObject implementation
		// =====================================

		TriggerObject::TriggerObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObjectSurface * in_surface_object) :
			GeometricObject(in_layer_instance, in_surface_object)
		{
		}

		bool TriggerObject::Initialize()
		{
			if (!GeometricObject::Initialize())
				return false;
			// default values are set to the one defined by default in constructor
			enabled = geometric_object->FindPropertyBool("ENABLED", enabled);
			trigger_once = geometric_object->FindPropertyBool("TRIGGER_ONCE", trigger_once);
			trigger_id = geometric_object->FindPropertyInt("TRIGGER_ID", trigger_id);
			outside_box_factor = geometric_object->FindPropertyFloat("OUTSIDE_BOX_FACTOR", outside_box_factor);

			// enable the possibility to trigger once again the object
			enter_event_triggered = false;
			return true;
		}

		chaos::box2 TriggerObject::GetBoundingBox(bool world_system) const
		{
			chaos::TiledMap::GeometricObjectSurface * surface = geometric_object->GetObjectSurface();
			if (surface == nullptr)
				return chaos::box2();
			chaos::box2 result = surface->GetBoundingBox(false);  // make our own correction for world system because the LayerInstance can change its offset
			if (world_system)
				result.position += layer_instance->GetLayerOffset();
			return result;
		}

		bool TriggerObject::IsCollisionWith(chaos::box2 const & other_box, std::vector<chaos::weak_ptr<TriggerObject>> const * triggers) const
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

		bool TriggerObject::OnPlayerCollisionEvent(float delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle, chaos::CollisionType event_type)
		{
			return false; // do not do anything with collision
		}

		bool TriggerObject::OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, chaos::CollisionType event_type)
		{
			return false; // do not do anything with collision
		}

		void TriggerObject::SetEnabled(bool in_enabled)
		{
			enabled = in_enabled;
			SetModified();
		}

		void TriggerObject::SetTriggerOnce(bool in_trigger_once)
		{
			trigger_once = in_trigger_once;
			SetModified();
		}

		BaseObjectCheckpoint * TriggerObject::DoCreateCheckpoint() const
		{
			return new TriggerObjectCheckpoint();
		}

		bool TriggerObject::DoSaveIntoCheckpoint(BaseObjectCheckpoint * checkpoint) const
		{
			TriggerObjectCheckpoint * trigger_checkpoint = auto_cast(checkpoint);
			if (trigger_checkpoint == nullptr)
				return false;

			if (!GeometricObject::DoSaveIntoCheckpoint(checkpoint))
				return false;

			trigger_checkpoint->enabled = enabled;
			trigger_checkpoint->trigger_once = trigger_once;
			trigger_checkpoint->enter_event_triggered = enter_event_triggered;

			return true;
		}

		bool TriggerObject::DoLoadFromCheckpoint(BaseObjectCheckpoint const * checkpoint)
		{
			TriggerObjectCheckpoint const * trigger_checkpoint = auto_cast(checkpoint);
			if (trigger_checkpoint == nullptr)
				return false;

			if (!GeometricObject::DoLoadFromCheckpoint(checkpoint))
				return false;

			enabled = trigger_checkpoint->enabled;
			trigger_once = trigger_checkpoint->trigger_once;
			enter_event_triggered = trigger_checkpoint->enter_event_triggered;

			return true;
		}

		// =============================================================
		// CheckPointTriggerObject implementation
		// =============================================================

		bool CheckpointTriggerObject::Initialize()
		{
			if (!TriggerObject::Initialize())
				return false;
			trigger_once = true; // force a trigger once for checkpoint
			return true;
		}

		bool CheckpointTriggerObject::OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, chaos::CollisionType event_type)
		{
			if (event_type != chaos::CollisionType::COLLISION_STARTED)
				return false;

			GameInstance * game_instance = GetLayerInstance()->GetGame()->GetGameInstance();
			if (game_instance != nullptr)
				game_instance->CreateRespawnCheckpoint();

			return true; // collisions handled successfully
		}

		bool CheckpointTriggerObject::IsParticleCreationEnabled() const
		{
			return false;
		}

		// =====================================
		// PlayerStartObject implementation
		// =====================================

		bool PlayerStartObject::Initialize()
		{
			if (!GeometricObject::Initialize())
				return false;
			return true;
		}

		// =================================================
		// NotificationTriggerObject
		// =================================================

		bool NotificationTriggerObject::IsParticleCreationEnabled() const
		{
			return false;
		}

		bool NotificationTriggerObject::Initialize()
		{
			if (!TriggerObject::Initialize())
				return false;

			notification_string      = geometric_object->FindPropertyString("NOTIFICATION", "");
			notification_lifetime    = geometric_object->FindPropertyFloat("LIFETIME", notification_lifetime);
			stop_when_collision_over = geometric_object->FindPropertyBool("STOP_WHEN_COLLISION_OVER", stop_when_collision_over);
			player_collision         = geometric_object->FindPropertyBool("PLAYER_COLLISION", player_collision);

			return true;
		}

		bool NotificationTriggerObject::OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, chaos::CollisionType event_type)
		{
			if (player_collision)
				return false;
			return OnTriggerCollision(delta_time, event_type);
		}

		bool NotificationTriggerObject::OnPlayerCollisionEvent(float delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle, chaos::CollisionType event_type)
		{
			if (!player_collision)
				return false;
			return OnTriggerCollision(delta_time, event_type);
		}

		bool NotificationTriggerObject::OnTriggerCollision(float delta_time, chaos::CollisionType event_type)
		{
			// early exit
			if (event_type != chaos::CollisionType::COLLISION_STARTED && event_type != chaos::CollisionType::COLLISION_FINISHED) // ignore AGAIN event
				return false;
			if (event_type == chaos::CollisionType::COLLISION_FINISHED && !stop_when_collision_over) // ignore FINISHED if you do not want to kill the notification
				return false;
			// get some variables 
			Game * game = layer_instance->GetGame();
			if (game == nullptr)
				return false;			
			death::GameHUD * hud = game->GetCurrentHUD();
			if (hud == nullptr)
				return false;
			GameHUDNotificationComponent * notification_component = hud->FindComponentByClass<GameHUDNotificationComponent>();
			if (notification_component == nullptr)
				return false;
			// show notification
			if (event_type == chaos::CollisionType::COLLISION_STARTED)
				notification_component->ShowNotification(notification_string.c_str(), notification_lifetime);
			// hide notification
			else if (event_type == chaos::CollisionType::COLLISION_FINISHED) // XXX : 'stop_when_collision_over' has already be checked
				notification_component->HideNotification();
			return true;
		}

		// =====================================
		// SoundTriggerObject implementation
		// =====================================

		bool SoundTriggerObject::Initialize()
		{
			if (!TriggerObject::Initialize())
				return false;
			
			sound_name         = geometric_object->FindPropertyString("SOUND_NAME", "");
			min_distance_ratio = geometric_object->FindPropertyFloat("MIN_DISTANCE_RATIO", min_distance_ratio);
			min_distance_ratio = std::clamp(min_distance_ratio, 0.0f, 1.0f);
			
			pause_timer_when_too_far = geometric_object->FindPropertyFloat("PAUSE_TIMER_WHEN_TOO_FAR", pause_timer_when_too_far);
			is_3D_sound              = geometric_object->FindPropertyBool("3D_SOUND", is_3D_sound);
			looping                  = geometric_object->FindPropertyBool("LOOPING", looping);
			stop_when_collision_over   = geometric_object->FindPropertyBool("STOP_WHEN_COLLISION_OVER", stop_when_collision_over);

			return true;
		}

		chaos::Sound * SoundTriggerObject::CreateSound() const
		{
			// early exit
			if (sound_name.length() == 0)
				return nullptr;

			// can only play sounds on point and surface			
			chaos::TiledMap::GeometricObjectSurface * surface_object = geometric_object->GetObjectSurface();
			if (surface_object == nullptr)
				return nullptr;

			// create the sound
			chaos::Sound * result = nullptr;

			Game * game = layer_instance->GetGame();
			if (game != nullptr)
			{
				chaos::box2 box      = surface_object->GetBoundingBox(true);
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

		bool SoundTriggerObject::OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, chaos::CollisionType event_type)
		{
			if (event_type == chaos::CollisionType::COLLISION_STARTED)
			{
				chaos::Sound * new_sound = CreateSound();
				if (stop_when_collision_over)
					sound = new_sound;
				return true;
			}
			if (event_type == chaos::CollisionType::COLLISION_FINISHED)
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

		bool SoundTriggerObject::IsParticleCreationEnabled() const
		{
			return false;
		}

		// =============================================================
		// FinishingTriggerObject implementation
		// =============================================================

		bool FinishingTriggerObject::IsParticleCreationEnabled() const
		{
			return false;
		}

		bool FinishingTriggerObject::OnPlayerCollisionEvent(float delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, chaos::CollisionType event_type)
		{
			if (event_type != chaos::CollisionType::COLLISION_STARTED)
				return false;

			Game * game = layer_instance->GetGame();
			if (game != nullptr)
			{
				GameLevelInstance * level_instance = game->GetLevelInstance();
				if (level_instance != nullptr)
					level_instance->SetLevelCompletionFlag();
			}
			return true;
		}

		// =====================================
		// CameraObject implementation
		// =====================================

		bool CameraObject::Initialize()
		{
			if (!GeometricObject::Initialize())
				return false;
			return true;
		}



		// =====================================
		// BoxScissoringWithRepetitionResult : an utility object to compute instances in 2D of a box that collide a given scissor
		// =====================================

		class BoxScissoringWithRepetitionResult
		{
		public:

			/** constructor */
			BoxScissoringWithRepetitionResult(chaos::box2 const & in_target_box, chaos::box2 const & in_scissor_box, bool in_wrap_x, bool in_wrap_y);

			/** offset of a given instance */
			glm::vec2 GetInstanceOffset(glm::ivec2 const & index) const;

		public:

			/** the 'index' of the first instance to render/collide ... (included) */
			glm::ivec2 start_instance = glm::ivec2(0, 0);
			/** the 'index' of the last instance to render/collide ... (excluded) */
			glm::ivec2 last_instance = glm::ivec2(0, 0);
			/** copy of the construction parameters */
			chaos::box2 target_box;;
			/** copy of the construction parameters */
			chaos::box2 scissor_box;
			/** copy of the construction parameters */
			bool wrap_x = false;
			/** copy of the construction parameters */
			bool wrap_y = false;
		};

		// =====================================
		// BoxScissoringWithRepetitionResult implementation
		// =====================================

		BoxScissoringWithRepetitionResult::BoxScissoringWithRepetitionResult(chaos::box2 const & in_target_box, chaos::box2 const & in_scissor_box, bool in_wrap_x, bool in_wrap_y)
		{
			// copy the parameters
			target_box = in_target_box;
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

			// clamp out all non intersecting box when no WRAP
			if (!wrap_x)
			{
				if ((target_bottomleft.x + target_size.x < scissor_bottomleft.x) || (target_bottomleft.x > scissor_bottomleft.x + scissor_size.x))
				{
					start_instance = last_instance = glm::ivec2(0, 0); // nothing to render at all
					return;
				}
			}

			if (!wrap_y)
			{
				if ((target_bottomleft.y + target_size.y < scissor_bottomleft.y) || (target_bottomleft.y > scissor_bottomleft.y + scissor_size.y))
				{
					start_instance = last_instance = glm::ivec2(0, 0); // nothing to render at all
					return;
				}
			}

			// number of time to decal the layer box, to be directly left of the scissor box
			glm::ivec2 offset_count = glm::ivec2(
				(int)std::ceil((scissor_bottomleft.x - target_bottomleft.x - target_size.x) / target_size.x),
				(int)std::ceil((scissor_bottomleft.y - target_bottomleft.y - target_size.y) / target_size.y)
			);

			// the bottomleft corner of the decaled box
			glm::vec2  virtual_target_bottomleft = target_bottomleft + chaos::RecastVector<glm::vec2>(offset_count) * target_size;
			// competition of the number of repetition
			glm::vec2  tmp = ((scissor_bottomleft - virtual_target_bottomleft + scissor_size) / target_size);

			glm::ivec2 repetition_count = glm::ivec2(
				(int)std::ceil(tmp.x),
				(int)std::ceil(tmp.y)
			);

			// unwrap case, then only visible instance is 0 (we previsously tested for visibility)
			if (!wrap_x)
			{
				offset_count.x = 0;
				repetition_count.x = 1;
			}
			// unwrap case, then only visible instance is 0 (we previsously tested for visibility)
			if (!wrap_y)
			{
				offset_count.y = 0;
				repetition_count.y = 1;
			}

			// finalize the initialization
			start_instance = offset_count;
			last_instance = offset_count + repetition_count;
		}

		glm::vec2 BoxScissoringWithRepetitionResult::GetInstanceOffset(glm::ivec2 const & index) const
		{
			return 2.0f * target_box.half_size * chaos::RecastVector<glm::vec2>(index);
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
			// get the level time	
			level_timeout = in_tiled_map->FindPropertyFloat("LEVEL_TIMEOUT", -1);
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

		chaos::ParticleLayerBase * Level::DoCreateParticleLayer(LayerInstance * layer_instance)
		{
			return new chaos::ParticleLayer<TiledMap::TileParticleTrait>();
		}

		GeometricObject * Level::DoCreateGeometricObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
		{
			chaos::TiledMap::GeometricObjectSurface * surface_object = in_geometric_object->GetObjectSurface();

			if (surface_object != nullptr)
			{
				if (chaos::TiledMapTools::HasFlag(surface_object, "Finish", "Finish", "Finish"))
					return new FinishingTriggerObject(in_layer_instance, surface_object);
				if (chaos::TiledMapTools::HasFlag(surface_object, "Checkpoint", "Checkpoint", "Checkpoint"))
					return new CheckpointTriggerObject(in_layer_instance, surface_object);
				if (chaos::TiledMapTools::HasFlag(surface_object, "Notification", "Notification", "Notification"))
					return new NotificationTriggerObject(in_layer_instance, surface_object);
				if (chaos::TiledMapTools::HasFlag(surface_object, "Sound", "Sound", "Sound"))
					return new SoundTriggerObject(in_layer_instance, surface_object);
			}
			return nullptr;
		}

#define DEATH_DOCREATE_OBJECT(result_type, func_name, declared_parameters, constructor_parameters)\
		result_type * Level::Do##func_name(declared_parameters)\
		{\
			return new result_type(constructor_parameters);\
		}

#define DEATH_CREATE_OBJECT(result_type, func_name, declared_parameters, constructor_parameters)\
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

#define DEATH_CREATE_OBJECT_FULL(result_type, func_name, declared_parameters, constructor_parameters)\
	DEATH_DOCREATE_OBJECT(result_type, func_name, declared_parameters, constructor_parameters)\
	DEATH_CREATE_OBJECT(result_type, func_name, declared_parameters, constructor_parameters)

		DEATH_CREATE_OBJECT(GeometricObject, CreateGeometricObject, LayerInstance * in_layer_instance BOOST_PP_COMMA() chaos::TiledMap::GeometricObject * in_geometric_object, in_layer_instance BOOST_PP_COMMA() in_geometric_object);
		DEATH_CREATE_OBJECT_FULL(CameraObject, CreateCamera, LayerInstance * in_layer_instance BOOST_PP_COMMA() chaos::TiledMap::GeometricObject * in_geometric_object, in_layer_instance BOOST_PP_COMMA() in_geometric_object);
		DEATH_CREATE_OBJECT_FULL(PlayerStartObject, CreatePlayerStart, LayerInstance * in_layer_instance BOOST_PP_COMMA() chaos::TiledMap::GeometricObject * in_geometric_object, in_layer_instance BOOST_PP_COMMA() in_geometric_object);
		DEATH_CREATE_OBJECT_FULL(LayerInstance, CreateLayerInstance, LevelInstance * in_level_instance BOOST_PP_COMMA() chaos::TiledMap::LayerBase * in_layer, in_level_instance BOOST_PP_COMMA() in_layer);

#undef DEATH_DOCREATE_OBJECT
#undef DEATH_CREATE_OBJECT
#undef DEATH_CREATE_OBJECT_FULL

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
			uniform vec4 camera_box;

			void main()
			{
				vec2 pos = position + offset;

				vs_position = pos;
				vs_texcoord = texcoord;
				vs_color = color;

				vec4 transformed_pos = camera_transform * vec4(pos.x, pos.y, 0.0, 1.0);

				gl_Position.xy = transformed_pos.xy / camera_box.zw;
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

		bool Level::OnPlayerTileCollision(float delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle, TileParticle * particle)
		{
			return true; // continue with other
		}

		// =====================================
		// LayerInstance implementation
		// =====================================

		LayerInstance::LayerInstance(LevelInstance * in_level_instance, chaos::TiledMap::LayerBase * in_layer) :
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

			triggers_enabled = layer->FindPropertyBool("TRIGGERS_ENABLED", false);
			player_collision_enabled = layer->FindPropertyBool("PLAYER_COLLISIONS_ENABLED", false);
			camera_collision_enabled = layer->FindPropertyBool("CAMERA_COLLISIONS_ENABLED", false);
			tile_collisions_enabled = layer->FindPropertyBool("TILE_COLLISIONS_ENABLED", false);

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
				if (!InitializeImageLayer(image_layer))
					return false;
				return FinalizeParticles();
			}

			chaos::TiledMap::ObjectLayer * object_layer = layer->GetObjectLayer();
			if (object_layer != nullptr)
			{
				if (!InitializeObjectLayer(object_layer))
					return false;
				return FinalizeParticles();
			}

			chaos::TiledMap::TileLayer * tile_layer = layer->GetTileLayer();
			if (tile_layer != nullptr)
			{
				if (!InitializeTileLayer(tile_layer))
					return false;
				return FinalizeParticles();
			}

			return false;
		}

		bool LayerInstance::InitializeImageLayer(chaos::TiledMap::ImageLayer * image_layer)
		{
			return true;
		}

		void LayerInstance::CreateGeometricObjectParticles(chaos::TiledMap::GeometricObject * geometric_object, GeometricObject * object, LayerInstanceParticlePopulator * particle_populator)
		{
			chaos::TiledMap::Map * tiled_map = level_instance->GetTiledMap();

			// create additionnal particles (TEXT)
			chaos::TiledMap::GeometricObjectText * text = geometric_object->GetObjectText();
			if (text != nullptr)
			{
				// create particle layer if necessary
				if (particle_layer == nullptr)
					if (CreateParticleLayer() == nullptr) // the generate layer is of  TileParticleTrait => this works fine for Text (except a useless GID per particle)
						return;

				Game * game = GetGame();

				chaos::ParticleTextGenerator::GeneratorResult result;
				chaos::ParticleTextGenerator::GeneratorParams params;

				params.line_height = (float)text->pixelsize;
				params.hotpoint_type = chaos::HotpointType::TOP;
				params.position = text->position;
				params.default_color = text->color;

				std::string const * font_name = text->FindPropertyString("FONT_NAME");
				if (font_name != nullptr)
					params.font_info_name = *font_name;

				// create particles
				game->GetTextGenerator()->Generate(text->text.c_str(), result, params);
				chaos::ParticleTextGenerator::CreateTextAllocation(particle_layer.get(), result);
			}

			// create additionnal particles (TILES)
			chaos::TiledMap::GeometricObjectTile * tile = geometric_object->GetObjectTile();
			if (tile != nullptr)
			{
				int gid = tile->gid;



                // shuludum ?

				// shu FindProperty



				// search the tile information 
				chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
				if (tile_info.tiledata == nullptr)
					return;
				// aspect ratio is to be maintained ??

				bool keep_aspect_ratio = tile->FindPropertyBool("KEEP_ASPECT_RATIO", true);
#if 0

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
#endif











				// create a simple particle
				chaos::box2 particle_box = tile->GetBoundingBox(true);
				if (object != nullptr)
				{
					chaos::TiledMap::GeometricObjectSurface const * geometric = geometric_object->GetObjectSurface();
					if (geometric != nullptr)
					{
						keep_aspect_ratio = false;
						particle_box = geometric->GetBoundingBox(false); // shuxxx : the TILE is generated on the same layer then the surface. does it get the layer_offset ????
					}
				}
				particle_populator->AddParticle(tile_info.tiledata->atlas_key.c_str(), particle_box, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), gid, tile->horizontal_flip, tile->vertical_flip, keep_aspect_ratio);
			}
		}


		GeometricObject * LayerInstance::CreateObjectInstance(chaos::TiledMap::GeometricObject * geometric_object)
		{
			Level * level = GetTiledLevel();

			// player start 
			if (chaos::TiledMapTools::IsPlayerStart(geometric_object))
			{
				PlayerStartObject * player_start = level->CreatePlayerStart(this, geometric_object);
				if (player_start != nullptr)
					player_starts.push_back(player_start);
				return player_start;
			}

			// camera 
			if (chaos::TiledMapTools::IsCamera(geometric_object))
			{
				CameraObject * camera = level->CreateCamera(this, geometric_object);
				if (camera != nullptr)
					cameras.push_back(camera);
				return camera;
			}

			// other type of object 
			GeometricObject * object = level->CreateGeometricObject(this, geometric_object);
			if (object != nullptr)
			{
				TriggerObject * trigger = auto_cast(object);
				if (trigger != nullptr)
					triggers.push_back(trigger);
				else
					geometric_objects.push_back(object);
				return object;
			}
			return nullptr;
		}

        LayerInstanceParticlePopulator* LayerInstance::CreateParticlePopulator()
        {
            return new LayerInstanceParticlePopulator();
        }

		bool LayerInstance::InitializeObjectLayer(chaos::TiledMap::ObjectLayer * object_layer)
		{
			// search the bounding box (explicit or not)
			chaos::box2 box;
			chaos::box2 explicit_bounding_box;

			// the particle generator
            chaos::shared_ptr<LayerInstanceParticlePopulator> particle_populator = CreateParticlePopulator();
			if (!particle_populator->Initialize(this))
				return false;

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

				// create the object
				GeometricObject * object = CreateObjectInstance(geometric_object);
                if (object != nullptr)
                {
#if _DEBUG
                    if (!chaos::Application::HasApplicationCommandLineFlag("-TiledGeometricObject::ForceParticleCreation")) // CMDLINE
#endif
                    {
                        if (!object->GetGeometricObject()->FindPropertyBool("PARTICLE_CREATION", object->IsParticleCreationEnabled()))
                            continue;
                    }
                }
                CreateGeometricObjectParticles(geometric_object, object, particle_populator.get());
			}

			// final flush
            particle_populator->FlushParticles();
			// update the bounding box
			if (!IsGeometryEmpty(explicit_bounding_box))
				bounding_box = explicit_bounding_box;
			else
				bounding_box = box | particle_populator->GetBoundingBox();

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
			for (size_t i = 0; i < allocation_count; ++i)
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
			std::string const * enabled_renderpasses = layer->FindPropertyString("ENABLED_RENDERPASSES");
			if (enabled_renderpasses != nullptr)
				in_particle_layer->AddEnabledRenderPasses(enabled_renderpasses->c_str());
			// disabled renderpasses
			std::string const * disabled_renderpasses = layer->FindPropertyString("DISABLED_RENDERPASSES");
			if (disabled_renderpasses != nullptr)
				in_particle_layer->AddDisabledRenderPasses(disabled_renderpasses->c_str());
			return true;
		}

		chaos::ParticleAllocationBase * LayerInstance::SpawnParticles(size_t count)
		{
			// create particle layer if necessary
            if (CreateParticleLayer() == nullptr)
				return nullptr;
			// create the allocation
			return particle_layer->SpawnParticles(count);
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
				particle_layer = GetTiledLevel()->DoCreateParticleLayer(this);
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

		bool LayerInstance::InitializeTileLayer(chaos::TiledMap::TileLayer * tile_layer)
		{
			Level * level = GetTiledLevel();

			// early exit for empty tile_layer
			size_t count = tile_layer->tile_indices.size();
			if (count == 0)
				return false;

            chaos::shared_ptr<LayerInstanceParticlePopulator> particle_populator = CreateParticlePopulator();
			if (!particle_populator->Initialize(this))
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
				glm::ivec2  tile_coord = tile_layer->GetTileCoordinate(i);
				chaos::box2 particle_box = tile_layer->GetTileBoundingBox(tile_coord, tile_info.tiledata->image_size, false);

				bool horizontal_flip = false;
				bool vertical_flip = false;
                particle_populator->AddParticle(tile_info.tiledata->atlas_key.c_str(), particle_box, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), gid, horizontal_flip, vertical_flip);
			}

			// final flush
            particle_populator->FlushParticles();
			// update the bounding box
			bounding_box = particle_populator->GetBoundingBox();

			return true;
		}

		void LayerInstance::ComputePlayerAndCameraCollision(float delta_time)
		{
			// get the game
			Game * game = GetGame();
			if (game == nullptr)
				return;

			// early exit
			bool trigger_collision_enabled = AreTriggersEnabled();
			bool tile_collision_enabled    = AreTileCollisionsEnabled();
			if (!trigger_collision_enabled && !tile_collision_enabled)
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
					if (trigger_collision_enabled)
						if (!ComputePlayerCollisionWithSurfaceTriggers(delta_time, player, player_particle))
							continue;
					// collision with tiles
					if (tile_collision_enabled)
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
					if (trigger_collision_enabled)
						ComputeCameraCollisionWithSurfaceTriggers(delta_time, camera_box);
				}
			}
		}

		PlayerAndTriggerCollisionRecord * LayerInstance::FindPlayerCollisionRecord(Player * player)
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


		bool LayerInstance::ComputeCameraCollisionWithSurfaceTriggers(float delta_time, chaos::box2 const & camera_box)
		{
			// the new colliding triggers
			std::vector<chaos::weak_ptr<TriggerObject>> new_triggers;

			// search all colliding triggers
			size_t triggers_count = triggers.size();
			for (size_t i = 0; i < triggers_count; ++i)
			{
				TriggerObject * trigger = triggers[i].get();
				if (trigger == nullptr || !trigger->IsEnabled())
					continue;
				// detect collision
				if (trigger->IsCollisionWith(camera_box, &camera_collision_records))
					new_triggers.push_back(trigger);
			}

			// triggers collisions 
			size_t new_triggers_count = new_triggers.size();
			for (size_t i = 0; i < new_triggers_count; ++i)
			{
				TriggerObject * trigger = new_triggers[i].get();

				// search in previous frame data	
				chaos::CollisionType collision_type = chaos::CollisionType::COLLISION_STARTED;
				if (std::find(camera_collision_records.begin(), camera_collision_records.end(), trigger) != camera_collision_records.end()) 
					collision_type = chaos::CollisionType::COLLISION_AGAIN;

				// trigger once : do not trigger anymore entering events
				if (collision_type == chaos::CollisionType::COLLISION_STARTED && trigger->IsTriggerOnce() && trigger->enter_event_triggered)
					continue;
				// trigger event
				if (trigger->OnCameraCollisionEvent(delta_time, camera_box, collision_type))
				{
					if (collision_type == chaos::CollisionType::COLLISION_STARTED && trigger->IsTriggerOnce())
					{
						trigger->enter_event_triggered = true;
						trigger->SetModified();
					}
				}
			}

			// triggers end of collisions
			size_t previous_count = camera_collision_records.size();
			for (size_t i = 0; i < previous_count; ++i)
			{
				if (std::find(new_triggers.begin(), new_triggers.end(), camera_collision_records[i]) == new_triggers.end()) // no more colliding
					camera_collision_records[i]->OnCameraCollisionEvent(delta_time, camera_box, chaos::CollisionType::COLLISION_FINISHED);
			}

			// store the new triggers
			camera_collision_records = std::move(new_triggers);

			return true;

		}

		bool LayerInstance::ComputePlayerCollisionWithSurfaceTriggers(float delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle)
		{
			// the new colliding triggers
			std::vector<chaos::weak_ptr<TriggerObject>> new_triggers;
			// the previous colliding triggers
			PlayerAndTriggerCollisionRecord * previous_collisions = FindPlayerCollisionRecord(player);

			// search all colliding triggers
			size_t triggers_count = triggers.size();
			for (size_t i = 0; i < triggers_count; ++i)
			{
				TriggerObject * trigger = triggers[i].get();
				if (trigger == nullptr || !trigger->IsEnabled())
					continue;
				// detect collision
				if (trigger->IsCollisionWith(player_particle->bounding_box, (previous_collisions != nullptr) ? &previous_collisions->triggers : nullptr))
					new_triggers.push_back(trigger);
			}

			// triggers collisions 
			size_t new_triggers_count = new_triggers.size();
			for (size_t i = 0; i < new_triggers_count; ++i)
			{
				TriggerObject * trigger = new_triggers[i].get();

				// search in previous frame data
				chaos::CollisionType collision_type = chaos::CollisionType::COLLISION_STARTED;
				if (previous_collisions != nullptr)
					if (std::find(previous_collisions->triggers.begin(), previous_collisions->triggers.end(), trigger) != previous_collisions->triggers.end())
						collision_type = chaos::CollisionType::COLLISION_AGAIN;
				// trigger once : do not trigger anymore entering events
				if (collision_type == chaos::CollisionType::COLLISION_STARTED && trigger->IsTriggerOnce() && trigger->enter_event_triggered)
					continue;
				// trigger event
				if (trigger->OnPlayerCollisionEvent(delta_time, player, player_particle, collision_type))
				{
					if (collision_type == chaos::CollisionType::COLLISION_STARTED && trigger->IsTriggerOnce())
					{
						trigger->enter_event_triggered = true;
						trigger->SetModified();
					}
				}
			}

			// triggers end of collisions
			if (previous_collisions != nullptr)
			{
				size_t previous_count = previous_collisions->triggers.size();
				for (size_t i = 0; i < previous_count; ++i)
				{
					if (std::find(new_triggers.begin(), new_triggers.end(), previous_collisions->triggers[i]) == new_triggers.end()) // no more colliding
						previous_collisions->triggers[i]->OnPlayerCollisionEvent(delta_time, player, player_particle, chaos::CollisionType::COLLISION_FINISHED);
				}
			}

			// store the record
			if (previous_collisions != nullptr)
				previous_collisions->triggers = std::move(new_triggers);
			else
			{
				PlayerAndTriggerCollisionRecord new_record;
				new_record.player = player;
				new_record.triggers = std::move(new_triggers);
				collision_records.push_back(std::move(new_record));
			}
			return true; // continue other collisions 
		}

		bool LayerInstance::ComputePlayerTileCollisions(float delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle)
		{
			TiledMap::Level * level = GetTiledLevel();

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

		bool LayerInstance::DoTick(float delta_time)
		{
			// tick the game objects
			size_t player_start_count = player_starts.size();
			for (size_t i = 0; i < player_start_count; ++i)
				player_starts[i]->Tick(delta_time);

			size_t camera_count = cameras.size();
			for (size_t i = 0; i < camera_count; ++i)
				cameras[i]->Tick(delta_time);

			size_t trigger_count = triggers.size();
			for (size_t i = 0; i < trigger_count; ++i)
				triggers[i]->Tick(delta_time);

			size_t geometric_count = geometric_objects.size();
			for (size_t i = 0; i < geometric_count; ++i)
				geometric_objects[i]->Tick(delta_time);

			// tick the particles
			if (particle_layer != nullptr)
				particle_layer->Tick(delta_time);
			return true;
		}

		int LayerInstance::DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params)
		{
			// early exit
			int result = 0;
			if (particle_layer == nullptr)
				return result;

			// camera is expressed in world, so is for layer
			chaos::obox2 camera_obox = GetTiledLevelInstance()->GetCameraOBox(0);
			chaos::obox2 initial_camera_obox = GetTiledLevelInstance()->GetInitialCameraOBox(0);


			// XXX : we want some layers to appear further or more near the camera
			//       the displacement_ratio represent how fast this layer is moving relatively to other layers.
			//       The reference layer is the layer where the 'effective' camera (and so the PlayerStart is)
			//         => when player goes outside the screen, the camera is updated so that it is still watching the player
			//         => that why we consider the PlayerStart's layer as the reference
			//       to simulate other layer's speed, we just create at rendering time 'virtual cameras' (here this is 'final_camera_box')
			//       We only multiply 'true camera' distance from its initial position by a ratio value

			// apply the displacement to the camera

			glm::vec2 final_ratio = glm::vec2(1.0f, 1.0f);

			LayerInstance const * reference_layer = level_instance->reference_layer.get();
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
			final_camera_obox.position  = initial_camera_obox.position  + (camera_obox.position - initial_camera_obox.position) * final_ratio;
			final_camera_obox.half_size = initial_camera_obox.half_size + (camera_obox.half_size - initial_camera_obox.half_size) * final_ratio;

			// compute repetitions
			BoxScissoringWithRepetitionResult scissor_result = BoxScissoringWithRepetitionResult(layer_box, chaos::GetBoundingBox(final_camera_obox), wrap_x, wrap_y);

			// new provider for camera override (will be fullfill only if necessary)
			chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
			main_uniform_provider.AddVariableValue("camera_transform", CameraTransform::GetCameraTransform(final_camera_obox));

			chaos::box2 final_camera_box;
			final_camera_box.position = final_camera_obox.position;
			final_camera_box.half_size = final_camera_obox.half_size;
			main_uniform_provider.AddVariableValue("camera_box", chaos::EncodeBoxToVector(final_camera_box));


			// HACK : due to bad LAYER_BOUNDING_BOX computation, the layer containing PLAYER_START may be clamped and layer hidden
			glm::ivec2 start_instance = scissor_result.start_instance;
			glm::ivec2 last_instance = scissor_result.last_instance;
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
					glm::vec2 instance_offset = scissor_result.GetInstanceOffset(glm::ivec2(x, y));
					instance_uniform_provider.AddVariableValue("offset", instance_offset + offset);
					// draw call
					result += particle_layer->Display(renderer, &instance_uniform_provider, render_params);
				}
			}
			return result;
		}

#define DEATH_FIND_OBJECT(result_type, func_name, member_vector, constness)\
		result_type constness * LayerInstance::func_name(chaos::NamedObjectRequest request) constness\
		{\
			if ((request.use_name && request.name == nullptr) && member_vector.size() > 0)\
				return member_vector[0].get();\
			return NamedObject::FindNamedObject(member_vector, request);\
		}
		DEATH_FIND_OBJECT(GeometricObject, FindGeometricObject, geometric_objects, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(GeometricObject, FindGeometricObject, geometric_objects, const);
		DEATH_FIND_OBJECT(TriggerObject, FindTrigger, triggers, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(TriggerObject, FindTrigger, triggers, const);
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, player_starts, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, player_starts, const);
		DEATH_FIND_OBJECT(CameraObject, FindCamera, cameras, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(CameraObject, FindCamera, cameras, const);

#undef DEATH_FIND_OBJECT


		size_t LayerInstance::GetTriggerCount() const
		{
			return triggers.size();
		}

		TriggerObject * LayerInstance::GetTrigger(size_t index)
		{
			if (index >= triggers.size())
				return nullptr;
			return triggers[index].get();
		}

		TriggerObject const * LayerInstance::GetTrigger(size_t index) const
		{
			if (index >= triggers.size())
				return nullptr;
			return triggers[index].get();
		}


		TiledLayerCheckpoint * LayerInstance::DoCreateCheckpoint() const
		{
			return new TiledLayerCheckpoint();
		}

		template<typename ELEMENT_VECTOR, typename CHECKPOINT_VECTOR>
		bool LayerInstance::DoSaveIntoCheckpointHelper(ELEMENT_VECTOR const & elements, CHECKPOINT_VECTOR & checkpoints) const
		{
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
			{
				// object in death::TiledMap point of view
				auto const * obj = elements[i].get();
				if (obj == nullptr || !obj->IsModified()) // only modified objects
					continue;
				// object for chaos point of view
				chaos::TiledMap::GeometricObject * geometric_object = obj->geometric_object.get();
				if (geometric_object == nullptr || geometric_object->GetObjectID() < 0)
					continue;
				// save the checkpoint
				BaseObjectCheckpoint * checkpoint = obj->SaveIntoCheckpoint();
				if (checkpoint == nullptr)
					continue;
				checkpoints[geometric_object->GetObjectID()] = checkpoint;
			}
			return true;
		}

		bool LayerInstance::DoSaveIntoCheckpoint(TiledLayerCheckpoint * checkpoint) const
		{
			DoSaveIntoCheckpointHelper(triggers, checkpoint->trigger_checkpoints);
			DoSaveIntoCheckpointHelper(geometric_objects, checkpoint->object_checkpoints);
			return true;
		}

		template<typename ELEMENT_VECTOR, typename CHECKPOINT_VECTOR>
		bool LayerInstance::DoLoadFromCheckpointHelper(ELEMENT_VECTOR & elements, CHECKPOINT_VECTOR const & checkpoints)
		{
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
			{
				// object in death::TiledMap point of view
				auto * obj = elements[i].get();
				if (obj == nullptr)
					continue;
				// object for chaos point of view
				chaos::TiledMap::GeometricObject * geometric_object = obj->geometric_object.get();
				if (geometric_object == nullptr || geometric_object->GetObjectID() < 0)
					continue;
				// get checkpoint
				BaseObjectCheckpoint * obj_checkpoint = nullptr;

				auto it = checkpoints.find(geometric_object->GetObjectID());
				if (it != checkpoints.end())
					obj_checkpoint = it->second.get();

				// -checkpoint found    => use it
				// -no checkpoint point =>
				//    -> object is currently modified, restore initial settings
				if (obj_checkpoint != nullptr)
					obj->LoadFromCheckpoint(obj_checkpoint);
				else if (obj->IsModified())
					obj->Initialize();
			}
			return true;
		}

		bool LayerInstance::DoLoadFromCheckpoint(TiledLayerCheckpoint const * checkpoint)
		{
			DoLoadFromCheckpointHelper(triggers, checkpoint->trigger_checkpoints);
			DoLoadFromCheckpointHelper(geometric_objects, checkpoint->object_checkpoints);
			return true;
		}

		void LayerInstance::OnLevelEnded()
		{
			size_t player_start_count = player_starts.size();
			for (size_t i = 0; i < player_start_count; ++i)
				player_starts[i]->OnLevelEnded();

			size_t camera_count = cameras.size();
			for (size_t i = 0; i < camera_count; ++i)
				cameras[i]->OnLevelEnded();

			size_t trigger_count = triggers.size();
			for (size_t i = 0; i < trigger_count; ++i)
				triggers[i]->OnLevelEnded();

			size_t object_count = geometric_objects.size();
			for (size_t i = 0; i < object_count; ++i)
				geometric_objects[i]->OnLevelEnded();
		}

		void LayerInstance::OnLevelStarted()
		{
			size_t player_start_count = player_starts.size();
			for (size_t i = 0; i < player_start_count; ++i)
				player_starts[i]->OnLevelStarted();

			size_t camera_count = cameras.size();
			for (size_t i = 0; i < camera_count; ++i)
				cameras[i]->OnLevelStarted();

			size_t trigger_count = triggers.size();
			for (size_t i = 0; i < trigger_count; ++i)
				triggers[i]->OnLevelStarted();

			size_t object_count = geometric_objects.size();
			for (size_t i = 0; i < object_count; ++i)
				geometric_objects[i]->OnLevelStarted();
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

		void LevelInstance::ComputePlayerAndCameraCollision(float delta_time)
		{
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				layer_instances[i]->ComputePlayerAndCameraCollision(delta_time);
		}

		bool LevelInstance::DoTick(float delta_time)
		{
			GameLevelInstance::DoTick(delta_time);

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

		int LevelInstance::DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params)
		{
			int result = 0;

			// display particle manager
			if (particle_manager != nullptr)
				result += particle_manager->Display(renderer, uniform_provider, render_params);
			// draw the layer instances0
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
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
			for (size_t i = 0; i < count; ++i)
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
		result_type constness * LevelInstance::func_name(chaos::NamedObjectRequest request) constness\
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
		DEATH_FIND_OBJECT(GeometricObject, FindGeometricObject, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(GeometricObject, FindGeometricObject, const);
		DEATH_FIND_OBJECT(TriggerObject, FindTrigger, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(TriggerObject, FindTrigger, const);
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(PlayerStartObject, FindPlayerStart, const);
		DEATH_FIND_OBJECT(CameraObject, FindCamera, BOOST_PP_EMPTY());
		DEATH_FIND_OBJECT(CameraObject, FindCamera, const);

#undef DEATH_FIND_OBJECT

		LayerInstance * LevelInstance::FindLayerInstance(chaos::NamedObjectRequest request)
		{
			if ((request.use_name && request.name == nullptr) && layer_instances.size() > 0)
				return layer_instances[0].get();
			return NamedObject::FindNamedObject(layer_instances, request);
		}
		LayerInstance const * LevelInstance::FindLayerInstance(chaos::NamedObjectRequest request) const
		{
			if ((request.use_name && request.name == nullptr) && layer_instances.size() > 0)
				return layer_instances[0].get();
			return NamedObject::FindNamedObject(layer_instances, request);
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

			// get the aspect ratio
			float aspect_ratio = 16.0f / 9.0f;

			Game const * game = GetGame();
			if (game != nullptr)
				aspect_ratio = game->GetViewportWantedAspect();

			// compute the surface
			chaos::TiledMap::GeometricObjectSurface * camera_surface = camera_object->GetGeometricObject()->GetObjectSurface();
			if (camera_surface == nullptr)
				return;
			chaos::box2 camera_box = chaos::AlterBoxToAspect(camera_surface->GetBoundingBox(true), aspect_ratio, true);

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
            chaos::shared_ptr<LayerInstanceParticlePopulator> particle_populator = layer_instance->CreateParticlePopulator();
			if (!particle_populator->Initialize(layer_instance))
				return;

			// compute the bounding box
			chaos::box2 player_bounding_box;
			player_bounding_box.position = player_start->GetGeometricObject()->position;

			chaos::TiledMap::GeometricObjectSurface const * object_surface = player_start->GetGeometricObject()->GetObjectSurface();
			if (object_surface != nullptr)
				player_bounding_box = object_surface->GetBoundingBox(true);


			// shuludum


            particle_populator->AddParticle(bitmap_name->c_str(), player_bounding_box);
            particle_populator->FlushParticles();

			// allocation
			chaos::ParticleAllocationBase * player_allocation = particle_populator->GetParticleAllocation();

			// set the player allocation
			player->SetPlayerAllocation(player_allocation);

			// XXX : while camera, is restricted so we can see player, we considere that the displacement_ratio of the layer containing the player start is the reference one
			reference_layer = layer_instance;

			// shuxxx : first time FinalizeParticles(...) was called, there was no effect because the PlayerStartLayer has no particle. 
			//          call it twice as a fast fix
			layer_instance->FinalizeParticles();
		}

		void LevelInstance::OnPlayerLeaved(Player * player)
		{
			if (player == nullptr)
				return;
			player->SetPlayerAllocation(nullptr);
		}

		void LevelInstance::CreateBackgroundImage()
		{
			std::string const * background_material = nullptr;
			std::string const * background_texture = nullptr;

			TiledMap::Level const * level = GetTiledLevel();
			if (level != nullptr)
			{
				background_material = level->GetTiledMap()->FindPropertyString("BACKGROUND_MATERIAL");
				background_texture = level->GetTiledMap()->FindPropertyString("BACKGROUND_TEXTURE");
			}

			game->CreateBackgroundImage(
				(background_material == nullptr) ? nullptr : background_material->c_str(),
				(background_texture == nullptr) ? nullptr : background_texture->c_str());
		}

		void LevelInstance::SetInGameMusic()
		{
			std::string const * level_music = nullptr;

			TiledMap::Level const * level = GetTiledLevel();
			if (level != nullptr)
				level_music = level->GetTiledMap()->FindPropertyString("MUSIC");

			if (level_music == nullptr)
				GameLevelInstance::SetInGameMusic();
			else
				game->SetInGameMusic(level_music->c_str());
		}

		LevelCheckpoint * LevelInstance::DoCreateCheckpoint() const
		{
			return new TiledLevelCheckpoint();
		}

		bool LevelInstance::DoSaveIntoCheckpoint(LevelCheckpoint * checkpoint) const
		{
			TiledLevelCheckpoint * tiled_level_checkpoint = auto_cast(checkpoint);
			if (tiled_level_checkpoint == nullptr)
				return false;

			if (!GameLevelInstance::DoSaveIntoCheckpoint(checkpoint))
				return false;

			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
			{
				// the layer (death::TiledMap point of view)
				LayerInstance const * layer = layer_instances[i].get();
				if (layer == nullptr)
					continue;
				// the layer (chaos point of view). If must have an 
				chaos::TiledMap::LayerBase const * base_layer = layer->layer.get();
				if (base_layer == nullptr || base_layer->GetObjectID() < 0)
					continue;
				// create the checkpoint 
				TiledLayerCheckpoint * layer_checkpoint = layer_instances[i]->SaveIntoCheckpoint();
				if (layer_checkpoint == nullptr)
					continue;
				// insert the layer_checkpoint in level_checkpoint
				tiled_level_checkpoint->layer_checkpoints[base_layer->GetObjectID()] = layer_checkpoint;
			}

			return true;
		}

		bool LevelInstance::DoLoadFromCheckpoint(LevelCheckpoint const * checkpoint)
		{
			TiledLevelCheckpoint const * tiled_level_checkpoint = auto_cast(checkpoint);
			if (tiled_level_checkpoint == nullptr)
				return false;

			// super method
			if (!GameLevelInstance::DoLoadFromCheckpoint(checkpoint))
				return false;

			// iterate over layers that have serialized a checkpoint
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
			{
				// the layer (death::TiledMap point of view)
				LayerInstance * layer = layer_instances[i].get();
				if (layer == nullptr)
					continue;
				// the layer (chaos point of view). If must have an 
				chaos::TiledMap::LayerBase * base_layer = layer->layer.get();
				if (base_layer == nullptr || base_layer->GetObjectID() < 0)
					continue;
				// find the corresponding checkpoint
				auto it = tiled_level_checkpoint->layer_checkpoints.find(base_layer->GetObjectID());
				if (it == tiled_level_checkpoint->layer_checkpoints.end())
					continue;
				// load layer_chackpoint
				layer->LoadFromCheckpoint(it->second.get());
			}
			return true;
		}

		void LevelInstance::OnLevelEnded()
		{
			GameLevelInstance::OnLevelEnded();

			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				layer_instances[i]->OnLevelEnded();
		}

		void LevelInstance::OnLevelStarted()
		{
			GameLevelInstance::OnLevelStarted();

			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				layer_instances[i]->OnLevelStarted();
		}

	}; // namespace TiledMap

}; // namespace death
