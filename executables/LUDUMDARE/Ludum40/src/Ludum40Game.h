#pragma once

#include "Ludum40PCH.h"
#include "Ludum40SpriteManager.h"
#include "Ludum40SpriteLayer.h"

// ======================================================================================

class MyGamepadManager : public chaos::GamepadManager
{
public:

	MyGamepadManager(class Game * in_game) : game(in_game) {}

protected:

	virtual bool DoPollGamepad(chaos::PhysicalGamepad * physical_gamepad) override;

protected:

	class Game * game = nullptr;
};

// ======================================================================================

class Game : public chaos::Object
{
	static constexpr int PLAYER_LAYER = -1;
	static constexpr int PAUSED_OBJECT_LAYER = -3;
	static constexpr int TITLE_OBJECT_LAYER = -2;
	static constexpr int GAMEOVER_OBJECT_LAYER = -4;

	friend class GameInfo;

	friend class MyGamepadManager;

public:

	void Tick(float delta_time);

	bool Initialize(chaos::Window * in_window, nlohmann::json const * config, glm::vec2 const & in_world_size);

	void Finalize();

	void Display(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size);

	void SetPause(bool in_paused);

	void StartGame();

	void ResetWorld();

	bool OnKeyEvent(chaos::Key key, chaos::KeyAction action);


	chaos::Window* GetWindow() { return window; }

protected:

  void InitializeFromConfiguration(nlohmann::json const * config);

	void UpdateParticlesPosition(float delta_time, glm::vec2 delta_pos);

	void SpawnExtraParticles(GameInfo game_info, float delta_time);

	void DisplaySprites(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size);

	void DisplayBackground(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size);
	void DisplayControls(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size);
	void DisplayFullscreen(chaos::GPURenderContext * render_context, glm::ivec2 viewport_size, chaos::shared_ptr<chaos::GPUTexture> texture, chaos::shared_ptr<chaos::GPUProgram> program);

	void SetLayerVisibility(int layer, bool visible);



	bool DoInitialize(boost::filesystem::path const & resource_path, boost::filesystem::path const & object_path, nlohmann::json const * json);

	bool LoadObjectDefinition(nlohmann::json const * json);

	bool GenerateAtlas(boost::filesystem::path const & path);

	bool GenerateBackgroundResources(boost::filesystem::path const & path);

	bool InitializeSpriteManagers();

	bool LoadSpriteLayerInfo(nlohmann::json const * json);

	bool InitializeGamepadManager();

	void GameOver();

	void OnGameStarted();

	bool OnPhysicalGamepadInput(chaos::PhysicalGamepad * physical_gamepad);

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

	chaos::shared_ptr<chaos::GPUAtlas> texture_atlas;

	std::vector<ObjectDefinition> object_definitions;

	chaos::shared_ptr<MyGamepadManager> gamepad_manager;

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
	float delta_speed = 7.0f;                    // the 'absolute_speed' increase that is applied when power up is taken

	glm::vec2 screen_safe_aera = glm::vec2(0.5f, 0.9f);

	// game state
	bool game_paused  = false;
	bool game_started = false;
	bool pending_gameover = false;
	bool pending_restart_game = false; // we don't want to restart the game during a complicated call stack => wait for next tick

	/** the window in GLFW library */
	chaos::Window * window = nullptr;

	// stick position
	glm::vec2 stick_position = glm::vec2(0.0f, 0.0f);

	// background data
	chaos::shared_ptr<chaos::GPUMesh> fullscreen_mesh;

	chaos::shared_ptr<chaos::GPUProgram>  background_program;
	chaos::shared_ptr<chaos::GPUTexture>    background_texture;
	chaos::shared_ptr<chaos::GPUProgram>  control_program;
	chaos::shared_ptr<chaos::GPUTexture>    control_texture;

  std::vector<boost::filesystem::path> background_paths;
  size_t background_index = 0;

	// sounds
	chaos::shared_ptr<chaos::SoundManager> sound_manager;
	chaos::shared_ptr<chaos::SoundSource> music_source;
	chaos::shared_ptr<chaos::SoundSource> gameover_source;
	chaos::shared_ptr<chaos::SoundSource> collision_source;
	chaos::shared_ptr<chaos::SoundSource> bonus1_source;
	chaos::shared_ptr<chaos::SoundSource> bonus2_source;
	chaos::shared_ptr<chaos::SoundSource> start_source;
	chaos::shared_ptr<chaos::SoundSource> pause_source;
};
