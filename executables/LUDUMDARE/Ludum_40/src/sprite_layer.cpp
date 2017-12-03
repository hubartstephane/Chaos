#include "game.h"


#include <chaos/StandardHeaders.h> 
#include <chaos/MathTools.h> 
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

bool ObjectDefinition::LoadFromJSON(nlohmann::json const & json_entry)
{
	id = json_entry.value("id", 0);
	layer = json_entry.value("layer", 0);
	size = json_entry.value("size", 1.0f);
	bitmap_path = json_entry.value("bitmap", "");

	initial_particle_count = json_entry.value("initial_particle_count", 0);
	min_lifetime = json_entry.value("min_lifetime", 0.0f);
	max_lifetime = json_entry.value("max_lifetime", 0.0f);

	return true;
}

// ======================================================================================

GameInfo::GameInfo(class Game const & game):
	texture_atlas(game.texture_atlas),
	object_definitions(game.object_definitions){}

// ======================================================================================


void SpriteLayer::Tick(double delta_time, GameInfo game_info, chaos::box2 const * clip_rect)
{
	if (!visible)
		return;
	UpdateParticleLifetime(delta_time);
	UpdateParticleVelocity(delta_time);
	DestroyParticleByClipRect(clip_rect);
	UpdateGPUBuffer(game_info);	
}

void SpriteLayer::SetVisible(bool in_visible)
{
	visible = in_visible;
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

void SpriteLayer::DestroyAllParticles()
{
	particles.clear();
}

void SpriteLayer::InitialPopulateSprites(GameInfo game_info)
{
	chaos::BitmapAtlas::BitmapSet const * bitmap_set = game_info.texture_atlas.GetBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return;

	for (ObjectDefinition const & def : game_info.object_definitions)
	{
		if (def.layer != layer) // only our layer
			continue;
		if (def.initial_particle_count == 0) // only if some particles are to be created
			continue;

		chaos::BitmapAtlas::BitmapEntry const * bitmap_entry = bitmap_set->GetEntry(def.id);  // texturing info required to get a ratio between width & height
		if (bitmap_entry == nullptr)
			continue;

		if (bitmap_entry->height <= 0.0f || bitmap_entry->width <= 0.0f) // "empty particles" => ignore
			continue;

		float ratio = chaos::MathTools::CastAndDiv<float>(bitmap_entry->height, bitmap_entry->width);

		// generate the particles
		static float SCALE = 1.0f;

		Particle p;
		p.id = def.id;
		p.life_time = 0.0f;
		p.half_size = 0.5f * SCALE * glm::vec2(def.size, def.size * ratio);
		for (int i = 0 ; i < def.initial_particle_count ; ++i)
		{
			p.position = glm::vec2(0.0f, 0.0f);
			p.velocity = glm::vec2(0.0f, 0.0f);
			particles.push_back(p);
		}
	}
}

void SpriteLayer::UpdateGPUBuffer(GameInfo game_info)
{	
	sprite_manager->ClearSprites(); // remove all GPU buffer data

									// the buffer stores particles that share the layer value, but not the 'type'
									// When we want to add data in GPU buffer, we have to Find texture data (may be costly)
									// This algo uses another approch to avoid that

	chaos::BitmapAtlas::BitmapSet const * bitmap_set = game_info.texture_atlas.GetBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return;

	for (ObjectDefinition const & def : game_info.object_definitions)  // take all object definitions of the whole GAME
	{
		if (def.layer != layer) // manage only the ones of concerns (layer number consideration) 
			continue;

		int id = def.id;

		chaos::BitmapAtlas::BitmapEntry const * bitmap_entry = bitmap_set->GetEntry(id); // search data corresponding the the model of this sprite
		if (bitmap_entry == nullptr)
			continue;

		size_t particle_count = particles.size();
		for (size_t j = 0 ; j < particle_count ; ++j)
		{
			Particle const & p = particles[j]; // only manage the particles corresponding to this model of sprite
			if (p.id != id)
				continue;
			sprite_manager->AddSpriteBitmap(bitmap_entry, p.position, 2.0f * p.half_size, chaos::Hotpoint::CENTER);
		}
	}
}

void SpriteLayer::Draw(chaos::GLProgramVariableProvider * uniform_provider)
{
	if (layer == -3)
		layer = layer;

	if (visible)
		sprite_manager->Display(uniform_provider);
}
