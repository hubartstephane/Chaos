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
		// some allocation ID's
		CHAOS_DECLARE_TAG(TITLE_ID);
		CHAOS_DECLARE_TAG(SCORE_ID);
		CHAOS_DECLARE_TAG(BEST_SCORE_ID);
		// some layer ID's
		CHAOS_DECLARE_TAG(GAME_LAYER_ID);
		CHAOS_DECLARE_TAG(PLAYER_LAYER_ID);
		CHAOS_DECLARE_TAG(HUD_LAYER_ID);
		CHAOS_DECLARE_TAG(TEXT_LAYER_ID);
	};

	// =============================================
	// GameHUD
	// =============================================

	class GameHUD : public chaos::Renderable
	{
	public:

		/** constructor */
		GameHUD(class Game * in_game);

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

		/** get the particle manager */
		chaos::ParticleManager * GetParticleManager() { return particle_manager.get(); }
		/** get the particle manager */
		chaos::ParticleManager const * GetParticleManager() const { return particle_manager.get(); }

		/** initialize the content of the HUD */
		virtual bool FillHUDContent();

	protected:

		/** initialization of the HUD from the game values */
		virtual bool InitializeHUD();

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

		/** the game owning the HUD */
		class Game * game = nullptr;
	};

	// =============================================
	// MainMenuHUD
	// =============================================

	class MainMenuHUD : public GameHUD
	{
	public:

		/** constructor */
		MainMenuHUD(class Game * in_game) :
			GameHUD(in_game) {}

		/** override */
		virtual bool FillHUDContent() override;
	};

	// =============================================
	// PauseMenuHUD
	// =============================================

	class PauseMenuHUD : public GameHUD
	{
	public:

		/** constructor */
		PauseMenuHUD(class Game * in_game) :
			GameHUD(in_game) {}

		/** override */
		virtual bool FillHUDContent() override;
	};

	// =============================================
	// GameOverHUD
	// =============================================

	class GameOverHUD : public GameHUD
	{
	public:

		/** constructor */
		GameOverHUD(class Game * in_game) :
			GameHUD(in_game) {}

		/** override */
		virtual bool FillHUDContent() override;
	};

	// =============================================
	// PlayingHUD
	// =============================================

	class PlayingHUD : public GameHUD
	{

	public:

		/** constructor */
		PlayingHUD(class Game * in_game) :
			GameHUD(in_game) {}

		/** override */
		virtual bool FillHUDContent() override;

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

	protected:

		/** caching the score */
		int cached_score_value = -1;
	};

}; // namespace death
