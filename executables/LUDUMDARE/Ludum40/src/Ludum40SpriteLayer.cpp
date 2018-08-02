#include "Ludum40Game.h"


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
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/SoundManager.h>
#include <json.hpp>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/JSONTools.h>

#include "Ludum40SpriteManager.h"
#include "Ludum40SpriteLayer.h"

// Particles position stored relative to camera

// ======================================================================================

bool SpriteLayer::LoadFromJSON(nlohmann::json const & json_entry)
{
	chaos::JSONTools::GetAttribute(json_entry, "name", name, "noname");
	chaos::JSONTools::GetAttribute(json_entry, "layer", layer, 0);
	chaos::JSONTools::GetAttribute(json_entry, "relative_speed", relative_speed, 1.0f);
	chaos::JSONTools::GetAttribute(json_entry, "start_visible", start_visible, true);
	chaos::JSONTools::GetAttribute(json_entry, "collision_type", collision_type, 1);
	chaos::JSONTools::GetAttribute(json_entry, "min_particle_count", min_particle_count, 0);
	chaos::JSONTools::GetAttribute(json_entry, "max_particle_count", max_particle_count, 0);

	return true;
}

// ======================================================================================

bool ObjectDefinition::LoadFromJSON(nlohmann::json const & json_entry)
{
	chaos::JSONTools::GetAttribute(json_entry, "id", id, 0);
	chaos::JSONTools::GetAttribute(json_entry, "layer", layer, 0);
	chaos::JSONTools::GetAttribute(json_entry, "size", size, 1.0f);
	chaos::JSONTools::GetAttribute(json_entry, "bitmap", bitmap_path, "");
	chaos::JSONTools::GetAttribute(json_entry, "min_lifetime", min_lifetime, 0.0f);
	chaos::JSONTools::GetAttribute(json_entry, "max_lifetime", max_lifetime, 0.0f);
	chaos::JSONTools::GetAttribute(json_entry, "spawn_type", spawn_type, SPAWN_TYPE_CENTER);

	return true;
}

// ======================================================================================

GameInfo::GameInfo(class Game const & game):
	texture_atlas(game.texture_atlas.get()),
	object_definitions(game.object_definitions),
	world_box(game.GetWorldBox(false)),
	world_box_padding(game.GetWorldBox(true))
{}

ObjectDefinition const * GameInfo::GetObjectDefinition(int id)
{
	for (ObjectDefinition const & def : object_definitions)
		if (def.id == id)
			return &def;
	return nullptr;
}

// ======================================================================================


