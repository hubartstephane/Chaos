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

		bool TextureArrayAtlas::LoadAtlas(boost::filesystem::path const & filename)
		{
			Clear();

			// use a temporary standard atlas to load everything necessary from file
			Atlas atlas;
			if (!atlas.LoadAtlas(filename))
				return false;

			// create and fill a texture array generator
			std::vector<unique_bitmap_ptr> const & bitmaps = atlas.GetBitmaps();

			TextureArrayGenerator generator;
			for (size_t i = 0; i < bitmaps.size(); ++i)
				generator.AddGenerator(new TextureArraySliceGenerator_Image(bitmaps[i].get(), false)); // do not release image, we have a unique_ptr on it

																					  // generate the texture array
			texture = generator.GenerateTexture();
			if (texture == nullptr)
			{
				Clear();
				return false;
			}

			// capture all data
			atlas_count    = atlas.atlas_count;
			dimension      = atlas.dimension;
			bitmap_sets    = std::move(atlas.bitmap_sets);
			character_sets = std::move(atlas.character_sets);
			return true;
		}
	};
};

