#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void GPUAtlas::Clear()
	{
		BitmapAtlas::AtlasBase::Clear();
		texture = nullptr;
	}

	bool GPUAtlas::LoadAtlas(FilePathParam const& path)
	{
		Clear();

		// use a temporary standard atlas to load everything necessary from file
		BitmapAtlas::Atlas atlas;
		if (!atlas.LoadAtlas(path))
			return false;
		return LoadFromBitmapAtlas(std::move(atlas));
	}

	bool GPUAtlas::LoadFromBitmapAtlas(BitmapAtlas::Atlas const& atlas)
	{
		Clear();
		if (!DoLoadFromBitmapAtlas(atlas))
		{
			Clear();
			return false;
		}
		return true;
	}

	bool GPUAtlas::LoadFromBitmapAtlas(BitmapAtlas::Atlas&& atlas)
	{
		Clear();
		if (!DoLoadFromBitmapAtlas(std::move(atlas)))
		{
			Clear();
			return false;
		}
		return true;
	}

	bool GPUAtlas::DoGenerateTextureArray(BitmapAtlas::Atlas const& atlas)
	{
		// create and fill a texture array generator
		std::vector<bitmap_ptr> const& bitmaps = atlas.GetBitmaps();

		GPUTextureArrayGenerator generator;
		for (size_t i = 0; i < bitmaps.size(); ++i)
			generator.AddGenerator(new GPUTextureArraySliceGenerator_Image(bitmaps[i].get(), false)); // do not release image, we have a unique_ptr on it

		// generate the texture array
		texture = generator.GenTextureObject();
		if (texture == nullptr)
			return false;
		return true;
	}

	bool GPUAtlas::DoLoadFromBitmapAtlas(BitmapAtlas::Atlas&& atlas)
	{
		if (!DoGenerateTextureArray(atlas))
			return false;

		// steal all data
		atlas_count = atlas.atlas_count;
		dimension = atlas.dimension;
		root_folder = std::move(atlas.root_folder);
		return true;
	}

	bool GPUAtlas::DoLoadFromBitmapAtlas(BitmapAtlas::Atlas const& atlas)
	{
		if (!DoGenerateTextureArray(atlas))
			return false;

		// copy all data
		atlas_count = atlas.atlas_count;
		dimension = atlas.dimension;

		return DoCopyFolder(&root_folder, &atlas.root_folder);
	}

	bool GPUAtlas::DoCopyFolder(BitmapAtlas::AtlasFolderInfo* dst_folder_info, BitmapAtlas::AtlasFolderInfo const* src_folder_info)
	{
		assert(dst_folder_info != nullptr);
		assert(src_folder_info != nullptr);

		// copy name
		dst_folder_info->SetName(dst_folder_info->GetName());
		dst_folder_info->SetTag(src_folder_info->GetTag());
		// copy bitmaps and characters
		dst_folder_info->bitmaps = src_folder_info->bitmaps;
		dst_folder_info->fonts = src_folder_info->fonts;
		// recursively copy data
		size_t child_folder_count = src_folder_info->folders.size();
		for (size_t i = 0; i < child_folder_count; ++i)
		{
			BitmapAtlas::AtlasFolderInfo const* src_child_folder = src_folder_info->folders[i].get();
			if (src_child_folder == nullptr)
				continue;

			BitmapAtlas::AtlasFolderInfo* dst_child_folder = new BitmapAtlas::AtlasFolderInfo;
			if (dst_child_folder == nullptr)
				continue;

			dst_folder_info->folders.push_back(std::move(std::unique_ptr<BitmapAtlas::AtlasFolderInfo>(dst_child_folder)));
			DoCopyFolder(dst_child_folder, src_child_folder);
		}
		return true;
	}

}; // namespace chaos

