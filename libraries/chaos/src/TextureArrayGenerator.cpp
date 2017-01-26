#include <chaos/TextureArrayGenerator.h>
#include <chaos/GLTextureTools.h>

namespace chaos
{
	// ========================================================================
	// ImageSliceRegister functions
	// ========================================================================

	bool ImageSliceRegister::InsertSlice(ImageDescription & description, void * user_data)
	{
		// test whether the slice format is valid
		if (!IsImageSliceValid(description))
			return false;      

		// insert the slice into the register
		ImageSliceRegisterEntry entry;
		entry.description = description;
		entry.user_data   = user_data;
		slices.push_back(entry);
		return true;
	}

	bool ImageSliceRegister::IsImageSliceValid(ImageDescription const & description) const
	{
		if (!description.IsValid())
			return false;
		if (description.IsEmpty())
			return false;
		if (description.component_type != ImageDescription::TYPE_UNSIGNED_CHAR)
			return false;
		return true;
	}

	// ========================================================================
	// ImageSliceGenerator functions
	// ========================================================================

	ImageSliceGeneratorProxyBase * ImageSliceGenerator::CreateProxy() const
	{
		if (image != nullptr)
			return new ImageSliceGeneratorProxy(image, release_image);
		else
		{
			FIBITMAP * image = ImageTools::LoadImageFromFile(image_path.string().c_str());
			if (image != nullptr)
				return new ImageSliceGeneratorProxy(image, true);
		}
		return nullptr;
	}

	// ========================================================================
	// ImageSliceGeneratorProxy functions
	// ========================================================================

	ImageSliceGeneratorProxy::~ImageSliceGeneratorProxy()
	{
		if (release_image)
		{
			if (image != nullptr)
			{
				FreeImage_Unload(image);
				image = nullptr;
			}
		}
	}

	void ImageSliceGeneratorProxy::AddSlices(ImageSliceRegister & slice_register)
	{
		if (image != nullptr)
			slice_register.InsertSlice(ImageTools::GetImageDescription(image), image);
	}

	void ImageSliceGeneratorProxy::ReleaseSlices(ImageSliceRegisterEntry * slices, size_t count)
	{
		// no releasing here. wait until proxy destructor is called.
		// because if user wants to call 
		//   - generator.GenerateTexture();   twice in a row, we still expect to generate a texture array twice (an effective clean would break that)
	}

	// ========================================================================
	// TextureArrayGenerator functions
	// ========================================================================

	TextureArrayGenerator::~TextureArrayGenerator()
	{
		Clean();
	}

	bool TextureArrayGenerator::AddGenerator(ImageSliceGenerator const & generator, SliceInfo * slice_info)
	{
		// creates the proxy
		ImageSliceGeneratorProxyBase * proxy = generator.CreateProxy();
		if (proxy == nullptr)
			return false;
		// insert it into the list
		GeneratorEntry entry;
		entry.proxy      = proxy;
		entry.slice_info = slice_info;
		generators.push_back(entry);
		return true;
	}

	void TextureArrayGenerator::Clean()
	{
		for (auto it : generators)
			delete (it.proxy);
		generators.clear(); // destroy the intrusive_ptr
	}

	boost::intrusive_ptr<Texture> TextureArrayGenerator::GenerateTexture(GenTextureParameters const & parameters)
	{
		ImageSliceRegister  slice_register;
		std::vector<size_t> slice_counts;
		// insert all slices
		for (size_t i = 0 ; i < generators.size() ; ++i)            
		{
			GeneratorEntry entry = generators[i];

			// fill the slice register. keep trace of how many insertion
			size_t c1 = slice_register.size();
			entry.proxy->AddSlices(slice_register);
			size_t c2 = slice_register.size();

			// return back the slices values
			if (entry.slice_info != nullptr)
			{
				entry.slice_info->first_slice = (int)c1;
				entry.slice_info->slice_count = (int)(c2 - c1);
			}

			slice_counts.push_back(c2 - c1); // insert the count of slices inserted for that generator
		}

		// no slice, no texture
		if (slice_register.size() == 0)
			return nullptr;

		// search max size, max bpp, and final target (GL_TEXTURE_1D_ARRAY or GL_TEXTURE_2D_ARRAY)
		int width  = 0;
		int height = 0;
		int bpp    = 0;
		for (ImageSliceRegisterEntry const & entry : slice_register.slices)
		{
			width  = max(width,  entry.description.width);
			height = max(height, entry.description.height);
			bpp    = max(bpp,    entry.description.GetBPP());
		}

		// create the texture and fill the slices
		boost::intrusive_ptr<Texture> result = GenerateTexture(slice_register, bpp, width, height, parameters);

		// release slices
		size_t start = 0;
		for (size_t i = 0; i < generators.size(); ++i)
		{
			GeneratorEntry entry = generators[i];

			size_t count = slice_counts[i];
			entry.proxy->ReleaseSlices(&slice_register.slices[start], count); // each proxy is responsible for releasing its own slices
			start += count;
		}

		// clean the generator
		Clean();

		return result;
	}

	boost::intrusive_ptr<Texture> TextureArrayGenerator::GenerateTexture(ImageSliceRegister & slice_register, int bpp, int width, int height, GenTextureParameters const & parameters) const
	{
		// compute the 'flat' texture target
		GLenum flat_target = GLTextureTools::GetTextureTargetFromSize(width, height, false);
		// convert to 'array' target 
		GLenum array_target = GLTextureTools::ToArrayTextureType(flat_target);
		assert(array_target != GL_NONE);

		GenTextureResult result;
		glCreateTextures(array_target, 1, &result.texture_id);
		if (result.texture_id > 0)
		{
			// choose format and internal format (beware FreeImage is BGR/BGRA)
			GLenum internal_format = GLTextureTools::GetTextureFormatsFromBPP(bpp).second;
			assert(internal_format != GL_NONE);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			int level_count = GLTextureTools::GetMipmapLevelCount(width, height);
			glTextureStorage3D(result.texture_id, level_count, internal_format, width, height, slice_register.size());

			for (size_t i = 0; i < slice_register.size(); ++i)
			{
				ImageDescription desc = slice_register.slices[i].description;

				int bpp  = desc.GetBPP();
				int type = desc.component_type == (ImageDescription::TYPE_UNSIGNED_CHAR)? GL_UNSIGNED_BYTE : GL_FLOAT;

				glPixelStorei(GL_UNPACK_ROW_LENGTH, 8 * desc.pitch_size / bpp);

				GLenum current_format = GLTextureTools::GetTextureFormatsFromBPP(bpp).first;
				assert(current_format != GL_NONE);
				glTextureSubImage3D(result.texture_id, 0, 0, 0, i, desc.width, desc.height, 1, current_format, type, desc.data);
			}

			result.texture_description.type   = GL_TEXTURE_2D_ARRAY;
			result.texture_description.width  = width;
			result.texture_description.height = height;
			result.texture_description.depth  = slice_register.size();
			result.texture_description.internal_format = internal_format;

			GLTextureTools::GenTextureApplyParameters(result, parameters);

			return new Texture(result.texture_id, result.texture_description);
		}
		return nullptr;
	}

};