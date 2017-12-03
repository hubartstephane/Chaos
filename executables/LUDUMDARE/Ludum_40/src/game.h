#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramVariableProvider.h>
#include <chaos/SoundManager.h>
#include <json.hpp>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/SpriteManager.h>


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
	static const int PLAYER_OBJECT_ID = 0;

	static const int PLAYER_LAYER = -1;
	static const int TITLE_OBJECT_LAYER = -2;
	static const int PAUSED_OBJECT_LAYER = -3;

	friend class GameInfo;

	friend class MyGamepadManager;

public:

	void Tick(double delta_time, chaos::box2 const * clip_rect);

	bool Initialize(GLFWwindow * in_glfw_window, glm::vec2 const & in_world_size, boost::filesystem::path const & path);

	void Finalize();

	void Display(glm::ivec2 viewport_size);

	void SetPause(bool in_paused);

	void StartGame();

	void ResetWorld();

	bool OnKeyEvent(int key, int action);

protected:

	void DisplaySprites(glm::ivec2 viewport_size);

	void DisplayBackground(glm::ivec2 viewport_size);

	void SetLayerVisibility(int layer, bool visible);



	bool DoInitialize(boost::filesystem::path const & resource_path, boost::filesystem::path const & object_path, nlohmann::json const & json_entry);

	bool LoadObjectDefinition(nlohmann::json const & json_entry);

	bool GenerateAtlas(boost::filesystem::path const & path);

	bool GenerateBackgroundResources(boost::filesystem::path const & path);

	bool InitializeSpriteManagers();

	bool LoadSpriteLayerInfo(nlohmann::json const & json_entry);

	bool InitializeGamepadManager();



	void OnGameStarted();

	bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

	class SpriteLayer * FindSpriteLayer(int layer);

	class SpriteLayer const * FindSpriteLayer(int layer) const;

	glm::vec2 GetPlayerPosition() const;

	glm::vec2 GetPlayerInitialScreenPosition() const;

	void SetPlayerPosition(glm::vec2 const & in_position);

	class ObjectDefinition const * FindObjectDefinition(int id) const;

	class Particle const * GetPlayerParticle() const;

	class Particle * GetPlayerParticle();

	void UpdatePlayerDisplacement(float delta_time);

	void UpdateWorldDisplacement(float delta_time);

	void ResetPlayerCachedInputs();

	void ApplyStickDisplacement(float delta_time, glm::vec2 const & direction);


	chaos::box2 GetWorldBBox(bool use_padding) const; 

protected:

	std::vector<SpriteLayer> sprite_layers;

	chaos::BitmapAtlas::TextureArrayAtlas texture_atlas;

	std::vector<ObjectDefinition> object_definitions;

	boost::intrusive_ptr<MyGamepadManager> gamepad_manager;



	glm::vec2 world_size;

	glm::vec2 world_padding_ratio = glm::vec2(1.0f, 2.0f); // world_padding is relative to the world size

	glm::vec2 player_screen_position = glm::vec2(0.0f, 0.0f);

	glm::vec2 world_position = glm::vec2(0.0f, 0.0f);

	float world_speed = 30.0f;

	float player_speed = 500.0f;
	
	// some in game values


	// initial values
	float player_initial_speed = 500.0f;

	float world_initial_speed = 30.0f;


	// game state
	bool game_paused = false;

	bool game_started = false;

	/** the window in GLFW library */
	GLFWwindow * glfw_window = nullptr;

	// stick position
	glm::vec2 stick_position = glm::vec2(0.0f, 0.0f);

	// background data
	boost::intrusive_ptr<chaos::GLProgram>  background_program;
	boost::intrusive_ptr<chaos::SimpleMesh> background_mesh;
	boost::intrusive_ptr<chaos::Texture>    background_texture;
};
