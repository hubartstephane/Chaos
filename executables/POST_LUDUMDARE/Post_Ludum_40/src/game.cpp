#include "game.h"

#include <chaos/StandardHeaders.h> 
#include <chaos/CollisionFramework.h>
#include <chaos/MathTools.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GPUProgramVariableProvider.h>
#include <chaos/SoundManager.h>
#include <json.hpp>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/SpriteManager.h>
#include <chaos/SoundManager.h>
#include <chaos/JSONTools.h>


// ======================================================================================

//                                             extra_x
// +---------------+----------------------+---------------+
// |               |                      |               |
// |   ZONE A      |       ZONE B         |   ZONE C      |
// |               |                      |               | extra_y
// |               |                      |               |
// +---------------+----------------------+---------------+
// |               |                      |               |
// |  ZONE D       |    SCREEN            |  ZONE E       |
// |               |        world_size    |               |
// |               |                      | world_padding |
// +---------------+----------------------+---------------+
//
// particles are to be spawned in any of the zone A, B, C, D, E
// propability of theses zones dependant on their surface
//
//

// ======================================================================================

bool MyGamepadManager::DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	if (game != nullptr)
		return game->OnPhysicalGamepadInput(physical_gamepad);
	return true;
}

// ======================================================================================

void Game::Tick(double delta_time)
{
	if (pending_gameover || pending_restart_game)
	{
		GameOver();
	}		
	else
	{
		gamepad_manager->Tick((float)delta_time);

		GameInfo game_info(*this);
		if (game_started && !game_paused)
		{
			SpawnExtraParticles(game_info, (float)delta_time);
			UpdatePlayerDisplacement((float)delta_time);
			FindPlayerCollision();
		}
		
		for(size_t i = 0 ; i < sprite_layers.size() ; ++i)
			sprite_layers[i].Tick(delta_time, game_info);		

		ResetPlayerCachedInputs();
	}
}

void Game::SpawnExtraParticles(GameInfo game_info, float delta_time)
{
	for (size_t i = 0 ; i < sprite_layers.size() ; ++i)
	{
		SpriteLayer & layer = sprite_layers[i];
		if ((int)layer.particles.size() >= layer.max_particle_count) // already too much particles
			continue;

		// number of particles we would like to have at the end (with LEVEL consideration)		
		int count = layer.min_particle_count + (level * level_particle_increment); 

		// clamp to the max count (singleton will remains singletons)
		if (count > layer.max_particle_count) 
			count = layer.max_particle_count;

		int particles_to_spawn = (count - (int)layer.particles.size());

		if (particles_to_spawn > max_particles_per_frame)
			particles_to_spawn = max_particles_per_frame;

		if (particles_to_spawn > 0)
			layer.PopulateSprites(game_info, particles_to_spawn);		
	}
}


chaos::box2 Game::GetWorldBox(bool use_padding) const
{
	if (use_padding)
	{
		if (world_padding_ratio.x > 0.0f && world_padding_ratio.y > 0.0f)
		{
			float min_x = world_position.x - (0.5f * world_size.x) - (world_size.x * world_padding_ratio.x); 
			float max_x = world_position.x + (0.5f * world_size.x) + (world_size.x * world_padding_ratio.x); 
		
			float min_y = world_position.y - (0.5f * world_size.y);
			float max_y = world_position.y + (0.5f * world_size.y) + (world_size.y * world_padding_ratio.y); 

			glm::vec2 bottom_left = glm::vec2(min_x, min_y);
			glm::vec2 top_right   = glm::vec2(max_x, max_y);

			return chaos::box2(std::make_pair(bottom_left, top_right));		
		}	
	}
	return chaos::box2(world_position, 0.5f * world_size);;
}

void Game::ResetPlayerCachedInputs()
{
	stick_position = glm::vec2(0.0f, 0.0f);
}

