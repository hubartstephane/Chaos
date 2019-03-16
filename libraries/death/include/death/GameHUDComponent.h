#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Renderable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>

#include <death/GameFramework.h>

namespace death
{
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

		/** MAYBE (!!!) called whenever the hud is beeing inserted into the hud (the HUD::RegisterComponent is template function. the function below is not necessaraly been called) */
		virtual void OnInsertedInHUD();
		/** called whenever the hud is beeing removed into the hud */
		virtual void OnRemovedFromHUD();

	protected:

		/** the current hud */
		GameHUD * hud = nullptr;
	};





	class GameHUDSingleAllocationComponent : public GameHUDComponent
	{
	protected:

		/** override */
		virtual void OnRemovedFromHUD() override;

	protected:

		/** allocations for the title */
		chaos::shared_ptr<chaos::ParticleAllocation> allocations;
	};



	class GameHUDTitleComponent : public GameHUDSingleAllocationComponent
	{
		friend class GameHUD;

	protected:

		virtual void OnInsertedInHUD(char const * game_name, bool normal, chaos::TagType layer_id); // this is not an override !
	};

	class GameHUDBestScoreComponent : public GameHUDSingleAllocationComponent
	{
		friend class GameHUD;

	protected:

		virtual void OnInsertedInHUD(int score); // this is not an override !
	};

	class GameHUDInstructionComponent : public GameHUDSingleAllocationComponent
	{
		friend class GameHUD;

	protected:

		virtual void OnInsertedInHUD(char const * instructions); // this is not an override !
	};








	template<typename T, T initial_value>
	class GameHUDCacheValueComponent : public GameHUDSingleAllocationComponent
	{
		using type = T;

	protected:

		GameHUDCacheValueComponent(char const * in_format) : format(in_format){}

		/** override */
		virtual bool DoTick(double delta_time) override 
		{
			// update the cache value if necessary
			if (!UpdateCachedValue())
				return true;			
			// get box
			chaos::box2 view_box = GetGame()->GetViewBox();
			std::pair<glm::vec2, glm::vec2> corners = view_box.GetCorners();
			// format text and create particles
			chaos::ParticleTextGenerator::GeneratorParams params;
			params.line_height = 60;
			params.font_info_name = "normal";					
			TweakTextGeneratorParams(params, view_box);
			// generate the allocation
			std::string str = chaos::StringTools::Printf(format.c_str(), cached_value);
			allocations = hud->GetGameParticleCreator().CreateTextParticles(str.c_str(), params, death::GameHUDKeys::TEXT_LAYER_ID);
			return true; 
		}

		/** get the position (and configuration) of the text on screen */
		virtual void GetTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box)
		{			
			params.hotpoint_type = chaos::Hotpoint::RIGHT | chaos::Hotpoint::TOP;
			params.position.x = corners.second.x - 20.0f;
			params.position.y = corners.second.y - 20.0f;
		}

		/** update the cached value and returns true whether the particle system has to be regenerated */
		virtual bool UpdateCachedValue() { false; }

	protected:

		/** the cached value */
		type cached_value = initial_value;
		/** the format */
		std::string format;
	};






	class GameHUDScoreComponent : public GameHUDSingleAllocationComponent
	{
		friend class GameHUD;

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

	protected:

		/** caching the score */
		int cached_value = -1;
	};

	class GameHUDFramerateComponent : public GameHUDSingleAllocationComponent
	{
		friend class GameHUD;

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;
		/** override */
		virtual int DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;

	protected:

		/** the framerate registered at previous frame*/
		mutable float framerate = -1.0f;
		/** caching the framerate */
		float cached_value = -1.0f;
	};



}; // namespace death
