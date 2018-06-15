#include <chaos/TextureArrayAtlas.h>
#include <chaos/TextureArrayGenerator.h>

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
			return LoadFromBitmapAtlas(atlas);
		}

		bool TextureArrayAtlas::LoadFromBitmapAtlas(Atlas const & atlas)
		{
			Clear();

			// create and fill a texture array generator
			std::vector<unique_bitmap_ptr> const & bitmaps = atlas.GetBitmaps();

			TextureArrayGenerator generator;
			for (size_t i = 0; i < bitmaps.size(); ++i)
				generator.AddGenerator(new TextureArraySliceGenerator_Image(bitmaps[i].get(), false)); // do not release image, we have a unique_ptr on it

			// generate the texture array
			texture = generator.GenTextureObject();
			if (texture == nullptr)
			{
				Clear();
				return false;
			}

			// copy all data
			atlas_count    = atlas.atlas_count;
			dimension      = atlas.dimension;

			for (size_t i = 0 ; i < atlas.bitmap_sets.size() ; ++i)
			{
				BitmapSet * bitmap_set = new BitmapSet();
				if (bitmap_set == nullptr)
					break;
				*bitmap_set = *atlas.bitmap_sets[i];
				bitmap_sets.push_back(std::move(std::unique_ptr<BitmapSet>(bitmap_set)));
			}

			for (size_t i = 0 ; i < atlas.character_sets.size() ; ++i)
			{
				CharacterSet * character_set = new CharacterSet();
				if (character_set == nullptr)
					break;
				*character_set = *atlas.character_sets[i];
				character_sets.push_back(std::move(std::unique_ptr<CharacterSet>(character_set)));
			}
			return true;
		}
	};
};