void Game::UpdatePlayerDisplacement(float delta_time)
{
	glm::vec2 stick_to_apply = glm::vec2(0.0f, 0.0f);

	if (stick_position.x != 0.0f || stick_position.y != 0.0f)
	{
		glm::vec2 invert_y_stick = glm::vec2(1.0f, -1.0f);

		stick_to_apply = stick_position * invert_y_stick;
	}
	else
	{
		glm::vec2 simulated_stick = glm::vec2(0.0f, 0.0f);

		if (glfwGetKey(glfw_window, GLFW_KEY_LEFT))
			simulated_stick.x -= 1.0f;
		if (glfwGetKey(glfw_window, GLFW_KEY_RIGHT))
			simulated_stick.x += 1.0f;

		if (glfwGetKey(glfw_window, GLFW_KEY_DOWN))
			simulated_stick.y -= 1.0f;
		if (glfwGetKey(glfw_window, GLFW_KEY_UP))
			simulated_stick.y += 1.0f;
	
		stick_to_apply = simulated_stick;
	}

	UpdateWorldAndPlayerPosition(delta_time, stick_to_apply);
}

void Game::UpdateWorldAndPlayerPosition(float delta_time, glm::vec2 const & direction)
{
	Particle * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return;

	glm::vec2 wp1 = world_position;

	// take camera box, recenter to SCREEN SPACE and SCALE DOWN
	chaos::box2 world_box = GetWorldBox(false);
	world_box.position = glm::vec2(0.0f, 0.0f);
	world_box.half_size *= screen_safe_aera;
		
	// apply joystick displacement
	glm::vec2 player_screen_position = player_particle->position; 

  player_speed = direction * max_speed;

	player_screen_position += (delta_time * player_speed);

	// restrict the player displacement
	chaos::box2 player_box = chaos::box2(player_screen_position, player_particle->half_size);
	
	float p1 = 0.0f;
	float p2 = 0.0f;

	p1 = player_box.position.x;
	chaos::RestrictToInside(world_box, player_box, false); // PLAYER is being recentered into safe screen aera => camera is compensenting
	p2 = player_box.position.x;

	player_screen_position = player_box.position;
	player_particle->position = player_screen_position;

	// displace the world due to player engine
	world_position += glm::vec2(-(p2 - p1), delta_time * player_absolute_speed);

	// update all screen space position particles
	glm::vec2 wp2 = world_position;
	UpdateParticlesPosition(0.0f, -(wp2 - wp1));

	// FIX !!! we only use screen space coordinate, the having a world that is still at 0 position is not a bad idea 
	//         to avoid float issues
	world_position = glm::vec2(0.0f, 0.0f); 
}

bool Game::FindPlayerCollision()
{
	bool result = false;

	Particle const * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return result;

  glm::vec2 pp  = player_particle->position;
  glm::vec2 phs = player_particle->half_size;

	chaos::box2    player_box    = chaos::box2(pp, phs);
	chaos::sphere2 player_sphere = chaos::GetInnerCircle(player_box);

  chaos::triangle2 player_triangle;
  player_triangle.a = glm::vec2(pp.x, pp.y + phs.y);
  player_triangle.b = glm::vec2(pp.x + phs.x, pp.y - phs.y);
  player_triangle.c = glm::vec2(pp.x - phs.x, pp.y - phs.y);
  player_triangle = chaos::PrepareTriangleForCollision(player_triangle);

	for (size_t i = 0 ; i < sprite_layers.size() ; ++i)
	{
		SpriteLayer & layer = sprite_layers[i];
		if (layer.collision_type == SpriteLayer::NO_COLLISION)
			continue;

		size_t j = 0;
		while (j < layer.particles.size())
		{
			Particle & p = layer.particles[j];

			chaos::box2 particle_box = chaos::box2(p.position, p.half_size);

			if (chaos::Collide(player_box, particle_box)) // raw collision detection
			{				
				chaos::sphere2 particle_sphere = chaos::GetInnerCircle(particle_box);

				if (chaos::Collide(player_triangle, particle_sphere)) // more precise
				{
					if (OnCollision(p, layer))
					{
						result = true;
						layer.particles[j] = layer.particles.back();
						layer.particles.pop_back();
						continue;				
					}				
				}
			}				
			++j;
		}
	}
	return result;
}

