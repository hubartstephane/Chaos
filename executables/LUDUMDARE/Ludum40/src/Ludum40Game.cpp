#include "Ludum40PCH.h"
#include "Ludum40Game.h"
#include "Ludum40SpriteManager.h"


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

bool MyGamepadManager::DoPoolGamepad(chaos::PhysicalGamepad * physical_gamepad)
{
	if (game != nullptr)
		return game->OnPhysicalGamepadInput(physical_gamepad);
	return true;
}

// ======================================================================================

void Game::Tick(float delta_time)
{
	gamepad_manager->Tick(delta_time);

	if (pending_gameover || pending_restart_game)
	{
		GameOver();
	}
	else
	{


		GameInfo game_info(*this);
		if (game_started && !game_paused)
		{
			SpawnExtraParticles(game_info, delta_time);
			UpdatePlayerDisplacement(delta_time);
			FindPlayerCollision();
		}

		for(size_t i = 0 ; i < sprite_layers.size() ; ++i)
			sprite_layers[i].Tick(delta_time, game_info);


	}
	ResetPlayerCachedInputs();
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
		stick_to_apply = stick_position;
	}
	else
	{
		glm::vec2 simulated_stick = glm::vec2(0.0f, 0.0f);

		GLFWwindow * glfw_window = window->GetGLFWHandler();

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
	chaos::sphere2 player_sphere = chaos::GetInnerSphere(player_box);

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
				chaos::sphere2 particle_sphere = chaos::GetInnerSphere(particle_box);

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

			bool death_enabled = true;
#if !_DEBUG
			death_enabled = false;
#endif
			if (death_enabled)
			{
				pending_gameover = true;
			}
			else
			{
				if (collision_source != nullptr)
					collision_source->Play(chaos::PlaySoundDesc());
			}
		}
		else
		{
			if (collision_source != nullptr)
				collision_source->Play(chaos::PlaySoundDesc());
		}

	}
	else if (layer.collision_type == SpriteLayer::COLLISION_LEVELUP)
	{
		if (bonus1_source != nullptr)
			bonus1_source->Play(chaos::PlaySoundDesc());
		++level;
	}
	else if (layer.collision_type == SpriteLayer::COLLISION_SPEEDUP)
	{
		if (bonus2_source != nullptr)
			bonus2_source->Play(chaos::PlaySoundDesc());
		player_absolute_speed += delta_speed;
	}
	return true;
}



bool Game::Initialize(chaos::Window * in_window, nlohmann::json const * config, glm::vec2 const & in_world_size)
{
  chaos::Application * application = chaos::Application::GetInstance();
  if (application == nullptr)
    return false;

  boost::filesystem::path const & resources_path = application->GetResourcesPath();



  if (config != nullptr)
	  InitializeFromConfiguration(config);

  window = in_window;

	world_size = in_world_size;

	boost::filesystem::path object_path = resources_path / "objects" / "objects.json";

	// Load the file
	chaos::Buffer<char> buf = chaos::FileTools::LoadFile(object_path, chaos::LoadFileFlag::ASCII);
	if (buf == nullptr)
		return false;

  // parse JSON structures
  nlohmann::json json;
  if (chaos::JSONTools::Parse(buf, json))
    return DoInitialize(resources_path, object_path, &json);
	return false;
}

bool Game::InitializeSounds(boost::filesystem::path const & resource_path)
{
	sound_manager = chaos::WindowApplication::GetSoundManagerInstance(); // copy shared reference to the manager
	if (sound_manager == nullptr)
		return false;

	chaos::PlaySoundDesc desc;
	desc.looping = true;

	music_source = sound_manager->FindSource("music");
	if (music_source != nullptr)
		music_source->Play(desc);

	gameover_source = sound_manager->FindSource("gameover");
	bonus1_source = sound_manager->FindSource("bonus1");
	bonus2_source = sound_manager->FindSource("bonus2");
	collision_source = sound_manager->FindSource("collision");
	start_source = sound_manager->FindSource("start");
	pause_source = sound_manager->FindSource("pause");
	return true;
}

