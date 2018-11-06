#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <chaos/TiledMapTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GPUProgramGenerator.h>

namespace death
{
	namespace TiledMap
	{

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
			return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<chaos::ParticleDefault::ParticleTrait>);
		}

		void Level::FlushParticles(TileParticleData const * particles, size_t count, chaos::ParticleAllocation * allocation, LayerInstance * layer_instance)
		{
			size_t old_count = allocation->GetParticleCount();
			// early exit
			if (count == 0)
				return;
			// reserve memory
			allocation->AddParticles(count);
			// an accessor to flush
			chaos::ParticleAccessor<chaos::ParticleDefault::Particle> p = allocation->GetParticleAccessor<chaos::ParticleDefault::Particle>();
			for (size_t i = 0; i < count; ++i)
				p[old_count + i] = particles[i].particle;
		}


#define DEATH_EMPTY_TOKEN
#define DEATH_DOCREATE_OBJECT(result_type, func_name, declared_parameters, calling_parameters)\
		result_type * Level::Do##func_name(declared_parameters)\
		{\
			return new result_type(calling_parameters);\
		}
#define DEATH_CREATE_OBJECT(result_type, func_name, declared_parameters, constructor_parameters, initialization_parameters)\
		result_type * Level::func_name(declared_parameters)\
		{\
			result_type * result = Do##func_name(constructor_parameters);\
			if (result == nullptr)\
				return nullptr;\
			if (!result->Initialize(initialization_parameters))\
			{\
				delete result;\
				return nullptr;\
			}\
			return result;\
		}

		DEATH_DOCREATE_OBJECT(PlayerStartObject, CreatePlayerStart, LayerInstance * in_layer_instance, in_layer_instance);
		DEATH_CREATE_OBJECT(PlayerStartObject, CreatePlayerStart, LayerInstance * in_layer_instance BOOST_PP_COMMA() chaos::TiledMap::GeometricObject * in_geometric_object, in_layer_instance, in_geometric_object);

		DEATH_DOCREATE_OBJECT(LayerInstance, CreateLayerInstance, LevelInstance * in_level_instance, in_level_instance);
		DEATH_CREATE_OBJECT(LayerInstance, CreateLayerInstance, LevelInstance * in_level_instance BOOST_PP_COMMA() chaos::TiledMap::LayerBase * in_layer, in_level_instance, in_layer);