bool Game::OnCollision(Particle & p, SpriteLayer & layer)
{
	if (layer.collision_type == SpriteLayer::COLLISION_DEATH)
	{
		if (--life < 0)
		{
			life = 0; // cheat mode en debug, but want life to be visually visible
#if !_DEBUG
			pending_gameover = true;
#else
			if (collision_source != nullptr)
				collision_source->PlaySound(chaos::PlaySoundDesc());	
#endif
		}
		else
		{
			if (collision_source != nullptr)
				collision_source->PlaySound(chaos::PlaySoundDesc());		
		}
			
	}
	else if (layer.collision_type == SpriteLayer::COLLISION_LEVELUP)
	{
		if (bonus1_source != nullptr)
			bonus1_source->PlaySound(chaos::PlaySoundDesc());
		++level;
	}
	else if (layer.collision_type == SpriteLayer::COLLISION_SPEEDUP)
	{
		if (bonus2_source != nullptr)
			bonus2_source->PlaySound(chaos::PlaySoundDesc());
		player_absolute_speed += delta_speed;
	}
	return true;
}



bool Game::Initialize(GLFWwindow * in_glfw_window, nlohmann::json const * configuration, glm::vec2 const & in_world_size)
{
  chaos::Application * application = chaos::Application::GetInstance();
  if (application == nullptr)
    return false;

  boost::filesystem::path const & resources_path = application->GetResourcesPath();



  if (configuration != nullptr)
	InitializeFromConfiguration(*configuration);

	glfw_window = in_glfw_window;

	world_size = in_world_size;

	boost::filesystem::path object_path = resources_path / "objects" / "objects.json";

	// Load the file
	chaos::Buffer<char> buf = chaos::FileTools::LoadFile(object_path, true);
	if (buf == nullptr)
		return false;

  // parse JSON structures
  nlohmann::json json_entry = chaos::JSONTools::Parse(buf);
  return DoInitialize(resources_path, object_path, json_entry);

	return false;
}

bool Game::InitializeSounds(boost::filesystem::path const & resource_path)
{
  chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
  if (application == nullptr)
    return false;

  sound_manager = application->GetSoundManager(); // copy shared reference to the manager
  if (sound_manager == nullptr)
    return false;
    
	chaos::PlaySoundDesc desc;
	desc.looping = true;

	music_source = sound_manager->AddSource(resource_path / "Sounds" / "music.ogg");
	if (music_source != nullptr)
		music_source->PlaySound(desc);

	gameover_source = sound_manager->AddSource(resource_path / "Sounds" / "gameover.ogg");
	bonus1_source = sound_manager->AddSource(resource_path / "Sounds" / "bonus1.ogg");
	bonus2_source = sound_manager->AddSource(resource_path / "Sounds" / "bonus2.ogg");
	collision_source = sound_manager->AddSource(resource_path / "Sounds" / "collision.ogg");
	start_source = sound_manager->AddSource(resource_path / "Sounds" / "start.ogg");
	pause_source = sound_manager->AddSource(resource_path / "Sounds" / "pause.ogg");
		
	return true;
}

bool Game::DoInitialize(boost::filesystem::path const & resource_path, boost::filesystem::path const & object_path, nlohmann::json const & json_entry)
{
	if (!InitializeSounds(resource_path))
		return false;
	if (!LoadSpriteLayerInfo(json_entry))
		return false;
	if (!LoadObjectDefinition(json_entry))
		return false;
	if (!GenerateAtlas(object_path))
		return false;
	if (!InitializeSpriteManagers())
		return false;

	if (!InitializeGamepadManager())
		return false;
	if (!GenerateBackgroundResources(resource_path))
		return false;

	ResetWorld();

	return true;
}

bool Game::InitializeGamepadManager()
{
	gamepad_manager = new MyGamepadManager(this);

	return (gamepad_manager != nullptr);
}

void Game::Finalize()
{
	sprite_layers.clear();
	object_definitions.clear();
	texture_atlas.Clear();

	fullscreen_mesh = nullptr;

	background_program = nullptr;	
	background_texture = nullptr;

	control_program = nullptr;
	control_texture = nullptr;
	
	gamepad_manager = nullptr;

	music_source = nullptr;
	gameover_source = nullptr;
	collision_source = nullptr;
	bonus1_source = nullptr;
	bonus2_source = nullptr;
	start_source = nullptr;
	pause_source = nullptr;

  sound_manager = nullptr;
}

