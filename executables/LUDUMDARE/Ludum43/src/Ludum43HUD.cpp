#include "Ludum43HUD.h"
#include "Ludum43Game.h"

bool LudumPlayingHUD::DoTick(double delta_time)
{
	// call super method
	GameHUD::DoTick(delta_time);

	LudumGame * ludum_game = dynamic_cast<LudumGame *>(game); // Game::PlaySound() in TickHeartWarning(..) requires a non const pointer
	if (ludum_game != nullptr)
	{
		UpdateWakenUpParticleCount(ludum_game);
		UpdateSavedParticleCount(ludum_game);
		UpdateLifeBar(ludum_game);	
	}
	return true;
}

void LudumPlayingHUD::UpdateWakenUpParticleCount(LudumGame const * ludum_game)
{
	int waken_up_particle_count = ludum_game->GetWakenUpParticleCount();
	if (waken_up_particle_count != cached_waken_up_particle_count)
	{
		RegisterParticles(death::GameHUDKeys::WAKENUP_PARTICLE_COUNT_ID, GetGameParticleCreator().CreateScoringText("Particles : %d", waken_up_particle_count, 20.0f, game->GetViewBox(), death::GameHUDKeys::TEXT_LAYER_ID));
		cached_waken_up_particle_count = waken_up_particle_count;
	}
}

void LudumPlayingHUD::UpdateSavedParticleCount(LudumGame const * ludum_game)
{
	int saved_particle_count = ludum_game->GetSavedParticleCount();
	if (saved_particle_count != cached_saved_particle_count)
	{
		if (saved_particle_count == 10)
			UnregisterParticles(death::GameHUDKeys::SAVED_PARTICLE_COUNT_ID);
		else
			RegisterParticles(death::GameHUDKeys::SAVED_PARTICLE_COUNT_ID, GetGameParticleCreator().CreateScoringText("Saved : %d", saved_particle_count, 70.0f, game->GetViewBox(), death::GameHUDKeys::TEXT_LAYER_ID));

		cached_saved_particle_count = saved_particle_count;
	}
}

void LudumPlayingHUD::UpdateLifeBar(LudumGame const * ludum_game)
{
	float life = ludum_game->GetPlayerLife();
	if (life != cached_life_value)
	{
		// create the allocation
		chaos::ParticleAllocation * allocation = FindParticleAllocation(death::GameHUDKeys::LIFE_ID);
		if (allocation == nullptr)
		{
			chaos::ParticleLayer * layer = particle_manager->FindLayer(death::GameHUDKeys::LIFE_LAYER_ID);
			if (layer == nullptr)
				return;

			allocation = layer->SpawnParticles(1);

			if (allocation == nullptr)
				return;
			RegisterParticles(death::GameHUDKeys::LIFE_ID, allocation);
		}
		else
		{
			allocation->Resize(1);
		}
		// fill the particle

		chaos::ParticleAccessor<ParticleLife> particles = allocation->GetParticleAccessor<ParticleLife>();
		if (particles.GetCount() == 0)
			return;

		glm::vec2 view_size = ludum_game->GetViewSize();


		glm::vec2 position1, position2;
		position1.x = -view_size.x * 0.5f + 40.0f;
		position1.y = -view_size.y * 0.5f + 40.0f;

		position2.x =  view_size.x * 0.5f - 40.0f;
		position2.y = -view_size.y * 0.5f + 70.0f;

		particles->bounding_box         = chaos::box2(std::make_pair(position1, position2));		
		particles->texcoords.bottomleft = glm::vec2(0.0f, 0.0f);
		particles->texcoords.topright   = glm::vec2(PLAYER_LIFETIME, 1.0f);
		particles->color                = glm::vec4(life, life, life, life);

		cached_life_value = life;
	}
}

bool LudumPlayingHUD::CreateHUDLayers()
{
	// call super method
	if (!death::GameHUD::CreateHUDLayers())
		return false;
	// create a layer for the life bar
	LudumGame * ludum_game = dynamic_cast<LudumGame *>(game);
	if (ludum_game != nullptr)
	{
		int render_order = -1;
		ParticleLifeTrait life_trait;
		life_trait.game = ludum_game;
		particle_manager->AddLayer<ParticleLifeTrait>(++render_order, death::GameHUDKeys::LIFE_LAYER_ID, "lifebar", life_trait);
	}
	return true;
}


#if 0


void LudumPlayingHUD::UpdateLifeParticles(LudumGame const * ludum_game)
{
	int current_life = ludum_game->GetCurrentLife();
	if (current_life != cached_life_value)
	{
		if (current_life < 0)
			UnregisterParticles(death::GameHUDKeys::LIFE_ID);
		else
		{
			chaos::ParticleAllocation * allocation = FindParticleAllocation(death::GameHUDKeys::LIFE_ID);
			if (allocation == nullptr)
			{
				allocation = GetGameParticleCreator().CreateParticles("life", current_life, death::GameHUDKeys::LIFE_LAYER_ID);
				if (allocation == nullptr)
					return;
				RegisterParticles(death::GameHUDKeys::LIFE_ID, allocation);
			}
			else
			{
				allocation->Resize(current_life);
			}

			// set the color
			chaos::ParticleAccessor<ParticleObject> particles = allocation->GetParticleAccessor<ParticleObject>();

			glm::vec2 view_size = ludum_game->GetViewSize();

			glm::vec2 particle_size;
			particle_size.x = 35.0f;
			particle_size.y = 20.0f;

			for (size_t i = 0; i < (size_t)current_life; ++i)
			{
				glm::vec2 position;
				position.x = -view_size.x * 0.5f + 20.0f + (particle_size.x + 5.0f) * (float)i;
				position.y = -view_size.y * 0.5f + 15.0f;

				particles[i].bounding_box.position = chaos::Hotpoint::Convert(position, particle_size, chaos::Hotpoint::BOTTOM_LEFT, chaos::Hotpoint::CENTER);
				particles[i].bounding_box.half_size = 0.5f * particle_size;

				particles[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		cached_life_value = current_life;
	}
}




#endif

