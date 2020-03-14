#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPURenderable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>
#include <chaos/Hotpoint.h>

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
		friend class GameHUDTimedComponent;

	public:

		/** getters on game */
		chaos::AutoCastable<Game> GetGame();
		/** getters on game */
		chaos::AutoConstCastable<Game> GetGame() const;

		/** getters on game instance */
		chaos::AutoCastable<GameInstance> GetGameInstance();
		/** getters on game instance */
		chaos::AutoConstCastable<GameInstance> GetGameInstance() const;

		/** get currently played level */
		chaos::AutoCastable<Level> GetLevel();
		/** get currently played level */
		chaos::AutoConstCastable<Level> GetLevel() const;

		/** get currently played level */
		chaos::AutoCastable<LevelInstance> GetLevelInstance();
		/** get currently played level */
		chaos::AutoConstCastable<LevelInstance> GetLevelInstance() const;

		/** get the player */
		chaos::AutoCastable<Player> GetPlayer(size_t player_index);
		/** get the player */
		chaos::AutoConstCastable<Player> GetPlayer(size_t player_index) const;
		/** get the number of player */
		size_t GetPlayerCount() const;

	protected:

		/** returns the coordinate of the view corner corresponding to the given hotpoint */
		static glm::vec2 GetCanvasBoxCorner(chaos::box2 const & canvas_box, chaos::Hotpoint hotpoint);
		/** MAYBE (!!!) called whenever the hud is beeing inserted into the hud (the HUD::RegisterComponent is template function. the function below is not necessaraly been called) */
		virtual void OnInsertedInHUD();
		/** called whenever the hud is beeing removed into the hud */
		virtual void OnRemovedFromHUD();
		/** initialization method from JSON */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);
		/** set the HUD */
		virtual void SetHUD(GameHUD * in_hud);

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
		friend class GameHUD;

	public:

		/** constructor */
		GameHUDTextComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
		/** constructor */
		GameHUDTextComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);

	protected:

		/** tweak the text generation parameters */
		virtual void TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & final_params) const;
		/** create the text */
		virtual void UpdateTextAllocation(char const * in_text);
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override;
		/** called whenever it is inserted in HUD */
		virtual void OnInsertedInHUD(char const * in_text = nullptr); // this is not an override !

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
		virtual bool DoTick(float delta_time) override
		{
			UpdateTextAllocation(nullptr);
			return true;
		}

		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override
		{
			if (!GameHUDTextComponent::InitializeFromConfiguration(json, config_path))
				return false;
		
			chaos::JSONTools::GetAttribute(json, "format", format);		
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
				bool update_required = (update_cache_value_func)?
					update_cache_value_func(cached_value, destroy_allocation):									
					UpdateCachedValue(destroy_allocation);
				// destroy allocation
				if (destroy_allocation)
					GameHUDTextComponent::UpdateTextAllocation(nullptr);
				else if (update_required)
				{
					if (format_func)
						GameHUDTextComponent::UpdateTextAllocation(format_func(cached_value).c_str());
					else
						GameHUDTextComponent::UpdateTextAllocation(FormatText().c_str());
				}
					
			}
			else
				GameHUDTextComponent::UpdateTextAllocation(in_text);

		}

		/** update the cached value and returns true whether the particle system has to be regenerated */
		virtual bool UpdateCachedValue(bool & destroy_allocation) { return false; }

	public:

		/** an alternate 'UpdateCachedValue' member */
		std::function<bool(T & cached_value, bool & destroy_allocation)> update_cache_value_func;
		/** an alternate 'FormatText' member */
		std::function<std::string(T const & cached_value)> format_func;

	protected:

		/** the cached value */
		type cached_value;
		/** the format */
		std::string format;
	};

	// ====================================================================
	// GameHUDNotificationComponent
	// ====================================================================

	class GameHUDNotificationComponent : public GameHUDTextComponent
	{
		friend class GameHUD;

	public:

		/** constructor */
		GameHUDNotificationComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
		/** constructor */
		GameHUDNotificationComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);

		/** show/initialize the notification */
		void ShowNotification(char const * in_message, float in_lifetime);
		/** hide the notification */
		void HideNotification();

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the life of the component */
		float lifetime = -1.0f;
		/** the current time */
		float current_time = 0.0f;
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
		virtual int DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params) override;
		/** override */
		virtual bool UpdateCachedValue(bool & destroy_allocation) override;

	protected:

		/** the framerate of the last rendering */
		float framerate = 0.0f;
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
		virtual bool DoTick(float delta_time) override;
		/** update all particles (count, alpha) */
		void UpdateLifeParticles(float delta_time);
		/** returns the number of life */
		virtual int GetLifeCount() const;
		/** tick heart */
		void TickHeartBeat(float delta_time);
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override;


	public:

		/** an alternate way to have life count */
		std::function<int()> get_life_count_func;

	protected:

		/** the layer owning the allocation */
		chaos::TagType layer_id = 0;
		/** caching the current life count */
		int cached_value = -1;

		/** the hotpoint of the first particle */
		chaos::Hotpoint hotpoint = chaos::Hotpoint::BOTTOM_LEFT;
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
		virtual bool DoTick(float delta_time) override;
	};

	// ====================================================================
	// GameHUDTimedComponent
	// ====================================================================

#if 0

	// SHU fixme : Tick -> RemoveFromHUD -> iterator KO

	class GameHUDTimedComponent : public GameHUDComponent
	{
		friend class GameHUD;

	public:

		/** constructor */
		GameHUDTimedComponent(GameHUDComponent * in_child_component, float in_lifetime);

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual void OnRemovedFromHUD() override;
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override;
		/** override */
		virtual void SetHUD(GameHUD * in_hud) override;

	public:

		/** the life of the component */
		float lifetime = -1.0f;
		/** the current time */
		float current_time = 0.0f;
		/** the inner component */
		chaos::shared_ptr<GameHUDComponent> child_component;
	};

#endif

}; // namespace death