SpriteLayer * Game::FindSpriteLayer(int layer)
{
	for (size_t i = 0 ; i < sprite_layers.size(); ++i)
		if (sprite_layers[i].layer == layer)
			return &sprite_layers[i];
	return nullptr;
}

SpriteLayer const * Game::FindSpriteLayer(int layer) const
{
	for (size_t i = 0 ; i < sprite_layers.size(); ++i)
		if (sprite_layers[i].layer == layer)
			return &sprite_layers[i];
	return nullptr;
}

bool Game::LoadSpriteLayerInfo(nlohmann::json const & json_entry)
{
	nlohmann::json const * layers = chaos::JSONTools::GetStructure(json_entry, "layers"); 
	if (layers != nullptr)
	{
		for (auto const & json_layer : *layers)
		{
			SpriteLayer sprite_layer;
			if (!sprite_layer.LoadFromJSON(json_layer))
				continue;			
			if (FindSpriteLayer(sprite_layer.layer) != nullptr) // already existing
				continue;				
			sprite_layers.push_back(sprite_layer);
		}
	}
	return true;
}

bool Game::InitializeSpriteManagers()
{
	chaos::SpriteManagerInitParams sprite_params;
	sprite_params.atlas = &texture_atlas;

	for (size_t i = 0 ; i < sprite_layers.size() ; ++i)
	{
		SpriteLayer & layer = sprite_layers[i];
	
		boost::intrusive_ptr<chaos::SpriteManager> sprite_manager = new chaos::SpriteManager();
		if (sprite_manager == nullptr)
			return false;			
		if (!sprite_manager->Initialize(sprite_params))
			return false;	

		sprite_params.program = sprite_manager->GetProgram(); // reuse the program of the first manager into the other managers

		layer.sprite_manager = sprite_manager;
	}

	// sort the layers
	std::sort(sprite_layers.begin(), sprite_layers.end(), [](SpriteLayer const & obj1, SpriteLayer const & obj2){	
		return (obj1.layer < obj2.layer);			
	});

	return true;
}

bool Game::LoadBackgroundTexture(size_t index)
{
  if (background_paths.size() == 0)
    return false;

  index = index % background_paths.size();

  boost::intrusive_ptr<chaos::Texture> new_background = chaos::GLTextureTools::GenTextureObject(background_paths[index]);
  if (new_background == nullptr)
    return false;
  background_texture = new_background;
  background_index   = index;
  return true;
}

bool Game::GenerateBackgroundResources(boost::filesystem::path const & path)
{
  // get the different background paths
  boost::filesystem::directory_iterator end;
  for (boost::filesystem::directory_iterator it(path / "backgrounds"); it != end; ++it)
    background_paths.push_back(*it);

	// generate the background texture
  if (!Game::LoadBackgroundTexture(0))
    return false;
	if (background_texture == nullptr)
		return false;

	// generate the control texture
	control_texture = chaos::GLTextureTools::GenTextureObject(path / "controls.png");
	if (control_texture == nullptr)
		return false;

	// generate the background program
	chaos::GPUProgramLoader background_loader;
	background_loader.AddShaderSourceFile(GL_VERTEX_SHADER, path / "background_vertex_shader.txt");
	background_loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, path / "background_pixel_shader.txt");

	background_program = background_loader.GenerateProgramObject();
	if (background_program == nullptr)
		return false;

	// generate the controls program
	chaos::GPUProgramLoader control_loader;
	control_loader.AddShaderSourceFile(GL_VERTEX_SHADER, path / "control_vertex_shader.txt");
	control_loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, path / "control_pixel_shader.txt");

	control_program = control_loader.GenerateProgramObject();
	if (control_program == nullptr)
		return false;

	// generate the background mesh
	chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

	fullscreen_mesh = chaos::QuadMeshGenerator(b).GenerateMesh();
	if (fullscreen_mesh == nullptr)
		return false;

	return true;
}

