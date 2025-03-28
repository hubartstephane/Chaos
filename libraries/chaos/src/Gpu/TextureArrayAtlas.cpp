#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace BitmapAtlas
	{
		void TextureArrayAtlas::Clear()
		{
			BitmapAtlas::AtlasBase::Clear();
			texture = nullptr;
		}

		bool TextureArrayAtlas::LoadAtlas(FilePathParam const & path)
		{
			Clear();

			// use a temporary standard atlas to load everything necessary from file
			Atlas atlas;
			if (!atlas.LoadAtlas(path))
				return false;
			return LoadFromBitmapAtlas(std::move(atlas));
		}

		bool TextureArrayAtlas::LoadFromBitmapAtlas(Atlas const & atlas)
		{
			Clear();
			if (!DoLoadFromBitmapAtlas(atlas))
			{
				Clear();
				return false;
			}
			return true;
		}

		bool TextureArrayAtlas::LoadFromBitmapAtlas(Atlas && atlas)
		{
			Clear();
			if (!DoLoadFromBitmapAtlas(std::move(atlas)))
			{
				Clear();
				return false;
			}
			return true;
		}

		bool TextureArrayAtlas::DoGenerateTextureArray(Atlas const & atlas)
		{
			// create and fill a texture array generator
			std::vector<bitmap_ptr> const & bitmaps = atlas.GetBitmaps();

			TextureArrayGenerator generator;
			for (size_t i = 0; i < bitmaps.size(); ++i)
				generator.AddGenerator(new TextureArraySliceGenerator_Image(bitmaps[i].get(), false)); // do not release image, we have a unique_ptr on it

			// generate the texture array
			texture = generator.GenTextureObject();
			if (texture == nullptr)
				return false;
			return true;
		}

		bool TextureArrayAtlas::DoLoadFromBitmapAtlas(Atlas && atlas)
		{
			if (!DoGenerateTextureArray(atlas))
				return false;

			// steal all data
			atlas_count = atlas.atlas_count;
			dimension   = atlas.dimension;
			root_folder = std::move(atlas.root_folder);
			return true;
		}

		bool TextureArrayAtlas::DoLoadFromBitmapAtlas(Atlas const & atlas)
		{
			if (!DoGenerateTextureArray(atlas))
				return false;

			// copy all data
			atlas_count = atlas.atlas_count;
			dimension = atlas.dimension;

			return DoCopyFolder(&root_folder, &atlas.root_folder);
		}

		bool TextureArrayAtlas::DoCopyFolder(FolderInfo * dst_folder_info, FolderInfo const * src_folder_info)
		{
			assert(dst_folder_info != nullptr);
			assert(src_folder_info != nullptr);

			// copy name
			dst_folder_info->SetName(dst_folder_info->GetName());
			dst_folder_info->SetTag(src_folder_info->GetTag());
			// copy bitmaps and characters
			dst_folder_info->bitmaps = src_folder_info->bitmaps;
			dst_folder_info->fonts   = src_folder_info->fonts;
			// recursively copy data
			size_t child_folder_count = src_folder_info->folders.size();
			for (size_t i = 0; i < child_folder_count; ++i)
			{
				FolderInfo const * src_child_folder = src_folder_info->folders[i].get();
				if (src_child_folder == nullptr)
					continue;

				FolderInfo * dst_child_folder = new FolderInfo;
				if (dst_child_folder == nullptr)
					continue;

				dst_folder_info->folders.push_back(std::move(std::unique_ptr<FolderInfo>(dst_child_folder)));
				DoCopyFolder(dst_child_folder, src_child_folder);
			}
			return true;
		}

	}; // namespace BitmapAtlas

}; // namespace chaos

