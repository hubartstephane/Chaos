#include "game.h"

#include <chaos/StandardHeaders.h> 
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

// ======================================================================================

bool ObjectDefinition::LoadFromJSON(nlohmann::json const & json_entry)
{
	id = json_entry.value("id", 0);
	layer = json_entry.value("layer", 0);
	scale = json_entry.value("scale", 1.0f);
	bitmap_path = json_entry.value("bitmap", "");

	return true;
}

// ======================================================================================


void SpriteLayer::Tick(double delta_time, chaos::box2 const * clip_rect)
{
	UpdateParticleLifetime(delta_time);
	UpdateParticleVelocity(delta_time);
	DestroyParticleByClipRect(clip_rect);
	UpdateGPUBuffer();	
}

void SpriteLayer::UpdateParticleLifetime(double delta_time)
{
	float dt = (float)delta_time;

	size_t i = 0;
	while (i < particles.size())
	{
		if (particles[i].life_time > 0.0f)
		{
			particles[i].life_time -= dt;
			if (particles[i].life_time <= 0.0f)
			{
				particles[i] = particles.back();
				particles.pop_back();
				continue;
			}
		}				
		++i;
	}
}

void SpriteLayer::UpdateParticleVelocity(double delta_time)
{
	float dt = (float)delta_time;
	
	for (size_t i = 0 ; i < particles.size() ; ++i)
		particles[i].position += particles[i].velocity * dt;
}

void SpriteLayer::DestroyParticleByClipRect(chaos::box2 const * in_clip_rect)
{
	if (in_clip_rect != nullptr)
	{
		chaos::box2 clip_rect = *in_clip_rect;

		size_t i = 0;
		while (i < particles.size())
		{
			chaos::box2 particle_box = chaos::box2(particles[i].position, particles[i].half_size);

			if (!chaos::Collide(clip_rect, particle_box))
			{
				particles[i] = particles.back();
				particles.pop_back();
				continue;
			}
			++i;
		}
	}
}




void SpriteLayer::UpdateGPUBuffer()
{

	sprite_manager->ClearSprites(); // remove all GPU buffer data

	for (size_t i = 0 ; i < particles.size() ; ++i)
	{
		//sprite_manager->AddSpriteBitmap()
	
	}
}

void SpriteLayer::Draw(chaos::GLProgramVariableProviderChain & uniform_provider)
{


}

// ======================================================================================

void Game::Tick(double delta_time, chaos::box2 const * clip_rect)
{
	for(size_t i = 0 ; i < sprite_layers.size() ; ++i)
		sprite_layers[i].Tick(delta_time, clip_rect);



	

}

bool Game::Initialize(boost::filesystem::path const & path)
{
	boost::filesystem::path object_path = path / "objects";

	if (!LoadObjectDefinition(object_path / "objects.json"))
		return false;
	if (!GenerateAtlas(object_path))
		return false;
	if (!GenerateSpriteGPUProgram(path))
		return false;
	if (!GenerateSpriteLayers())
		return false;

	return true;
}

void Game::Finalize()
{
	sprite_layers.clear();
	object_definitions.clear();
	texture_atlas.Clear();

	sprite_program = nullptr;
}

SpriteLayer * Game::FindSpriteLayer(int layer)
{
	for (size_t i = 0 ; i < sprite_layers.size(); ++i)
		if (sprite_layers[i].layer == layer)
			return &sprite_layers[i];
	return nullptr;
}

bool Game::GenerateSpriteLayers()
{
	chaos::SpriteManagerInitParams sprite_params;
	sprite_params.atlas = &texture_atlas;
	sprite_params.program = sprite_program;

	for (size_t i = 0 ; i < object_definitions.size() ; ++i)
	{
		int object_layer = object_definitions[i].layer;

		SpriteLayer * sprite_layer = FindSpriteLayer(object_layer);
		if (sprite_layer == nullptr)
		{		
			boost::intrusive_ptr<chaos::SpriteManager> sprite_manager = new chaos::SpriteManager();
			if (sprite_manager == nullptr)
				return false;			
			if (!sprite_manager->Initialize(sprite_params))
				return false;	

			SpriteLayer sl;
			sl.layer = object_layer;
			sl.sprite_manager = sprite_manager;

			sprite_layers.push_back(std::move(sl));
			
			sprite_layer = &sprite_layers.back();
		} 
	}

	// sort the layers
	std::sort(sprite_layers.begin(), sprite_layers.end(), [](SpriteLayer const & obj1, SpriteLayer const & obj2){	
		return (obj1.layer < obj2.layer);			
	});

	return true;
}

bool Game::GenerateSpriteGPUProgram(boost::filesystem::path const & path)
{
	chaos::GLProgramLoader loader;

	loader.AddShaderSourceFile(GL_VERTEX_SHADER, (path / "sprite_vertex_shader.txt").string().c_str());
	loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, (path / "sprite_pixel_shader.txt").string().c_str());

	sprite_program = loader.GenerateProgramObject();
	if (sprite_program == nullptr)
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
		chaos::WinTools::ShowFile(dst_p.string().c_str());
	}
#endif

	// generate texture Atlas
	if (!texture_atlas.LoadFromBitmapAtlas(atlas))
		return false;

	return true;
}

bool Game::LoadObjectDefinition(boost::filesystem::path const & path)
{
	chaos::Buffer<char> buf = chaos::FileTools::LoadFile(path, true);
	if (buf == nullptr)
		return false;

	try
	{
		nlohmann::json json_entry = nlohmann::json::parse(buf.data);
		return LoadObjectDefinition(json_entry);
	}
	catch(...)
	{

	}
	return false;
}

bool Game::LoadObjectDefinition(nlohmann::json const & json_entry)
{
	nlohmann::json objects = json_entry["objects"];

	for (auto const & obj : objects)
	{
		ObjectDefinition def;
		if (!def.LoadFromJSON(obj))
			continue;
		object_definitions.push_back(std::move(def));
	}
	return true;
}

void Game::Display(glm::ivec2 size)
{	
	chaos::GLProgramVariableProviderChain uniform_provider;

	for (int i = sprite_layers.size() - 1 ; i >= 0; --i)
		sprite_layers[i].Draw(uniform_provider);



#if 0
	chaos::GLProgramVariableProviderChain uniform_provider;
	uniform_provider.AddVariableValue("local_to_cam", local_to_cam);

	sprite_manager.Display(&uniform_provider);
#endif
}
