#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/SoundManager.h>
#include <json.hpp>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>

#include "SpriteManager.h"
#include "sprite_layer.h"

// ======================================================================================

class MyGamepadManager : public chaos::MyGLFW::GamepadManager
{
public:

	MyGamepadManager(class Game * in_game) : game(in_game) {}

protected:

	virtual bool DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad) override;

protected:

	class Game * game = nullptr;
};

// ======================================================================================

class Game : public chaos::ReferencedObject
{	
	static const int PLAYER_LAYER = -1;	
	static const int PAUSED_OBJECT_LAYER = -3;
	static const int TITLE_OBJECT_LAYER = -2;
	static const int GAMEOVER_OBJECT_LAYER = -4;

	friend class GameInfo;

	friend class MyGamepadManager;

public:

	void Tick(double delta_time);

	bool Initialize(GLFWwindow * in_glfw_window, nlohmann::json const * config, boost::filesystem::path const & config_path, glm::vec2 const & in_world_size);

	void Finalize();

	void Display(glm::ivec2 viewport_size);

	void SetPause(bool in_paused);

	void StartGame();

	void ResetWorld();

	bool OnKeyEvent(int key, int action);

protected:

  void InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);

	void UpdateParticlesPosition(float delta_time, glm::vec2 delta_pos);

	void SpawnExtraParticles(GameInfo game_info, float delta_time);

	void DisplaySprites(glm::ivec2 viewport_size);

	void DisplayBackground(glm::ivec2 viewport_size);
	void DisplayControls(glm::ivec2 viewport_size);
	void DisplayFullscreen(glm::ivec2 viewport_size, boost::intrusive_ptr<chaos::Texture> texture, boost::intrusive_ptr<chaos::GPUProgram> program);

	void SetLayerVisibility(int layer, bool visible);



	bool DoInitialize(boost::filesystem::path const & resource_path, boost::filesystem::path const & object_path, nlohmann::json const & json_entry);

	bool LoadObjectDefinition(nlohmann::json const & json_entry);

	bool GenerateAtlas(boost::filesystem::path const & path);

	bool GenerateBackgroundResources(boost::filesystem::path const & path);

	bool InitializeSpriteManagers();

	bool LoadSpriteLayerInfo(nlohmann::json const & json_entry);

	bool InitializeGamepadManager();

	void GameOver();

	void OnGameStarted();

	bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

	class SpriteLayer * FindSpriteLayer(int layer);

	class SpriteLayer const * FindSpriteLayer(int layer) const;

	glm::vec2 GetPlayerScreenPosition() const;

	glm::vec2 GetPlayerInitialScreenPosition() const;

	glm::vec2 GetWorldInitialPosition() const;

	void SetPlayerScreenPosition(glm::vec2 const & in_position);

	class ObjectDefinition const * FindObjectDefinition(int id) const;

	class Particle const * GetPlayerParticle() const;

	class Particle * GetPlayerParticle();

	void UpdatePlayerDisplacement(float delta_time);

	void UpdateWorldAndPlayerPosition(float delta_time, glm::vec2 const & direction);

	bool FindPlayerCollision();

	bool OnCollision(Particle & p, SpriteLayer & layer);

	void ResetPlayerCachedInputs();

	bool InitializeSounds(boost::filesystem::path const & resource_path);
	
  bool LoadBackgroundTexture(size_t index);

	chaos::box2 GetWorldBox(bool use_padding) const; 

protected:

	std::vector<SpriteLayer> sprite_layers;

	boost::intrusive_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;

	std::vector<ObjectDefinition> object_definitions;

	boost::intrusive_ptr<MyGamepadManager> gamepad_manager;

	// screen information
	glm::vec2 world_size;

	glm::vec2 world_padding_ratio = glm::vec2(1.0f, 2.0f); // world_padding is relative to the world size

	// game values
	int   life  = 0;
	int   level = 0;
	float player_screen_speed = 0.0f;   
	float player_absolute_speed = 0.0f; 
  float max_speed = 500.0f;
  float acceleration_factor = 500.0f;
  float slowdown_factor = 500.0f;
	glm::vec2 world_position = glm::vec2(0.0f, 0.0f); 
  glm::vec2 player_speed = glm::vec2(0.0f, 0.0f);
	
	// initial values
	int   level_particle_increment = 10;
	int   max_particles_per_frame = 100;
	int   initial_life  = 50;
	int   initial_level = 0;
	float initial_player_screen_speed = 500.0f;   // speed at which the player may move on screen
	float initial_player_absolute_speed = 50.0f; // speed at which the player is push forward by its engine
	float delta_speed = 7.0f;                    // the 'absolute_speed' increase that is applyed when power up is taken
	
	glm::vec2 screen_safe_aera = glm::vec2(0.5f, 0.9f);

	// game state
	bool game_paused  = false;
	bool game_started = false;
	bool pending_gameover = false;
	bool pending_restart_game = false; // we don't want to restart the game during a complicated call stack => wait for next tick

	/** the window in GLFW library */
	GLFWwindow * glfw_window = nullptr;

	// stick position
	glm::vec2 stick_position = glm::vec2(0.0f, 0.0f);

	// background data
	boost::intrusive_ptr<chaos::SimpleMesh> fullscreen_mesh;

	boost::intrusive_ptr<chaos::GPUProgram>  background_program;	
	boost::intrusive_ptr<chaos::Texture>    background_texture;
	boost::intrusive_ptr<chaos::GPUProgram>  control_program;
	boost::intrusive_ptr<chaos::Texture>    control_texture;

  std::vector<boost::filesystem::path> background_paths;
  size_t background_index = 0;

	// sounds
	boost::intrusive_ptr<chaos::SoundManager> sound_manager;
	boost::intrusive_ptr<chaos::SoundSource> music_source;
	boost::intrusive_ptr<chaos::SoundSource> gameover_source;
	boost::intrusive_ptr<chaos::SoundSource> collision_source;
	boost::intrusive_ptr<chaos::SoundSource> bonus1_source;
	boost::intrusive_ptr<chaos::SoundSource> bonus2_source;
	boost::intrusive_ptr<chaos::SoundSource> start_source;
	boost::intrusive_ptr<chaos::SoundSource> pause_source;
};
