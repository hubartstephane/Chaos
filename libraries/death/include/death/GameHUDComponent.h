#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Renderable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>

#include <death/GameFramework.h>

namespace death
{

	// ====================================================================
	// GameHUDComponent
	// ====================================================================

	class GameHUDComponent : public chaos::Renderable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

		friend class GameHUD;

	public:

		/** getters on game */
		Game * GetGame();
		/** getters on game */
		Game const * GetGame() const;

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

	protected:

		/** returns the coordinate of the view corner corresponding to the given hotpoint */
		static glm::vec2 GetViewBoxCorner(chaos::box2 const & view_box, int hotpoint);

		/** MAYBE (!!!) called whenever the hud is beeing inserted into the hud (the HUD::RegisterComponent is template function. the function below is not necessaraly been called) */
		virtual void OnInsertedInHUD();
		/** called whenever the hud is beeing removed into the hud */
		virtual void OnRemovedFromHUD();

	protected:

		/** the current hud */
		GameHUD * hud = nullptr;
	};

	// ====================================================================
	// GameHUDSingleAllocationComponent
	// ====================================================================

	/** a HUD component with a single allocation */
	class GameHUDSingleAllocationComponent : public GameHUDComponent
	{
	protected:

		/** override */
		virtual void OnRemovedFromHUD() override;

	protected:

		/** allocations for the title */
		chaos::shared_ptr<chaos::ParticleAllocation> allocations;
	};

	// ====================================================================
	// GameHUDTitleComponent
	// ====================================================================

	class GameHUDTitleComponent : public GameHUDSingleAllocationComponent
	{
		friend class GameHUD;

	protected:

		virtual void OnInsertedInHUD(char const * game_name, bool normal, chaos::TagType layer_id); // this is not an override !
	};

	// ====================================================================
	// GameHUDBestScoreComponent
	// ====================================================================

	class GameHUDBestScoreComponent : public GameHUDSingleAllocationComponent
	{
		friend class GameHUD;

	protected:

		virtual void OnInsertedInHUD(int score); // this is not an override !
	};

	// ====================================================================
	// GameHUDInstructionComponent
	// ====================================================================

	class GameHUDInstructionComponent : public GameHUDSingleAllocationComponent
	{
		friend class GameHUD;

	protected:

		virtual void OnInsertedInHUD(char const * instructions); // this is not an override !
	};

	// ====================================================================
	// GameHUDCacheValueComponent
	// ====================================================================

	template<typename T>
	class GameHUDCacheValueComponent : public GameHUDSingleAllocationComponent
	{
		using type = T;

	protected:

		GameHUDCacheValueComponent(char const * in_format, type initial_value = type()) : 
			format(in_format), cached_value(initial_value){}

		/** override */
		virtual bool DoTick(double delta_time) override 
		{
			// update the cache value if necessary
			bool destroy_allocation = false;
			bool update_required = UpdateCachedValue(destroy_allocation);
			// destroy allocation
			if (destroy_allocation)
			{
				allocations = nullptr;
				return true;
			}
			// update ?
			if (!update_required)
				return true;
			// get box
			chaos::box2 view_box = GetGame()->GetViewBox();		
			// format text and create particles
			chaos::ParticleTextGenerator::GeneratorParams params;
			params.line_height = 30;
			params.font_info_name = "normal";					
			TweakTextGeneratorParams(params, view_box);
			// generate the allocation
			std::string str = chaos::StringTools::Printf(format.c_str(), cached_value);
			allocations = hud->GetGameParticleCreator().CreateTextParticles(str.c_str(), params, death::GameHUDKeys::TEXT_LAYER_ID);
			return true; 
		}

		/** get the position (and configuration) of the text on screen */
		virtual void TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box) {}

		/** update the cached value and returns true whether the particle system has to be regenerated */
		virtual bool UpdateCachedValue(bool & destroy_allocation) { return false; }

	protected:

		/** the cached value */
		type cached_value;
		/** the format */
		std::string format;
	};

	// ====================================================================
	// GameHUDScoreComponent
	// ====================================================================

	class GameHUDScoreComponent : public GameHUDCacheValueComponent<int>
	{
	public:

		GameHUDScoreComponent() : GameHUDCacheValueComponent<int>("Score : %d", -1){}

	protected:

		/** override */
		virtual bool UpdateCachedValue(bool & destroy_allocation) override;
		/** override */
		virtual void TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box) override;
	};

	// ====================================================================
	// GameHUDFramerateComponent
	// ====================================================================

	class GameHUDFramerateComponent : public  GameHUDCacheValueComponent<float>
	{
	public:

		GameHUDFramerateComponent() : GameHUDCacheValueComponent<float>("%02.01f FPS", -1.0f) {}

	protected:

		/** override */
		virtual int DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;
		/** override */
		virtual bool UpdateCachedValue(bool & destroy_allocation) override;
		/** override */
		virtual void TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box) override;

	protected:

		/** the framerate of the last rendering */
		mutable float framerate = 0.0f;
	};

}; // namespace death