bool Game::GenerateAtlas(boost::filesystem::path const & path)
{
	// Fill Atlas generation Input
	int ATLAS_SIZE = 1024;
	int ATLAS_PADDING = 10;
	chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());

	chaos::BitmapAtlas::AtlasInput input;

	chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return false;

	for (ObjectDefinition const & def : object_definitions)
	{
		boost::filesystem::path image_path = chaos::BoostTools::FindAbsolutePath(path, def.bitmap_path); // make the image path relative to resource path
		if (!bitmap_set->AddBitmapFile(image_path, nullptr, def.id))
			return false;
	}

	// generate STD Atlas
	chaos::BitmapAtlas::Atlas          atlas;
	chaos::BitmapAtlas::AtlasGenerator generator;
	if (!generator.ComputeResult(input, atlas, params))
		return false;

	// Display debug Atlas
#if 0
	boost::filesystem::path dst_p;
	if (chaos::FileTools::CreateTemporaryDirectory("TestMergedAtlas", dst_p))
	{
		atlas.SaveAtlas(dst_p / "LudumAtlas");
		chaos::WinTools::ShowFile(dst_p);
	}
#endif

	// generate texture Atlas
	if (!texture_atlas.LoadFromBitmapAtlas(atlas))
		return false;

	return true;
}

ObjectDefinition const * Game::FindObjectDefinition(int id) const
{
	for (ObjectDefinition const & def : object_definitions)
		if (def.id == id)
			return &def;
	return nullptr;
}

bool Game::LoadObjectDefinition(nlohmann::json const & json_entry)
{
	nlohmann::json const * objects = chaos::JSONTools::GetStructure(json_entry, "objects");
	if (objects != nullptr)
	{
		for (auto const & json_obj : *objects)
		{
			ObjectDefinition def;
			if (!def.LoadFromJSON(json_obj))
				continue;
			if (FindSpriteLayer(def.layer) == nullptr) // layer not existing
				continue;
			object_definitions.push_back(std::move(def));
		}	
	}
	return true;
}

void Game::DisplayBackground(glm::ivec2 viewport_size)
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	DisplayFullscreen(viewport_size, background_texture, background_program);
}

void Game::DisplayControls(glm::ivec2 viewport_size)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	DisplayFullscreen(viewport_size, control_texture, control_program);	
}

void Game::DisplayFullscreen(glm::ivec2 viewport_size, boost::intrusive_ptr<chaos::Texture> texture, boost::intrusive_ptr<chaos::GPUProgram> program)
{
	// compute the texture aspect, compare to world aspect so we can find correct texture coordinates
	chaos::TextureDescription texture_description = texture->GetTextureDescription();


  chaos::box2 texture_box = chaos::box2(std::make_pair(
    glm::vec2(0.0f, 0.0f),
    glm::vec2((float)texture_description.width, (float)texture_description.height)
  ));

  float world_aspect = chaos::MathTools::CastAndDiv<float>(world_size.x, world_size.y);

  // get a box that fit the texture size and wanted aspect
  chaos::box2 shrinked_texture_box = chaos::ShrinkBoxToAspect(texture_box, world_aspect);

  glm::vec2 min_texture_coord = glm::vec2(0.0f, 0.0f);
  glm::vec2 max_texture_coord = glm::vec2(1.0f, 1.0f);

  auto texture_corners = shrinked_texture_box.GetCorners();
  min_texture_coord = texture_corners.first  / (2.0f * texture_box.half_size);
  max_texture_coord = texture_corners.second / (2.0f * texture_box.half_size);

	// set the data for program
	float life_ratio = chaos::MathTools::CastAndDiv<float>(life, initial_life);
	if (life_ratio > 1.0f)
		life_ratio = 1.0f;

	float level_ratio = chaos::MathTools::CastAndDiv<float>(level, 40);
	if (level_ratio > 1.0f)
		level_ratio = 1.0f;

	chaos::GPUProgramVariableProviderChain uniform_provider;
	uniform_provider.AddVariableTexture("material", texture);
	uniform_provider.AddVariableValue("min_texture_coord", min_texture_coord);
	uniform_provider.AddVariableValue("max_texture_coord", max_texture_coord);
	uniform_provider.AddVariableValue("life_ratio", life_ratio);
	uniform_provider.AddVariableValue("level_ratio", level_ratio);

	fullscreen_mesh->Render(program.get(), &uniform_provider, 0, 0);
}

