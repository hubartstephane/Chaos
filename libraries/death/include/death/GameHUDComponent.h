#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Renderable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>

#include <death/GameFramework.h>
#include <death/GameHUDKeys.h>

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
		chaos::shared_ptr<chaos::ParticleAllocationBase> allocations;
	};


	// ====================================================================
	// GameHUDTextComponent
	// ====================================================================

	class GameHUDTextComponent : public GameHUDSingleAllocationComponent
	{

	public:

		GameHUDTextComponent(char const * in_text, chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);

		GameHUDTextComponent(char const * in_text, char const * font_name, float line_height, glm::vec2 const & position, int hotpoint_type, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);

		virtual void OnInsertedInHUD(); // this is not an override !

	protected:

		/** the text to display */
		std::string text;
		/** the layer owning the allocation */
		chaos::TagType layer_id = 0;
		/** the placement and aspect of the text */
		chaos::ParticleTextGenerator::GeneratorParams params;
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
			allocations = hud->GetGameParticleCreator().CreateTextParticles(str.c_str(), params, GameHUDKeys::TEXT_LAYER_ID);
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

		GameHUDScoreComponent() : GameHUDCacheValueComponent<int>("Score:%d", -1){}

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

	// ====================================================================
	// GameHUDLifeComponent
	// ====================================================================

	class GameHUDLifeComponent : public GameHUDSingleAllocationComponent
	{
	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;
		/** update all particles (count, alpha) */
		void UpdateLifeParticles(double delta_time);
		/** tick heart */
		void TickHeartBeat(double delta_time);

	protected:

		/** caching the current life count */
		int cached_value = -1;
		/** the current heart warning timer value */
		float heart_warning = 0.0f;
		/** the heart warning period */
		float heart_beat_speed = 1.0f;
	};

	// ====================================================================
	// GameHUDTimeoutComponent
	// ====================================================================

	class GameHUDTimeoutComponent : public GameHUDCacheValueComponent<float>
	{
	public:

		GameHUDTimeoutComponent() : GameHUDCacheValueComponent<float>("%02.01f", -1.0f) {}

	protected:

		/** override */
		virtual bool UpdateCachedValue(bool & destroy_allocation) override;
		/** override */
		virtual void TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box) override;
	};


}; // namespace death
