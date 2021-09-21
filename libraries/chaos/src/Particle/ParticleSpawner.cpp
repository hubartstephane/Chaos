#include <chaos/Chaos.h>

namespace chaos
{
	/** constructor */
	ParticleSpawner::ParticleSpawner(BitmapAtlas::TextureArrayAtlas const* in_atlas, ParticleLayerBase* in_particle_layer, ObjectRequest bitmap_request, ObjectRequest folder_request) :
		atlas(in_atlas),
		particle_layer(in_particle_layer)
	{
		// in case of error, make the Spawner invalid
		if (!SetBitmapInfo(bitmap_request, folder_request))
			particle_layer = nullptr;
	}

	BitmapAtlas::BitmapInfo const* ParticleSpawner::FindBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request) const
	{
		// early exit
		if (atlas == nullptr)
			return nullptr;
		// if the requested bitmap is nullptr, considere the call as successfull
		if (bitmap_request.IsNoneRequest())
			return nullptr;
		// find the folder
		BitmapAtlas::FolderInfo const* folder_info = atlas->GetFolderInfo(folder_request, true);
		if (folder_info == nullptr)
			return nullptr;
		// get the bitmap
		return folder_info->GetBitmapInfo(bitmap_request, true);
	}


	bool ParticleSpawner::SetBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request)
	{
		// early exit
		if (particle_layer == nullptr || atlas == nullptr)
			return false;
		// if the requested bitmap is nullptr, considere the call as successfull
		if (bitmap_request.IsNoneRequest())
		{
			bitmap_info = nullptr;
			return true;
		}
		// find the folder
		BitmapAtlas::FolderInfo const* folder_info = atlas->GetFolderInfo(folder_request, true);
		if (folder_info == nullptr)
			return false;
		// get the bitmap
		bitmap_info = folder_info->GetBitmapInfo(bitmap_request, true);
		if (bitmap_info == nullptr)
			return false;
		return true;
	}

	SpawnParticleResult ParticleSpawner::SpawnParticles(size_t count, bool new_allocation)
	{
		// early exit
		if (particle_layer == nullptr)
			return nullptr;

		return particle_layer->SpawnParticles(count, new_allocation).Process([this](ParticleAccessor<ParticleDefault> accessor)
		{
			if (bitmap_info != nullptr && accessor.IsValid())
			{
				// find the corresponding Bitmap
				ParticleTexcoords texcoords;

				if (bitmap_info->HasAnimation() && bitmap_info->GetAnimationImageCount() > 0)
					texcoords = bitmap_info->GetAnimationLayout(0, WrapMode::CLAMP).GetTexcoords();
				else
					texcoords = bitmap_info->GetTexcoords();

				// apply the texcoords to all particles
				for (ParticleDefault& particle : accessor)
					particle.texcoords = texcoords;
			}
		});
	}


	SpawnParticleResult ParticleSpawner::SpawnText(char const* in_text, bool new_allocation, ParticleTextGenerator::GeneratorParams const& params, ParticleTextGenerator::CreateTextAllocationParams const& allocation_params)
	{
		// early exit
		if (particle_layer == nullptr)
			return nullptr;
		// get the application
		WindowApplication const* window_application = Application::GetInstance();
		if (window_application == nullptr)
			return {};
		// get the generator
		ParticleTextGenerator::Generator const* generator = window_application->GetTextGenerator();
		if (generator == nullptr)
			return {};
		// generate the data
		ParticleTextGenerator::GeneratorResult generator_result;
		if (!generator->Generate(in_text, generator_result, params))
			return {};

		// shu48



		//return SpawnParticles()



		return nullptr;
	}

#if 0
	QuadPrimitive<vertex_type> AddText(char const* in_text, ParticleTextGenerator::GeneratorParams const& params = {}, ParticleTextGenerator::CreateTextAllocationParams const& allocation_params = {})
	{
		// get the application
		WindowApplication const* window_application = Application::GetInstance();
		if (window_application == nullptr)
			return {};
		// get the generator
		ParticleTextGenerator::Generator const* generator = window_application->GetTextGenerator();
		if (generator == nullptr)
			return {};
		// generate the data
		ParticleTextGenerator::GeneratorResult generator_result;
		if (!generator->Generate(in_text, generator_result, params))
			return {};
		// create the primitives
		return TextToPrimitives(*this, generator_result, allocation_params);
	}
#endif





}; // namespace chaos

