#include <death/GameParticleTools.h>

#include <chaos/ParticleDefault.h>

namespace death
{



	chaos::ParticleAllocation * GameParticleTools::SpawnObjects(chaos::TagType layer_id, size_t count) const
	{
		// spawn the particles
		chaos::ParticleLayer * layer = particle_manager->FindLayer(layer_id);
		if (layer == nullptr)
			return nullptr;
		return layer->SpawnParticles(count);
	}

	chaos::ParticleAllocation * GameParticleTools::CreateGameObjects(char const * bitmap_name, size_t count, chaos::TagType layer_id) const
	{
		// allocate the objects
		chaos::ParticleAllocation * allocation = SpawnObjects(layer_id, count);
		if (allocation == nullptr)
			return nullptr;

		if (!InitializeGameObjects(allocation, bitmap_name, count))
		{
			delete(allocation);
			return nullptr;
		}
		return allocation;
	}

	bool GameParticleTools::InitializeGameObjects(chaos::ParticleAllocation * allocation, char const * bitmap_name, size_t last_count) const
	{
		// find bitmap set
		chaos::BitmapAtlas::FolderInfo const * bitmap_set = texture_atlas->GetFolderInfo("sprites");
		if (bitmap_set == nullptr)
			return false;

		// find bitmap info
		chaos::BitmapAtlas::BitmapInfo const * info = bitmap_set->GetBitmapInfo(bitmap_name);
		if (info == nullptr)
			return false;

		chaos::ParticleAccessor<chaos::ParticleDefault::Particle> particles = allocation->GetParticleAccessor<chaos::ParticleDefault::Particle>();

		size_t particles_count = particles.GetCount();
		size_t start = 0;
		size_t count = last_count;

		if (particles_count < count)
			count = particles_count; 
		else
			start = particles_count - count;

		for (size_t i = 0 ; i < count ; ++i)
		{
			chaos::ParticleDefault::Particle & particle = particles[start + i];
			particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*info, texture_atlas->GetAtlasDimension());
		}

		return true;
	}

	chaos::ParticleAllocation * GameParticleTools::CreateTextParticles(char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params, chaos::TagType layer_id) const
	{
		// find layer of concern
		chaos::ParticleLayer * layer = particle_manager->FindLayer(layer_id);
		if (layer == nullptr)
			return nullptr;

		// generate the tokens
		chaos::ParticleTextGenerator::GeneratorResult result;

		particle_text_generator->Generate(text, result, params);

		chaos::ParticleAllocation * allocation = chaos::ParticleTextGenerator::CreateTextAllocation(layer, result);

		return allocation;
	}

	chaos::ParticleAllocation * GameParticleTools::CreateTitle(char const * title, bool normal, chaos::TagType layer_id) const
	{
		chaos::ParticleTextGenerator::GeneratorParams params;
		params.line_height = 0; // title_size; // shuxxx
		params.hotpoint_type = chaos::Hotpoint::CENTER;
		params.position.y = 0.0f;// title_placement_y; // shuxxx

		params.font_info_name = (normal) ? "normal" : "title";

		return CreateTextParticles(title, params, layer_id);
	}

	chaos::ParticleAllocation * GameParticleTools::CreateScoringText(char const * format, int value, float Y, chaos::TagType layer_id) const
	{
		// get view size
		chaos::box2 view ; // shuxxx = GetViewBox();

		std::pair<glm::vec2, glm::vec2> corners = view.GetCorners();

		// set the values
		chaos::ParticleTextGenerator::GeneratorParams params;
		params.line_height = 30;
		params.hotpoint_type = chaos::Hotpoint::TOP_LEFT;
		params.position.x = corners.first.x + 20.0f;
		params.position.y = corners.second.y - Y;
		params.font_info_name = "normal";

		// format text and create particles
		std::string str = chaos::StringTools::Printf(format, value);
		return CreateTextParticles(str.c_str(), params, layer_id);
	}
	
}; // namespace death