#undef DEATH_CREATE_OBJECT
#undef DEATH_DOCREATE_OBJECT
#undef DEATH_EMPTY_TOKEN

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

			void main() 
			{
				vs_position = position;
				vs_texcoord = texcoord;
				vs_color    = color;

				gl_Position.xy = (position - camera_box.xy) / camera_box.zw;
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
			boost::intrusive_ptr<chaos::GPUProgram> program = GenDefaultRenderProgram(); // store a temporary object for lifetime management
			return chaos::GPURenderMaterial::GenRenderMaterialObject(program.get());
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
		// PlayerStartObject implementation
		// =====================================

		PlayerStartObject::PlayerStartObject(class LayerInstance * in_layer_instance) :
			BaseObject(in_layer_instance)
		{
		}

		bool PlayerStartObject::Initialize(chaos::TiledMap::GeometricObject * in_geometric_object)
		{
			// copy the reference of the geometric object
			geometric_object = in_geometric_object;
			// store the name/id
			name = geometric_object->name;

			return true;
		}

		// =====================================
		// LayerInstance implementation
		// =====================================

		LayerInstance::LayerInstance(LevelInstance * in_level_instance):
			level_instance(in_level_instance)
		{
			assert(in_level_instance != nullptr);
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

		bool LayerInstance::Initialize(chaos::TiledMap::LayerBase * in_layer)
		{
			// store the layer
			layer = in_layer;

			// get the properties of interrest
			displacement_ratio = in_layer->FindPropertyFloat("DISPLACEMENT_FACTOR", 1.0f);
			wrap_x = in_layer->FindPropertyBool("WRAP_X", false);
			wrap_y = in_layer->FindPropertyBool("WRAP_Y", false);
			material_name = in_layer->FindPropertyString("MATERIAL", "");

			// empty the bounding box
			bounding_box = chaos::box2();

			// special initialization
			chaos::TiledMap::ImageLayer * image_layer = in_layer->GetImageLayer();
			if (image_layer != nullptr)
				return InitializeLayer(image_layer);

			chaos::TiledMap::ObjectLayer * object_layer = in_layer->GetObjectLayer();
			if (object_layer != nullptr)
				return InitializeLayer(object_layer);

			chaos::TiledMap::TileLayer * tile_layer = in_layer->GetTileLayer();
			if (tile_layer != nullptr)
				return InitializeLayer(tile_layer);

			return false;
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::ImageLayer * image_layer)
		{

			return true;
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::ObjectLayer * object_layer)
		{
			Level * level = GetTypedLevel();

			size_t count = object_layer->geometric_objects.size();
			for (size_t i = 0; i < count; ++i)
			{
				chaos::TiledMap::GeometricObject * geometric_object = object_layer->geometric_objects[i].get();
				if (geometric_object == nullptr)
					continue;
				
				// player start ?
				if (chaos::TiledMapTools::IsPlayerStart(geometric_object))
				{
					PlayerStartObject * player_start = level->CreatePlayerStart(this, geometric_object);
					if (player_start != nullptr)
						player_starts.push_back(player_start);
				}
#if 0
				// zones
				chaos::TiledMap::GeometricObjectRectangle * rectangle = geometric_object->GetObjectRectangle();
				chaos::TiledMap::GeometricObjectEllipse   * ellipse   = geometric_object->GetObjectEllipse();
				if (rectangle != nullptr || ellipse != nullptr)
				{
					if (rectangle != nullptr)
					{
						chaos::box2 bounding_box = rectangle->GetBoundingBox();

						auto corners = bounding_box.GetCorners();
						rectangle = rectangle;
					}

					if (ellipse != nullptr)
					{
						chaos::box2 bounding_box = ellipse->GetBoundingBox();

						auto corners = bounding_box.GetCorners();
						ellipse = ellipse;

					}

				}
	
				// zones
				chaos::TiledMap::GeometricObjectTile * tile = geometric_object->GetObjectTile();
				if (tile != nullptr)
				{

				}
					
#endif

			}

			chaos::box2 world_bounds;
			chaos::TiledMapTools::FindExplicitWorldBounds(object_layer, world_bounds);

			return true;
		}

		chaos::ParticleAllocation * LayerInstance::CreateParticleAllocation()
		{
			// find render material
			chaos::GPURenderMaterial * render_material = FindRenderMaterial(material_name.c_str());
			if (render_material == nullptr)
				return nullptr;
			// create a particle layer
			particle_layer = GetTypedLevel()->CreateParticleLayer(this);
			if (particle_layer == nullptr)
				return false;
			particle_layer->SetRenderMaterial(render_material);
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

			// get the texture atlas
			chaos::BitmapAtlas::TextureArrayAtlas const * texture_atlas = level->GetTextureAtlas(this);
			if (texture_atlas == nullptr)
				return false;

			// get the folder in which the bitmap information are stored
			chaos::BitmapAtlas::FolderInfo const * folder_info = level->GetFolderInfo(this);
			if (folder_info == nullptr)
				return false;

			// the allocation (will be created only at the last moment)
			chaos::ParticleAllocation * allocation = nullptr;

			// populate the layer
			chaos::TiledMap::Map * tiled_map = level_instance->GetTiledMap();

			size_t const PARTICLE_BUFFER_SIZE = 100;
			TileParticleData new_particles[PARTICLE_BUFFER_SIZE];

			size_t new_particle_count = 0;

			for (size_t i = 0; i < count; ++i)
			{
				TileParticleData & new_part = new_particles[new_particle_count];

				// gid of the tile
				int gid = tile_layer->tile_indices[i];
				if (gid <= 0)
					continue;
				// search the tile information 
				new_part.tile_info = tiled_map->FindTileInfo(gid);
				if (new_part.tile_info.tiledata == nullptr)
					continue;
				// search the bitmap information corresponding to this tile
				new_part.bitmap_info = folder_info->GetBitmapInfo(new_part.tile_info.tiledata->atlas_key.c_str());
				if (new_part.bitmap_info == nullptr)
					continue;
				// create an allocation if necessary
				if (allocation == nullptr)
				{
					allocation = CreateParticleAllocation();
					if (allocation == nullptr)
						return false;
				}
				// create a simple particle
				glm::ivec2 tile_coord = tile_layer->GetTileCoordinate(i);

				chaos::ParticleDefault::Particle & p = new_part.particle;
				p.bounding_box = tile_layer->GetTileBoundingBox(tile_coord, new_part.tile_info.tiledata->image_size);
				p.texcoords = chaos::ParticleTools::GetParticleTexcoords(*new_part.bitmap_info, texture_atlas->GetAtlasDimension());
				p.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				// flush the particles ?
				if (++new_particle_count == PARTICLE_BUFFER_SIZE)
				{
					level->FlushParticles(new_particles, new_particle_count, allocation, this);
					new_particle_count = 0;
				}
			}

			// final flush
			level->FlushParticles(new_particles, new_particle_count, allocation, this); // allocation may be nullptr, but in this case count should be 0
			return true;
		}

		bool LayerInstance::DoTick(double delta_time)
		{
			if (particle_layer != nullptr)
				particle_layer->Tick(delta_time);
			return true;
		}
		
		int LayerInstance::DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
		{
			if (particle_layer != nullptr)
				particle_layer->Display(uniform_provider, render_params);
			return 0;
		}

		PlayerStartObject * LayerInstance::FindPlayerStart(char const * name)
		{
			// very first player start required ?
			if (name == nullptr && player_starts.size() > 0)
				return player_starts[0].get();
			// search by name
			return NamedObject::FindNamedObject(player_starts, name);
		}

		PlayerStartObject const * LayerInstance::FindPlayerStart(char const * name) const
		{
			// very first player start required ?
			if (name == nullptr && player_starts.size() > 0)
				return player_starts[0].get();
			// search by name
			return NamedObject::FindNamedObject(player_starts, name);
		}

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

		bool LevelInstance::DoTick(double delta_time)
		{
			// tick the particle manager
			if (particle_manager != nullptr)
				particle_manager->Tick(delta_time);
			// tick all layer instances
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				layer_instances[i]->Tick(delta_time);

			return true;
		}

		int LevelInstance::DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
		{
			int result = 0;

			// display particle manager
			if (particle_manager != nullptr)
				result += particle_manager->Display(uniform_provider, render_params);
			// draw the layer instances
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				result += layer_instances[i]->Display(uniform_provider, render_params);
			
			return result;
		}

		bool LevelInstance::Initialize(death::Game * in_game)
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

		bool LevelInstance::CreateParticleManager(death::Game * in_game)
		{
			particle_manager = new chaos::ParticleManager;
			if (particle_manager == nullptr)
				return false;
			particle_manager->SetTextureAtlas(in_game->GetTextureAtlas()); // take the atlas
			return true;
		}

		bool LevelInstance::CreateLayerInstances(death::Game * in_game)
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

		chaos::GPURenderMaterial * LevelInstance::GetDefaultRenderMaterial()
		{
			if (default_material == nullptr)
				default_material = GetTypedLevel()->GetDefaultRenderMaterial(); // create material and cache
			return default_material.get();
		}

		PlayerStartObject * LevelInstance::FindPlayerStart(char const * name)
		{
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
			{
				PlayerStartObject * result = layer_instances[i]->FindPlayerStart(name);
				if (result != nullptr)
					return result;
			}
			return nullptr;
		}

		PlayerStartObject const * LevelInstance::FindPlayerStart(char const * name) const
		{
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
			{
				PlayerStartObject const * result = layer_instances[i]->FindPlayerStart(name);
				if (result != nullptr)
					return result;
			}
			return nullptr;
		}

		// =====================================
		// LevelInstance implementation
		// =====================================













	}; // namespace TiledMap

}; // namespace death

