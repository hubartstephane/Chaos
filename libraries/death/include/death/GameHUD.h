#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ParticleManager.h>
#include <chaos/Renderable.h>
#include <chaos/NamedObject.h>

#include <death/GameHUDKeys.h>
#include <death/GameHUDComponent.h>
#include <death/GameFramework.h>
#include <death/GameParticleCreator.h>

namespace death
{

	// =============================================
	// GameHUD
	// =============================================

	class GameHUD : public chaos::Renderable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

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

		/** get currently played level */
		GameLevel * GetLevel();
		/** get currently played level */
		GameLevel const * GetLevel() const;

		/** get currently played level */
		GameLevelInstance * GetLevelInstance();
		/** get currently played level */
		GameLevelInstance const * GetLevelInstance() const;

		/** get the player */
		Player * GetPlayer(int player_index);
		/** get the player */
		Player const * GetPlayer(int player_index) const;

		/** initialization of the HUD from the game values */
		virtual bool InitializeHUD();

		/** insert a component inside the HUD */
		template<typename COMPONENT_TYPE, typename ...PARAMS>
		void RegisterComponent(chaos::TagType key, COMPONENT_TYPE * component, PARAMS... params)
		{
			// XXX : why is this method a template ??
			//       a COMPONENT initialization requires the HUD for some resources
			//       so you cannot build the COMPONENT before the insertion
			//       you need to do both at the same time, and you require various kind of parameters
			//       COMPONENT initialization (OnInsertedInHUD(...)) cannot be virtual because we don't know the parameters for the construction
			//       that's why we use a template function
			assert(component != nullptr);
			assert(component->hud == nullptr);
			// remove previous component with the key
			UnregisterComponent(key);
			// register new component for that key
			components.insert(std:: make_pair(key, component));
			component->hud = this;
			component->OnInsertedInHUD(params...);
		}
		/** remove a component from the HUD */
		void UnregisterComponent(chaos::TagType key);

		/** get the component from its ID */
		GameHUDComponent * FindComponent(chaos::TagType key);
		/** get the component from its ID */
		GameHUDComponent const * FindComponent(chaos::TagType key) const;







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
		std::map<chaos::TagType, chaos::shared_ptr<GameHUDComponent>> components;

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
		virtual bool FillHUDContent() override;
	};

}; // namespace death