void Game::UpdateParticlesPosition(float delta_time, glm::vec2 delta_pos)
{
	for (size_t i = 0 ; i < sprite_layers.size() ; ++i)
	{
		SpriteLayer & layer = sprite_layers[i];
		if (layer.relative_speed <= 0.0f)
			continue;

		for (size_t j = 0 ; j < layer.particles.size() ; ++j)
		{
			Particle & p = layer.particles[j];
			p.position += delta_pos * layer.relative_speed;
		}
	}
}

void Game::DisplaySprites(glm::ivec2 viewport_size)
{
  boost::intrusive_ptr<chaos::GPUProgramVariableProviderChain> uniform_provider = new chaos::GPUProgramVariableProviderChain;

	glm::vec3 scale = glm::vec3(2.0f / world_size.x, 2.0f / world_size.y, 1.0f);
	glm::vec3 tr    = glm::vec3(-world_position.x, -world_position.y, 0.0f); 

	glm::mat4 local_to_cam =  glm::scale(scale) /* * glm::translate(tr)*/; // SCREEN SPACE particles, no TRANSATION

	uniform_provider->AddVariableValue("local_to_cam", local_to_cam);


  for (size_t i = sprite_layers.size(); i > 0; --i)
  {
    size_t index = i - 1;
    sprite_layers[index].Draw(uniform_provider.get());
  }
}

void Game::Display(glm::ivec2 viewport_size)
{
	// XXX : for screen,
	//
	//       (0, 0) is center.
	//       (world_size.x / 2, world_size.y) are limits of the the viewport

	DisplayBackground(viewport_size);
			
	DisplaySprites(viewport_size);

	DisplayControls(viewport_size);
}

bool Game::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	if (!game_started)
	{		
		if (physical_gamepad->IsAnyButtonPressed())
			StartGame();
	}
	else
	{
		if ((physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_SELECT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED) || 
			(physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_START) == chaos::MyGLFW::BUTTON_BECOME_PRESSED))
			SetPause(!game_paused);		
	}		

	glm::vec2 left_stick_position  = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_LEFT_AXIS);
	glm::vec2 right_stick_position = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);

	if (left_stick_position.x != 0.0f || left_stick_position.y != 0.0f)
		stick_position = left_stick_position;
	else if (right_stick_position.length() > 0.0f) 
		stick_position = right_stick_position;

	return true;
}


bool Game::OnKeyEvent(int key, int action)
{
	if (!game_started)
	{
		if (action == GLFW_PRESS)
		{
			StartGame();	
			return true;
		}	
	}
	else
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			pending_restart_game = true;
			return true;
		}		
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		SetPause(!game_paused);
		return true;
	}

	return false;
}

void Game::GameOver()
{
	if (pending_gameover && gameover_source != nullptr)
		gameover_source->PlaySound(chaos::PlaySoundDesc());

	bool old_pending_gameover = pending_gameover;
	bool old_pending_restart_game = pending_restart_game;
	
	game_started = false;
	game_paused  = false;

	ResetWorld(); // Layer visibility flags are reseted here !!! => copy want we want for "game over" or "restart"

	SetLayerVisibility(PAUSED_OBJECT_LAYER, false);
	SetLayerVisibility(GAMEOVER_OBJECT_LAYER, old_pending_gameover);
	SetLayerVisibility(TITLE_OBJECT_LAYER, old_pending_restart_game);

  LoadBackgroundTexture(background_index + 1);
}

void Game::ResetWorld()
{
	pending_restart_game = pending_gameover = false;	

	world_position = GetWorldInitialPosition();

	player_screen_speed = initial_player_screen_speed;
	player_absolute_speed = initial_player_absolute_speed;
	life  = initial_life;
	level = initial_level;
  player_speed = glm::vec2(0.0f, 0.0f);
	
	GameInfo game_info(*this);
	for (SpriteLayer & layer : sprite_layers)
	{
		layer.SetVisible(layer.start_visible);
		layer.DestroyAllParticles();
		//layer.PopulateSprites(game_info, layer.min_particle_count);	
	}

	glm::vec2 screen_space_position = GetPlayerInitialScreenPosition();

	glm::vec2 player_position = world_position + screen_space_position; 
	SetPlayerScreenPosition(screen_space_position);
}

