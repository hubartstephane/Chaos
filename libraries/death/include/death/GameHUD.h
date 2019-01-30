#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ParticleManager.h>
#include <chaos/Renderable.h>
#include <chaos/NamedObject.h>

#include <death/GameHUDKeys.h>
#include <death/GameParticleCreator.h>

namespace death
{

	class Game;
	class GameInstance;

	// =============================================
	// GameHUD
	// =============================================

	class GameHUD : public chaos::Renderable
	{
	public:

		/** constructor */
		GameHUD(class Game * in_game);

		/** getters on game */
		Game * GetGame() { return game; }
		/** getters on game */
		Game const * GetGame() const { return game; }

		/** getters on game instance */
		GameInstance * GetGameInstance();
		/** getters on game instance */
		GameInstance const * GetGameInstance() const;

		/** initialization of the HUD from the game values */
		virtual bool InitializeHUD();

		/** insert some particles inside the HUD */
		void RegisterParticles(chaos::TagType key, chaos::ParticleAllocation * allocation, bool remove_previous = true);
		/** remove some particles from the HUD */
		void UnregisterParticles(chaos::TagType key);
		/** clear all particles from the HUD */
		void Clear();

		/** get the particle allocation from its ID */
		chaos::ParticleAllocation * FindParticleAllocation(chaos::TagType key);
		/** get the particle allocation from its ID */
		chaos::ParticleAllocation const * FindParticleAllocation(chaos::TagType key) const;

		/** get the particle manager */
		chaos::ParticleManager * GetParticleManager() { return particle_manager.get(); }
		/** get the particle manager */
		chaos::ParticleManager const * GetParticleManager() const { return particle_manager.get(); }

		/** get particle tools */
		GameParticleCreator & GetGameParticleCreator(){ return particle_creator; }
		/** get particle tools */
		GameParticleCreator const & GetGameParticleCreator() const { return particle_creator; }

	protected:

		/** initialization method */
		virtual bool CreateInternalData(chaos::ParticleManager * in_particle_manager, chaos::ParticleTextGenerator::Generator * in_particle_text_generator, chaos::BitmapAtlas::TextureArrayAtlas * in_texture_atlas);
		/** create the layers */
		virtual bool CreateHUDLayers();
		/** create the particles */
		virtual bool FillHUDContent();

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;
		/** override */
		virtual int DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;

	protected:

		/** the allocations */
		std::multimap<chaos::TagType, chaos::shared_ptr<chaos::ParticleAllocation>> particle_allocations;

		/** the particle tools */
		GameParticleCreator particle_creator;

		/** the particle manager */
		chaos::shared_ptr<chaos::ParticleManager> particle_manager;
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

	protected:

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

	protected:

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

	protected:

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

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

		/** update the score particles */
		void UpdateScoreParticles();

	protected:

		/** caching the score */
		int cached_score_value = -1;
	};

}; // namespace death
