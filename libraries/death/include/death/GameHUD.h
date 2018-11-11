#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ParticleManager.h>
#include <chaos/Renderable.h>
#include <chaos/NamedObject.h>

namespace death
{
	// Create a gamespace for unique idenfiers
	namespace GameHUDKeys
	{		
		CHAOS_DECLARE_TAG(TITLE_ID);
		CHAOS_DECLARE_TAG(BEST_SCORE_ID);
	};

	class GameHUD : public chaos::Renderable
	{
	public:

		/** initialization method : set the particle manager and use it */
		bool Initialize(chaos::ParticleManager * in_particle_manager, bool in_external_manager);
		/** initialization method : create a new particle manager using given atlas */
		bool Initialize(chaos::BitmapAtlas::TextureArrayAtlas * in_texture_atlas);

		/** insert some particles inside the HUD */
		void RegisterParticles(chaos::TagType key, chaos::ParticleAllocation * allocation, bool remove_previous = true);
		/** remove some particles from the HUD */
		void UnregisterParticles(chaos::TagType key);
		/** clear all particles from the HUD */
		void Clear();


		bool IsDynamicParticlesRequiringUpdate(chaos::TagType key, int new_value);

		/** update a formated text parameter */
		chaos::ParticleAllocation * UpdateDynamicParticles(chaos::TagType key, int new_value);

		bool CreateDynamicParticles(chaos::TagType, int initial_value);


		/** get the particle manager */
		chaos::ParticleManager * GetParticleManager() { return particle_manager.get(); }
		/** get the particle manager */
		chaos::ParticleManager const * GetParticleManager() const { return particle_manager.get(); }

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;
		/** override */
		virtual int DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;

	protected:

		/** the allocations */
		std::multimap<chaos::TagType, boost::intrusive_ptr<chaos::ParticleAllocation>> particle_allocations;

		/** the particle manager */
		boost::intrusive_ptr<chaos::ParticleManager> particle_manager;
		/** indicates whether THIS is responsible or not of rendering/ticking the particle manager */
		bool external_manager = false;
	};

	class MainMenuHUD : public GameHUD
	{
	public:

	};

	class PauseMenuHUD : public GameHUD
	{
	public:

	};

	class GameOverHUD : public GameHUD
	{
	public:

	};

	class PlayingHUD : public GameHUD
	{

	public:

		void SetScoreValue(class Game * game, int new_score);

	protected:

		void CacheAndCreateScoreAllocation(class Game * game, int value, char const * format, float Y, int & cached_value, chaos::TagType key);

	protected:

		int cached_score_value = -1;
	};

}; // namespace death
