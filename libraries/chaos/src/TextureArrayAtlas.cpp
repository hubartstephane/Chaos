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
			atlas_count    = atlas.atlas_count;
			dimension      = atlas.dimension;
			bitmap_sets    = std::move(atlas.bitmap_sets);
			character_sets = std::move(atlas.character_sets);
			return true;		
		}

		bool TextureArrayAtlas::DoLoadFromBitmapAtlas(Atlas const & atlas)
		{
			if (!DoGenerateTextureArray(atlas))
				return false;

			// copy all data
			atlas_count = atlas.atlas_count;
			dimension   = atlas.dimension;

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
	}; // namespace BitmapAtlas
}; // namespace chaos