void Game::StartGame()
{
	if (game_started)
		return;
	if (game_paused)
		return;
	OnGameStarted();
}

void Game::SetLayerVisibility(int layer, bool visible)
{
	SpriteLayer * sprite_layer = FindSpriteLayer(layer);
	if (sprite_layer != nullptr)
		sprite_layer->SetVisible(visible);
}

void Game::SetPause(bool in_paused)
{
	if (!game_started)
		return;
	if (game_paused == in_paused)
		return;

	game_paused = in_paused;

	SetLayerVisibility(PAUSED_OBJECT_LAYER, game_paused);

	if (pause_source != nullptr)
		pause_source->PlaySound(chaos::PlaySoundDesc());
}

void Game::OnGameStarted()
{
	if (start_source != nullptr)
		start_source->PlaySound(chaos::PlaySoundDesc());

	game_started = true;
	game_paused  = false;

	SetLayerVisibility(TITLE_OBJECT_LAYER, false);
	SetLayerVisibility(GAMEOVER_OBJECT_LAYER, false);
}


Particle const * Game::GetPlayerParticle() const
{
	SpriteLayer const * layer = FindSpriteLayer(PLAYER_LAYER);
	if (layer != nullptr)
		if (layer->particles.size() == 1)
			return &layer->particles[0];
	return nullptr;
}

Particle * Game::GetPlayerParticle()
{
	SpriteLayer * layer = FindSpriteLayer(PLAYER_LAYER);
	if (layer != nullptr)
		if (layer->particles.size() == 1)
			return &layer->particles[0];
	return nullptr;
}

glm::vec2 Game::GetPlayerScreenPosition() const
{
	Particle const * player_particle = GetPlayerParticle();
	if (player_particle != nullptr)
		return player_particle->position;
	return glm::vec2(0.0f, 0.0f);
}

void Game::SetPlayerScreenPosition(glm::vec2 const & in_position)
{
	Particle * player_particle = GetPlayerParticle();
	if (player_particle != nullptr)
		player_particle->position = in_position;
}

glm::vec2 Game::GetWorldInitialPosition() const
{
	return glm::vec2(500.0f, 0.0f);
}

glm::vec2 Game::GetPlayerInitialScreenPosition() const
{
	Particle const * player_particle = GetPlayerParticle();
	if (player_particle != nullptr)
	{
		return glm::vec2(
			0.0f,
			0.75f * (-0.5f * world_size.y + player_particle->half_size.y)		
		);		
	}
	return glm::vec2(0.0f, 0.0f);	
}

void Game::InitializeFromConfiguration(nlohmann::json const & configuration)
{
  chaos::JSONTools::GetAttribute(configuration, "level_particle_increment", level_particle_increment, 10);
  chaos::JSONTools::GetAttribute(configuration, "max_particles_per_frame", max_particles_per_frame, 100);
  chaos::JSONTools::GetAttribute(configuration, "initial_life", initial_life, 50);
  chaos::JSONTools::GetAttribute(configuration, "initial_level", initial_level, 0);
  chaos::JSONTools::GetAttribute(configuration, "initial_player_screen_speed", initial_player_screen_speed, 500.0f);
  chaos::JSONTools::GetAttribute(configuration, "initial_player_absolute_speed", initial_player_absolute_speed, 50.0f);
  chaos::JSONTools::GetAttribute(configuration, "delta_speed", delta_speed, 7.0f);
  chaos::JSONTools::GetAttribute(configuration, "max_speed", max_speed, 500.0f);
  chaos::JSONTools::GetAttribute(configuration, "slowdown_factor", slowdown_factor, 500.0f);
  chaos::JSONTools::GetAttribute(configuration, "acceleration_factor", acceleration_factor, 500.0f);

  
}