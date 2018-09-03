#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/GPUTexture.h>
#include <chaos/FilePath.h>

namespace chaos
{
	namespace BitmapAtlas
	{
		class TextureArrayAtlas : public AtlasBase
		{
		public:

			/** the clearing method */
			virtual void Clear() override;
			/** load an atlas from an index file */
			bool LoadAtlas(FilePathParam const & path);
			/** generate a texture atlas from a standard atlas */
			bool LoadFromBitmapAtlas(Atlas const & atlas);
			/** generate a texture atlas from a standard atlas */
			bool LoadFromBitmapAtlas(Atlas && atlas);

			/* get the array texture */
			GPUTexture * GetTexture() { return texture.get(); }

		protected:

			/** generate a texture atlas from a standard atlas */
			bool DoLoadFromBitmapAtlas(Atlas const & atlas);
			/** generate a texture atlas from a standard atlas */
			bool DoLoadFromBitmapAtlas(Atlas && atlas);
			/** copy src folder into dst folder */
			bool DoCopyFolder(FolderInfo * dst_folder_info, FolderInfo const * src_folder_info);
			/** generate a texture array */
			bool DoGenerateTextureArray(Atlas const & atlas);

		protected:

			/** the texture array that will be generated */
			boost::intrusive_ptr<GPUTexture> texture;
		};
	}; // namespace BitmapAtlas
}; // namespace chaos
