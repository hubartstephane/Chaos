#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPURenderable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>

#include <death/GameFramework.h>
#include <death/GameHUDKeys.h>

namespace death
{

	// ====================================================================
	// GameHUDComponent
	// ====================================================================

	class GameHUDComponent : public chaos::GPURenderable
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
		static glm::vec2 GetCanvasBoxCorner(chaos::box2 const & canvas_box, int hotpoint);

		/** MAYBE (!!!) called whenever the hud is beeing inserted into the hud (the HUD::RegisterComponent is template function. the function below is not necessaraly been called) */
		virtual void OnInsertedInHUD();
		/** called whenever the hud is beeing removed into the hud */
		virtual void OnRemovedFromHUD();
		/** initialization method from JSON */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);

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

		/** show or hide the component */
		virtual void ShowComponent(bool in_show);
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

		/** constructor */
		GameHUDTextComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
		/** constructor */
		GameHUDTextComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);

		/** called whenever it is inserted in HUD */
		virtual void OnInsertedInHUD(char const * in_text = nullptr); // this is not an override !

	protected:

		/** tweak the text generation parameters */
		virtual void TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & final_params) const;
		/** create the text */
		virtual void UpdateTextAllocation(char const * in_text);
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override;

	protected:

		/** the layer owning the allocation */
		chaos::TagType layer_id = 0;
		/** the placement and aspect of the text */
		chaos::ParticleTextGenerator::GeneratorParams generator_params;
	};

	// ====================================================================
	// GameHUDCacheValueComponent
	// ====================================================================

	template<typename T>
	class GameHUDCacheValueComponent : public GameHUDTextComponent
	{
		using type = T;

	protected:

		/** constructor */
		GameHUDCacheValueComponent(char const * in_format, type in_initial_value, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
			GameHUDTextComponent(in_layer_id),
			cached_value(in_initial_value),
			format(in_format) {}
		/** constructor */
		GameHUDCacheValueComponent(char const * in_format, type in_initial_value, chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
			GameHUDTextComponent(in_params, in_layer_id) ,
			cached_value(in_initial_value),
			format(in_format){}

		/** override */
		virtual bool DoTick(double delta_time) override
		{
			UpdateTextAllocation(nullptr);
			return true;
		}

		/** format the text according to cached value */
		virtual std::string FormatText() const
		{
			return chaos::StringTools::Printf(format.c_str(), cached_value);
		}

		/** override */
		virtual void UpdateTextAllocation(char const * in_text) override
		{
			// search value if necessary
			if (in_text == nullptr)
			{
				// update the cache value if necessary
				bool destroy_allocation = false;
				bool update_required = UpdateCachedValue(destroy_allocation);
				// destroy allocation
				if (destroy_allocation)
					GameHUDTextComponent::UpdateTextAllocation(nullptr);
				else if (update_required)
					GameHUDTextComponent::UpdateTextAllocation(FormatText().c_str());
			}
			else
				GameHUDTextComponent::UpdateTextAllocation(in_text);

		}

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

		/** constructor */
		GameHUDScoreComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
		/** constructor */
		GameHUDScoreComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
			GameHUDCacheValueComponent<int>("Score: %d", -1, in_params, in_layer_id) {}

	protected:

		/** override */
		virtual bool UpdateCachedValue(bool & destroy_allocation) override;
	};

	// ====================================================================
	// GameHUDFramerateComponent
	// ====================================================================

	class GameHUDFramerateComponent : public  GameHUDCacheValueComponent<float>
	{
	public:

		/** constructor */
		GameHUDFramerateComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
		/** constructor */
		GameHUDFramerateComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
			GameHUDCacheValueComponent<float>("%02.01f FPS", -1.0f, in_params, in_layer_id) {}

	protected:

		/** override */
		virtual int DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params) const override;
		/** override */
		virtual bool UpdateCachedValue(bool & destroy_allocation) override;

	protected:

		/** the framerate of the last rendering */
		mutable float framerate = 0.0f;
	};

	// ====================================================================
	// GameHUDTimeoutComponent
	// ====================================================================

	class GameHUDTimeoutComponent : public GameHUDCacheValueComponent<float>
	{
	public:

		/** constructor */
		GameHUDTimeoutComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
		/** constructor */
		GameHUDTimeoutComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
			GameHUDCacheValueComponent<float>("%02.01f", -1.0f, in_params, in_layer_id) {}

	protected:

		/** override */
		virtual bool UpdateCachedValue(bool & destroy_allocation) override;
		/** override */
		virtual void TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & final_params) const override;
	};


	// ====================================================================
	// GameHUDLifeComponent
	// ====================================================================

	class GameHUDLifeComponent : public GameHUDSingleAllocationComponent
	{
	public:

		/** constructor */
		GameHUDLifeComponent(chaos::TagType in_layer_id = death::GameHUDKeys::LIFE_LAYER_ID);

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;
		/** update all particles (count, alpha) */
		void UpdateLifeParticles(double delta_time);
		/** tick heart */
		void TickHeartBeat(double delta_time);
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override;

	protected:

		/** the layer owning the allocation */
		chaos::TagType layer_id = 0;
		/** caching the current life count */
		int cached_value = -1;

		/** the hotpoint of the first particle */
		int hotpoint_type = chaos::Hotpoint::BOTTOM_LEFT;
		/** the position of the very first particle */
		glm::vec2 position = glm::vec2(0.0f, 0.0f);
		/** the particle size of the particle (special meaning, see .cpp) */
		glm::vec2 particle_size = glm::vec2(0.0f, 0.0f);
		/** the offset to apply between each particles */
		glm::vec2 particle_offset = glm::vec2(20.0f, 0.0f);

		/** the name of the particle to render */
		std::string particle_name = "life";
		/** the sound to play */
		std::string heart_beat_sound = "heartbeat";
		/** the heart beat frequency */
		float heart_beat_frequency = 1.0f;

		/** the minimum alpha apply to particle when it is the last life */
		float fadeout_warning_base = 0.4f;

		/** the current heart warning timer value */
		float warning_value = 0.0f;
	};
	

	// ====================================================================
	// GameHUDLevelTitleComponent
	// ====================================================================

	class GameHUDLevelTitleComponent : public GameHUDCacheValueComponent<std::string>
	{
		friend class GameHUD;

	public:

		/** constructor */
		GameHUDLevelTitleComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
		/** constructor */
		GameHUDLevelTitleComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
			GameHUDCacheValueComponent<std::string>("%s", std::string(), in_params, in_layer_id) {}

	protected:

		/** override */
		virtual std::string FormatText() const override;
		/** override */
		virtual bool UpdateCachedValue(bool & destroy_allocation) override;
	};	

	// ====================================================================
	// GameHUDFreeCameraComponent
	// ====================================================================

	class GameHUDFreeCameraComponent : public GameHUDTextComponent
	{

	public:

		/** constructor */
		GameHUDFreeCameraComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
		/** constructor */
		GameHUDFreeCameraComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;
	};

}; // namespace death
