#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/TextureArrayAtlas.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>

#include <death/GameHUDKeys.h>

namespace death
{
	class GameParticleCreator
	{
	public:

		/** initialization of the particle creator */
		bool Initialize(chaos::ParticleManager * in_particle_manager, chaos::ParticleTextGenerator::Generator * in_particle_text_generator, chaos::BitmapAtlas::TextureArrayAtlas * in_texture_atlas);

		/** spawn some objects */
		chaos::ParticleAllocation * SpawnParticles(chaos::TagType layer_id, size_t count) const;

		/** create some particle of the given type (Spawn + Initialization) */
		chaos::ParticleAllocation * CreateParticles(char const * bitmap_name, size_t count, chaos::TagType layer_id) const;
		/** create a text particle system */
		chaos::ParticleAllocation * CreateTextParticles(char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params, chaos::TagType layer_id = death::GameHUDKeys::TEXT_LAYER_ID) const; 		
		/** create a title */
		chaos::ParticleAllocation * CreateTitle(char const * title, bool normal, chaos::TagType layer_id = death::GameHUDKeys::TEXT_LAYER_ID) const;
		/** create a score text at the top left corner */
		chaos::ParticleAllocation * CreateScoringText(char const * format, int value, float Y, chaos::box2 const & view, chaos::TagType layer_id = death::GameHUDKeys::TEXT_LAYER_ID) const;

		/** initialize the game objects texture coordinates (from the end of the array) */
		bool InitializeParticles(chaos::ParticleAllocation * allocation, char const * bitmap_name, size_t last_count) const;

	protected:

		/** the particle manager */
		boost::intrusive_ptr<chaos::ParticleManager> particle_manager;
		/** the text generator */
		boost::intrusive_ptr<chaos::ParticleTextGenerator::Generator> particle_text_generator;
		/** the texture atlas */
		boost::intrusive_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;
	};

}; //namespace death

