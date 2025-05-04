#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================================
	// GPUAtlasGenerator implementation
	// ========================================================================

	GPUAtlas* GPUAtlasGenerator::GenerateAtlas(AtlasInput const& in_input, AtlasGeneratorParams const& in_params) const
	{
		// generate a standard atlas to be converted
		Atlas          atlas;
		AtlasGenerator generator;
		if (!generator.ComputeResult(in_input, atlas, in_params))
			return nullptr;
		return GenerateAtlas(std::move(atlas));
	}

	GPUAtlas* GPUAtlasGenerator::GenerateAtlas(Atlas const & in_atlas) const
	{
		// generate texture Atlas
		GPUAtlas* result = new GPUAtlas;
		if (result == nullptr)
			return nullptr;
		if (!FillAtlasContent(result, in_atlas))
		{
			delete(result);
			return nullptr;
		}
		return result;
	}

	GPUAtlas* GPUAtlasGenerator::GenerateAtlas(Atlas&& in_atlas) const
	{
		// generate texture Atlas
		GPUAtlas* result = new GPUAtlas;
		if (result == nullptr)
			return nullptr;
		if (!FillAtlasContent(result, std::move(in_atlas)))
		{
			delete(result);
			return nullptr;
		}
		return result;
	}

	bool GPUAtlasGenerator::FillAtlasContent(GPUAtlas* result, Atlas const & in_atlas) const
	{
		// empty previous content
		result->Clear();
		// create the texture
		if (in_atlas.GetBitmaps().size() > 0)
		{
			GPUTexture* texture = CreateTextureFromAtlas(in_atlas);
			if (texture == nullptr)
				return false;
			result->texture = texture;
		}
		// copy data
		result->atlas_count = in_atlas.atlas_count;
		result->dimension = in_atlas.dimension;
		return CopyAtlasFolders(&result->root_folder, &in_atlas.root_folder);
	}

	bool GPUAtlasGenerator::FillAtlasContent(GPUAtlas* result, Atlas&& in_atlas) const
	{
		// empty previous content
		result->Clear();
		// create the texture (if at least one bitmap)
		if (in_atlas.GetBitmaps().size() > 0)
		{
			GPUTexture* texture = CreateTextureFromAtlas(in_atlas);
			if (texture == nullptr)
				return false;
			result->texture = texture;
		}
		// steal all data
		result->atlas_count = in_atlas.atlas_count;
		result->dimension = in_atlas.dimension;
		result->root_folder = std::move(in_atlas.root_folder);
		return true;
	}

	GPUTexture * GPUAtlasGenerator::CreateTextureFromAtlas(Atlas const & in_atlas) const
	{
		// create and fill a texture array generator
		GPUTextureArrayGenerator generator(GetGPUDevice());

		for (bitmap_ptr const & bitmap : in_atlas.GetBitmaps())
			generator.AddGenerator(new GPUTextureArraySliceGenerator_Image(bitmap.get(), false)); // do not release image, we have a unique_ptr on it

		// generate the texture array
		return generator.GenTextureObject();
	}

	bool GPUAtlasGenerator::CopyAtlasFolders(AtlasFolderInfo* dst_folder_info, AtlasFolderInfo const* src_folder_info) const
	{
		assert(dst_folder_info != nullptr);
		assert(src_folder_info != nullptr);

		// copy name
		dst_folder_info->SetName(src_folder_info->GetName());
		dst_folder_info->SetTag(src_folder_info->GetTag());
		// copy bitmaps and characters
		dst_folder_info->bitmaps = src_folder_info->bitmaps;
		dst_folder_info->fonts = src_folder_info->fonts;
		// recursively copy data
		size_t child_folder_count = src_folder_info->folders.size();
		for (size_t i = 0; i < child_folder_count; ++i)
		{
			AtlasFolderInfo const* src_child_folder = src_folder_info->folders[i].get();
			if (src_child_folder == nullptr)
				continue;

			AtlasFolderInfo* dst_child_folder = new AtlasFolderInfo;
			if (dst_child_folder == nullptr)
				continue;

			dst_folder_info->folders.push_back(std::move(std::unique_ptr<AtlasFolderInfo>(dst_child_folder)));
			CopyAtlasFolders(dst_child_folder, src_child_folder);
		}
		return true;
	}

}; // namespace chaos