bool Game::DoInitialize(boost::filesystem::path const & resource_path, boost::filesystem::path const & object_path, nlohmann::json const * json)
{
	if (!InitializeSounds(resource_path))
		return false;
	if (!LoadSpriteLayerInfo(json))
		return false;
	if (!LoadObjectDefinition(json))
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
	texture_atlas = nullptr;

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

bool Game::LoadSpriteLayerInfo(nlohmann::json const * json)
{
	if (nlohmann::json const* layers = chaos::JSONTools::GetAttributeStructureNode(json, "layers"))
	{
		for (auto const & json_layer : *layers)
		{
			SpriteLayer sprite_layer;
			if (!sprite_layer.LoadFromJSON(&json_layer))
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
	SpriteManagerInitParams sprite_params;
	sprite_params.atlas = texture_atlas.get();

	for (size_t i = 0 ; i < sprite_layers.size() ; ++i)
	{
		SpriteLayer & layer = sprite_layers[i];

		chaos::shared_ptr<SpriteManager> sprite_manager = new SpriteManager();
		if (sprite_manager == nullptr)
			return false;
		if (!sprite_manager->Initialize(this, sprite_params))
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

  chaos::FilePathParam path(background_paths[index]);
  chaos::shared_ptr<chaos::GPUTexture> new_background = chaos::GPUTextureLoader(chaos::WindowApplication::GetGPUDeviceInstance()).GenTextureObject(path);
  if (new_background == nullptr)
    return false;
  background_texture = new_background;
  background_index   = index;
  return true;
}

bool Game::GenerateBackgroundResources(boost::filesystem::path const & path)
{
  // get the different background paths
	chaos::FileTools::WithDirectoryContent(path / "backgrounds", [this](boost::filesystem::path const& p)
	{
		background_paths.push_back(p);
		return false; // don't stop
	});

	// generate the background texture
  if (!Game::LoadBackgroundTexture(0))
    return false;
	if (background_texture == nullptr)
		return false;

	// generate the control texture

	chaos::WindowApplication* window_application = chaos::Application::GetInstance();

	chaos::FilePathParam p = path / "controls.png";
	control_texture = chaos::GPUTextureLoader(window_application->GetGPUDevice()).GenTextureObject(p);
	if (control_texture == nullptr)
		return false;

	// generate the background program
	chaos::GPUProgramGenerator background_program_generator;
	background_program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, path / "background_vertex_shader.txt");
	background_program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, path / "background_pixel_shader.txt");

	background_program = background_program_generator.GenProgramObject();
	if (background_program == nullptr)
		return false;

	// generate the controls program
	chaos::GPUProgramGenerator control_program_generator;
	control_program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, path / "control_vertex_shader.txt");
	control_program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, path / "control_pixel_shader.txt");

	control_program = control_program_generator.GenProgramObject();
	if (control_program == nullptr)
		return false;

	// generate the background mesh
	chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

	fullscreen_mesh = chaos::GPUQuadMeshGenerator(b).GenerateMesh(window_application->GetGPUDevice());
	if (fullscreen_mesh == nullptr)
		return false;

	return true;
}

bool Game::GenerateAtlas(boost::filesystem::path const & path)
{
	// Fill Atlas generation Input
	int ATLAS_SIZE = 1024;
	int ATLAS_PADDING = 10;
	chaos::AtlasGeneratorParams params = chaos::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());


	chaos::AtlasInput input;

	chaos::AtlasFolderInfoInput * folder = input.AddFolder("sprites", 0);
	if (folder == nullptr)
		return false;

	for (ObjectDefinition const & def : object_definitions)
	{
		boost::filesystem::path image_path = chaos::PathTools::FindAbsolutePath(path, def.bitmap_path); // make the image path relative to resource path
		if (!folder->AddBitmap(image_path, nullptr, def.id))
			return false;
	}

	// generate STD Atlas
	chaos::WindowApplication* window_application = chaos::Application::GetInstance();

	chaos::GPUAtlasGenerator generator(window_application->GetGPUDevice());
	texture_atlas = generator.GenerateAtlas(input, params);
	if (texture_atlas == nullptr)
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

bool Game::LoadObjectDefinition(nlohmann::json const * json)
{
	if (nlohmann::json const* objects = chaos::JSONTools::GetAttributeStructureNode(json, "objects"))
	{
		for (auto const & json_obj : *objects)
		{
			ObjectDefinition def;
			if (!def.LoadFromJSON(&json_obj))
				continue;
			if (FindSpriteLayer(def.layer) == nullptr) // layer not existing
				continue;
			object_definitions.push_back(std::move(def));
		}
	}
	return true;
}

void Game::DisplayBackground(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size)
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	DisplayFullscreen(render_context, viewport_size, background_texture, background_program);
}

void Game::DisplayControls(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	DisplayFullscreen(render_context, viewport_size, control_texture, control_program);
}

