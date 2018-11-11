#include <death/GameHUD.h>
#include <death/Game.h>

namespace death
{
	bool GameHUD::Initialize(chaos::ParticleManager * in_particle_manager, bool in_external_manager)
	{
		// ensure not already initialized or in use
		assert(particle_manager == nullptr);
		assert(in_particle_manager != nullptr);
		assert(particle_allocations.size() == 0);

		// copy the data
		particle_manager = in_particle_manager;
		external_manager = in_external_manager;
		return true;
	}

	bool GameHUD::Initialize(chaos::BitmapAtlas::TextureArrayAtlas * in_texture_atlas)
	{
		// ensure not already initialized or in use
		assert(particle_manager == nullptr);
		assert(in_texture_atlas != nullptr);
		assert(particle_allocations.size() == 0);

		// create a texture manager
		particle_manager = new chaos::ParticleManager;
		if (particle_manager == nullptr)
			return false;
		particle_manager->SetTextureAtlas(in_texture_atlas);

		return true;
	}

	void GameHUD::RegisterParticles(chaos::TagType key, chaos::ParticleAllocation * allocation, bool remove_previous)
	{
		if (remove_previous)
			UnregisterParticles(key);
		particle_allocations.insert(std::make_pair(key, allocation));
	}
	
	void GameHUD::UnregisterParticles(chaos::TagType key)
	{
		particle_allocations.erase(key);
	}
		
	void GameHUD::Clear()
	{
		particle_allocations.clear();
	}

	bool GameHUD::DoTick(double delta_time)
	{
		if (!external_manager && particle_manager != nullptr)
			particle_manager->Tick(delta_time);
		return true;
	}

	int GameHUD::DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
	{
		int result = 0; 
		if (!external_manager && particle_manager != nullptr)
			result += particle_manager->Display(uniform_provider, render_params);
		return result;
	}

	bool GameHUD::IsDynamicParticlesRequiringUpdate(chaos::TagType key, int new_value)
	{


		return true;
	}










	chaos::ParticleAllocation * GameHUD::UpdateDynamicParticles(chaos::TagType key, int new_value)
	{


		return nullptr;
	}

	bool GameHUD::CreateDynamicParticles(chaos::TagType, int initial_value)
	{



		return true;
	}

#if 0
	void GameHUD::CreateDynamicText(TagType key, char const * format, int initial_value)
#endif






	void PlayingHUD::SetScoreValue(class Game * game, int new_score)
	{
		CacheAndCreateScoreAllocation(game, new_score, "Score : %d", 20.0f, cached_score_value, GameHUDKeys::BEST_SCORE_ID);
	}

	void PlayingHUD::CacheAndCreateScoreAllocation(class Game * game, int value, char const * format, float Y,int & cached_value, chaos::TagType key)
	{
		if (value < 0)
			UnregisterParticles(key);
		else if (cached_value != value)
			RegisterParticles(key, game->CreateScoringText(format, value, Y, particle_manager.get()));
		cached_value = value;
	}

}; // namespace death