void SpriteLayer::Tick(double delta_time, GameInfo game_info)
{
	if (!visible)
		return;
	UpdateParticleLifetime(delta_time);
	UpdateParticleVelocity(delta_time);
	DestroyParticleByClipRect(game_info);
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

void SpriteLayer::DestroyParticleByClipRect(GameInfo game_info)
{
	chaos::box2 clip_rect = game_info.world_box_padding; // set clip in screen space
	clip_rect.position -= game_info.world_box.position;

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


// We want the different type of particles in a same layers to be in equal numbers
// => get the number of particles by type ID, try population for types of lower count first
std::vector<std::pair<int, size_t>> SpriteLayer::GetSpritePopulationStats(GameInfo game_info) const
{
	std::map<int, size_t> stats;

	// first add 0 for all types of the layers
	for (size_t i = 0 ; i < game_info.object_definitions.size() ; ++i)
	{
		ObjectDefinition const & def = game_info.object_definitions[i];
		if (def.layer != layer)
			continue;
		stats[def.id] = 0;	
	}

	// count the number of particles per object type
	size_t particle_count = particles.size();

	std::map<int, size_t> definitions_map; // MAP [OBJECT_TYPE => NUMBER OF PARTICLES]
	for (size_t i = 0 ; i < particle_count ; ++i)
	{
		// group costly map accesses by counting consecutive particles of same type
		int current_id = particles[i].id;

		size_t j = i + 1;
		while (j < particle_count && current_id == particles[j].id)
			++j;

		stats[current_id] += (j - i);
		i = j;
	}

	// convert map into a sorted vector
	std::vector<std::pair<int, size_t>> result;
	result.reserve(stats.size());
	for (auto stat : stats)
		result.push_back(stat);

	using Obj = std::pair<int, size_t>;

	std::sort(result.begin(), result.end(), [](Obj const & obj1, Obj const & obj2){
		return (obj1.second < obj2.second);			
	});

	return result;
}

void SpriteLayer::PopulateSprites(GameInfo game_info, int count)
{
	chaos::BitmapAtlas::BitmapSet const * bitmap_set = game_info.texture_atlas->GetBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return;

	std::vector<std::pair<int, size_t>> sprite_stats = GetSpritePopulationStats(game_info);

	for (size_t i = 0 ; (i < sprite_stats.size()) && (count > 0) ; ++i)
	{
		int obj_id = sprite_stats[i].first;

		ObjectDefinition const * def = game_info.GetObjectDefinition(obj_id);
		if (def == nullptr)
			continue;
		PopulateSpritesWithDef(game_info, count, *def);
	}
}

void SpriteLayer::PopulateSpritesWithDef(GameInfo game_info, int & count, ObjectDefinition const & def)
{
	chaos::BitmapAtlas::BitmapSet const * bitmap_set = game_info.texture_atlas->GetBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return;

	chaos::BitmapAtlas::BitmapEntry const * bitmap_entry = bitmap_set->GetEntry(def.id);  // texturing info required to get a ratio between width & height
	if (bitmap_entry == nullptr)
		return;

	if (bitmap_entry->height <= 0.0f || bitmap_entry->width <= 0.0f) // "empty particles" => ignore
		return;

	float ratio = chaos::MathTools::CastAndDiv<float>(bitmap_entry->height, bitmap_entry->width);

	// generate the particles
	Particle p;
	p.id = def.id;
	p.life_time = 0.0f;
	p.velocity = glm::vec2(0.0f, 0.0f);
	p.half_size = 0.5f * glm::vec2(def.size, def.size * ratio);

	float world_distance_2 = glm::length2(2.0f * game_info.world_box.half_size);

	if (def.spawn_type == ObjectDefinition::SPAWN_TYPE_CENTER)
	{
		while (count > 0)
		{
			p.position = glm::vec2(0.0f, 0.0f); // screen space particle at center
			particles.push_back(p);
			--count;
		}		
	}
	else if (def.spawn_type == ObjectDefinition::SPAWN_TYPE_OUTASCREEN || def.spawn_type == ObjectDefinition::SPAWN_TYPE_OUTASCREEN_TESTCOLLISION)
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

		// add some padding so that particles are never spawned directly on screen
		zones[0].position += p.half_size * glm::vec2(-1.0f, 1.0f);
		zones[1].position += p.half_size * glm::vec2( 0.0f, 1.0f);
		zones[2].position += p.half_size * glm::vec2(+1.0f, 1.0f);
		zones[3].position += p.half_size * glm::vec2(-1.0f, 0.0f);
		zones[4].position += p.half_size * glm::vec2(+1.0f, 0.0f);

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

		while (count > 0)
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

			p.position = 
				zones[surface_index].position + 
				zones[surface_index].half_size * random - world_box.position; // screen space position

			--count; // decrement even if the particle is refused

			bool add_particle = true;
			if (def.spawn_type == ObjectDefinition::SPAWN_TYPE_OUTASCREEN_TESTCOLLISION) // search any other object that is less than one screen away
			{
				for (size_t i = 0 ; i < particles.size() ; ++i)
				{
					float particles_distance_2 = glm::length2(p.position - particles[i].position);
					if (particles_distance_2 < world_distance_2)
					{
						add_particle = false;
						break;
					}				
				}									
			}

			if (add_particle)
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

	chaos::BitmapAtlas::BitmapSet const * bitmap_set = game_info.texture_atlas->GetBitmapSet("sprites");
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

void SpriteLayer::Draw(chaos::GPUProgramProviderBase * uniform_provider)
{
	if (visible)
		sprite_manager->Display(uniform_provider);
}