void Game::DisplayFullscreen(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size, chaos::shared_ptr<chaos::GPUTexture> texture, chaos::shared_ptr<chaos::GPUProgram> program)
{
	// compute the texture aspect, compare to world aspect so we can find correct texture coordinates
	chaos::TextureDescription texture_description = texture->GetTextureDescription();


  chaos::box2 texture_box = chaos::box2(std::make_pair(
    glm::vec2(0.0f, 0.0f),
    glm::vec2((float)texture_description.width, (float)texture_description.height)
  ));

  float world_aspect = chaos::MathTools::CastAndDiv<float>(world_size.x, world_size.y);

  // get a box that fit the texture size and wanted aspect
  chaos::box2 shrinked_texture_box = chaos::SetBoxAspect(texture_box, world_aspect, chaos::SetBoxAspectMethod::SHRINK_BOX);

  glm::vec2 min_texture_coord = glm::vec2(0.0f, 0.0f);
  glm::vec2 max_texture_coord = glm::vec2(1.0f, 1.0f);

  auto texture_corners = GetBoxCorners(shrinked_texture_box);
  min_texture_coord = texture_corners.first  / (2.0f * texture_box.half_size);
  max_texture_coord = texture_corners.second / (2.0f * texture_box.half_size);

	// set the data for program
	float life_ratio = chaos::MathTools::CastAndDiv<float>(life, initial_life);
	if (life_ratio > 1.0f)
		life_ratio = 1.0f;

	float level_ratio = chaos::MathTools::CastAndDiv<float>(level, 40);
	if (level_ratio > 1.0f)
		level_ratio = 1.0f;

	chaos::GPUProgramProvider uniform_provider;
	uniform_provider.AddTexture("material", texture);
	uniform_provider.AddVariable("min_texture_coord", min_texture_coord);
	uniform_provider.AddVariable("max_texture_coord", max_texture_coord);
	uniform_provider.AddVariable("life_ratio", life_ratio);
	uniform_provider.AddVariable("level_ratio", level_ratio);

	chaos::GPURenderParams render_params;
	fullscreen_mesh->DisplayWithProgram(program.get(), render_context, &uniform_provider, render_params);
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

void Game::DisplaySprites(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size)
{
  chaos::shared_ptr<chaos::GPUProgramProvider> uniform_provider = new chaos::GPUProgramProvider;

	glm::vec3 scale = glm::vec3(2.0f / world_size.x, 2.0f / world_size.y, 1.0f);
	glm::vec3 tr    = glm::vec3(-world_position.x, -world_position.y, 0.0f);

	glm::mat4 local_to_cam =  glm::scale(scale) /* * glm::translate(tr)*/; // SCREEN SPACE particles, no TRANSATION

	uniform_provider->AddVariable("local_to_cam", local_to_cam);


  for (size_t i = sprite_layers.size(); i > 0; --i)
  {
    size_t index = i - 1;
    sprite_layers[index].Draw(uniform_provider.get());
  }
}

void Game::Display(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size)
{
	// XXX : for screen,
	//
	//       (0, 0) is center.
	//       (world_size.x / 2, world_size.y) are limits of the the viewport

	DisplayBackground(render_context, viewport_size);

	DisplaySprites(render_context, viewport_size);

	DisplayControls(render_context, viewport_size);
}

bool Game::OnPhysicalGamepadInput(chaos::PhysicalGamepad * physical_gamepad)
{
	if (!game_started)
	{
		if (physical_gamepad->IsAnyKeyAction())
			StartGame();
	}
	else
	{
		if ((physical_gamepad->IsKeyJustPressed(chaos::GamepadButton::SPECIAL_LEFT)) ||
			(physical_gamepad->IsKeyJustPressed(chaos::GamepadButton::SPECIAL_RIGHT)))
			SetPause(!game_paused);
	}

	glm::vec2 left_stick_position  = physical_gamepad->GetStickValue(chaos::GamepadStick::LEFT_STICK);
	glm::vec2 right_stick_position = physical_gamepad->GetStickValue(chaos::GamepadStick::RIGHT_STICK);

	if (glm::length2(left_stick_position) > 0.0f)
		stick_position = left_stick_position;
	else if (glm::length2(right_stick_position) > 0.0f)
		stick_position = right_stick_position;

	return true;
}


bool Game::OnKeyEvent(chaos::KeyboardButton key, chaos::KeyAction action)
{
	if (!game_started)
	{
		if (action == chaos::KeyAction::Press)
		{
			StartGame();
			return true;
		}
	}
	else
	{
		if (key == chaos::KeyboardButton::ESCAPE && action == chaos::KeyAction::Press)
		{
			pending_restart_game = true;
			return true;
		}
	}

	if (key == chaos::KeyboardButton::P && action == chaos::KeyAction::Press)
	{
		SetPause(!game_paused);
		return true;
	}

	return false;
}

void Game::GameOver()
{
	if (pending_gameover && gameover_source != nullptr)
		gameover_source->Play(chaos::PlaySoundDesc());

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
		layer.PopulateSprites(game_info, layer.min_particle_count);
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
		pause_source->Play(chaos::PlaySoundDesc());
}

void Game::OnGameStarted()
{
	if (start_source != nullptr)
		start_source->Play(chaos::PlaySoundDesc());

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

void Game::InitializeFromConfiguration(nlohmann::json const * config)
{
  chaos::JSONTools::GetAttribute(config, "level_particle_increment", level_particle_increment, 10);
  chaos::JSONTools::GetAttribute(config, "max_particles_per_frame", max_particles_per_frame, 100);
  chaos::JSONTools::GetAttribute(config, "initial_life", initial_life, 50);
  chaos::JSONTools::GetAttribute(config, "initial_level", initial_level, 0);
  chaos::JSONTools::GetAttribute(config, "initial_player_screen_speed", initial_player_screen_speed, 500.0f);
  chaos::JSONTools::GetAttribute(config, "initial_player_absolute_speed", initial_player_absolute_speed, 50.0f);
  chaos::JSONTools::GetAttribute(config, "delta_speed", delta_speed, 7.0f);
  chaos::JSONTools::GetAttribute(config, "max_speed", max_speed, 500.0f);
  chaos::JSONTools::GetAttribute(config, "slowdown_factor", slowdown_factor, 500.0f);
  chaos::JSONTools::GetAttribute(config, "acceleration_factor", acceleration_factor, 500.0f);


}