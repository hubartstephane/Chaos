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

bool SpriteLayer::LoadFromJSON(nlohmann::json const & json_entry)
{
	name = json_entry.value("name", "noname");
	layer = json_entry.value("layer", 0);
	relative_speed = json_entry.value("relative_speed", 1.0f);
	start_visible = json_entry.value("start_visible", 1) > 0;
	collision = json_entry.value("collision", 1) > 0;
	max_particle_count = json_entry.value("max_particle_count", 0);

	return true;
}

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

	spawn_type = json_entry.value("spawn_type", SPAWN_TYPE_CENTER);

	return true;
}

// ======================================================================================

GameInfo::GameInfo(class Game const & game):
	texture_atlas(game.texture_atlas),
	object_definitions(game.object_definitions),
	world_box(game.GetWorldBBox(false)),
	world_box_padding(game.GetWorldBBox(true))
{}

// ======================================================================================


void SpriteLayer::Tick(double delta_time, GameInfo game_info)
{
	if (!visible)
		return;
	UpdateParticleLifetime(delta_time);
	UpdateParticleVelocity(delta_time);
	DestroyParticleByClipRect(game_info.world_box_padding);
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

void SpriteLayer::DestroyParticleByClipRect(chaos::box2 const & clip_rect)
{
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
		p.velocity = glm::vec2(0.0f, 0.0f);
		p.half_size = 0.5f * SCALE * glm::vec2(def.size, def.size * ratio);

		if (def.spawn_type == ObjectDefinition::SPAWN_TYPE_CENTER)
		{
			for (int i = 0 ; i < def.initial_particle_count ; ++i)
			{
				p.position = game_info.world_box.position;				

				particles.push_back(p);
			}			
		}
		else if (def.spawn_type == ObjectDefinition::SPAWN_TYPE_OUTASCREEN)
		{
			// compute the 5 spawn zone probabilities => compute their relative surface
			chaos::box2 world_box         = game_info.world_box;
			chaos::box2 world_box_padding = game_info.world_box_padding;

			float half_world_x = world_box.half_size.x; 
			float half_world_y = world_box.half_size.y;

			float extra_x = (1.0f * world_box_padding.half_size.x) - (1.0f * half_world_x);
			float extra_y = (2.0f * world_box_padding.half_size.y) - (2.0f * half_world_y);
			float world_x = (2.0f * half_world_x);
			float world_y = (2.0f * half_world_y);

			float half_extra_x = 0.5f * extra_x; 
			float half_extra_y = 0.5f * extra_y; 
			
			chaos::box2 zones[5];

			// compute zone x
			zones[0].position.x = zones[3].position.x =
				world_box.position.x - half_world_x - half_extra_x;
			zones[1].position.x = 
				world_box.position.x;
			zones[2].position.x = zones[4].position.x =
				world_box.position.x + half_world_x + half_extra_x;

			// compute zone y
			zones[0].position.y = zones[1].position.y = zones[2].position.y =
				world_box.position.y + half_world_y + half_extra_y;
			zones[3].position.y = zones[4].position.y =
				world_box.position.y;

			// compute the half_size
			zones[0].half_size = zones[2].half_size = 
				glm::vec2(half_extra_x, half_extra_y);
			zones[1].half_size = 
				glm::vec2(half_world_x, half_extra_y);
			zones[3].half_size = zones[4].half_size = 
				glm::vec2(half_extra_x, half_world_y);

			// compute the surfaces and sum then renormalize to have a propability repartition
			float surfaces[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

			float surface_sum = 0.0f;
			for (int i = 0 ; i < 5 ; ++i)
			{
				float s = chaos::GetSurface(zones[i]);
				surfaces[i] = s;
				surface_sum += s;			
			}

			for (int i = 0 ; i < 5 ; ++i)  // renormalize 
				surfaces[i] /= surface_sum;

			for (int i = 0 ; i < def.initial_particle_count ; ++i)
			{
				float surface_rand = chaos::MathTools::RandFloat();

				// find the wanted surface
				int surface_index = 0;
				while (surface_index < 5 && surface_rand > surfaces[surface_index])
				{
					surface_rand -= surfaces[surface_index]; 
					++surface_index;				
				}
				if (surface_index == 5)
					continue;

				glm::vec2 random = 2.0f * chaos::GLMTools::RandVec2() - glm::vec2(1.0f, 1.0f);  // random numbers between -1 and +1

				p.position = zones[surface_index].position + zones[surface_index].half_size * random;
				
				particles.push_back(p);
			}		
		}
		else if (def.spawn_type == ObjectDefinition::SPAWN_TYPE_BACKGROUND)
		{
			for (int i = 0 ; i < def.initial_particle_count * 5; ++i)
			{
				p.position = glm::vec2(0.0f, 0.0f);
				particles.push_back(p);
			}		
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
	if (visible)
		sprite_manager->Display(uniform_provider);
}
