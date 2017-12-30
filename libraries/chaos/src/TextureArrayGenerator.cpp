#include <chaos/TextureArrayGenerator.h>
#include <chaos/GLTextureTools.h>

namespace chaos
{
	// ========================================================================
	// TextureArraySliceRegistry functions
	// ========================================================================

	bool TextureArraySliceRegistry::InsertSlice(ImageDescription & description, void * user_data)
	{
		// test whether the slice format is valid
		if (!IsImageSliceValid(description))
			return false;

		// insert the slice into the registry
		TextureArraySliceRegistryEntry entry;
		entry.description = description;
		entry.user_data = user_data;
		slices.push_back(entry);
		return true;
	}

	bool TextureArraySliceRegistry::IsImageSliceValid(ImageDescription const & description) const
	{
		if (!description.IsValid())
			return false;
		if (description.IsEmpty())
			return false;
		return true;
	}

	// ========================================================================
	// TextureArraySliceGenerator_Image functions
	// ========================================================================

	TextureArraySliceGenerator_Image::~TextureArraySliceGenerator_Image()
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

	void TextureArraySliceGenerator_Image::RegisterSlices(TextureArraySliceRegistry & slice_registry)
	{
		if (image != nullptr)
			slice_registry.InsertSlice(ImageTools::GetImageDescription(image), image);
	}

	bool TextureArraySliceGenerator_Image::PreRegister()
	{
		if (image == nullptr)
		{
			image = ImageTools::LoadImageFromFile(image_path.string().c_str());
			if (image == nullptr)
				return false;
			release_image = true;
		}	
		return true;
	}

	// ========================================================================
	// TextureArrayGenerator functions
	// ========================================================================

	TextureArrayGenerator::~TextureArrayGenerator()
	{
		Clean();
	}

	bool TextureArrayGenerator::AddGenerator(TextureArraySliceGenerator * generator, SliceInfo * slice_info)
	{
		assert(generator != nullptr);

		boost::intrusive_ptr<TextureArraySliceGenerator> generator_ptr = generator; // want to ensure that if PreRegister(...) fails, the resource is released

		// prepare the generator
		if (!generator->PreRegister())
			return false;
		// insert it into the list
		GeneratorEntry entry;
		entry.generator = generator_ptr;
		entry.slice_info = slice_info;
		generators.push_back(entry);
		return true;
	}

	void TextureArrayGenerator::Clean()
	{
		generators.clear(); // destroy the intrusive_ptr
	}

	boost::intrusive_ptr<Texture> TextureArrayGenerator::GenerateTexture(PixelFormatMergeParams const & merge_params, GenTextureParameters const & parameters)
	{
		TextureArraySliceRegistry  slice_registry;
		std::vector<size_t> slice_counts;
		// insert all slices
		for (size_t i = 0; i < generators.size(); ++i)
		{
			GeneratorEntry & entry = generators[i];

			// fill the slice registry. keep trace of how many insertion
			size_t c1 = slice_registry.GetSliceCount();
			entry.generator->RegisterSlices(slice_registry);
			size_t c2 = slice_registry.GetSliceCount();

			// return back the slices values
			if (entry.slice_info != nullptr)
			{
				entry.slice_info->first_slice = (int)c1;
				entry.slice_info->slice_count = (int)(c2 - c1);
			}

			slice_counts.push_back(c2 - c1); // insert the count of slices inserted for that generator
		}

		// no slice, no texture
		if (slice_registry.GetSliceCount() == 0)
			return nullptr;

		// search max size and merged pixel format
		PixelFormatMerger pixel_format_merger(merge_params);

		int width = 0;
		int height = 0;
		for (TextureArraySliceRegistryEntry const & entry : slice_registry.slices)
		{
			width = max(width, entry.description.width);
			height = max(height, entry.description.height);
			pixel_format_merger.Merge(entry.description.pixel_format);
		}

		// test whether the final size is valid
		if (width <= 0 || height <= 0)
			return nullptr;

		PixelFormat pixel_format = pixel_format_merger.GetResult();
		if (!pixel_format.IsValid())
			return nullptr;

		// create the texture and fill the slices
		GenTextureResult texture_result = GenerateTexture(slice_registry, pixel_format, width, height, parameters);
		if (texture_result.texture_id <= 0)
			return nullptr;

		boost::intrusive_ptr<Texture> result = new Texture(texture_result.texture_id, texture_result.texture_description);

		// release slices
		size_t start = 0;
		for (size_t i = 0; i < generators.size(); ++i)
		{
			GeneratorEntry entry = generators[i];

			size_t count = slice_counts[i];
			entry.generator->ReleaseSlices(&slice_registry.slices[start], count); // each generator is responsible for releasing its own slices
			start += count;
		}

		return result;
	}

	GenTextureResult TextureArrayGenerator::GenerateTexture(TextureArraySliceRegistry & slice_registry, PixelFormat const & final_pixel_format, int width, int height, GenTextureParameters const & parameters) const
	{
		GenTextureResult result;

		// compute the 'flat' texture target
		GLenum flat_target = GLTextureTools::GetTextureTargetFromSize(width, height, false);
		if (flat_target == GL_NONE)
			return result;

		// convert to 'array' target 
		GLenum array_target = GLTextureTools::ToArrayTextureType(flat_target);
		if (array_target == GL_NONE)
			return result;

		// choose format and internal format
		GLPixelFormat gl_pixel_format = GLTextureTools::GetGLPixelFormat(final_pixel_format);
		if (!gl_pixel_format.IsValid())
			return result;

		// the number of slices
		size_t slice_count = slice_registry.slices.size();

		// find whether some conversion will be necessary (same remarks than for GLTextureTools::GenTexture(SkyBoxImages ...)
		// and allocate the buffer
		char * conversion_buffer = nullptr;

		if (final_pixel_format.component_count != 1) // destination is not GRAY, the RED texture + SWIZZLE cannot be used on independant slices
		{
			size_t required_allocation = 0;
			for (size_t i = 0; i < slice_count ; ++i)
			{
				ImageDescription const & desc = slice_registry.slices[i].description;
				if (desc.pixel_format.component_count == 1)
					required_allocation = max(required_allocation, (size_t)ImageTools::GetMemoryRequirementForAlignedTexture(final_pixel_format, desc.width, desc.height)); // slice is GRAY
			}
			if (required_allocation > 0)
			{
				conversion_buffer = new char[required_allocation];
				if (conversion_buffer == nullptr)
					return result;
			}
		}

		// generate the texture
		glCreateTextures(array_target, 1, &result.texture_id);
		if (result.texture_id > 0)
		{
			// initialize the storage
			int level_count = GLTextureTools::GetMipmapLevelCount(width, height);
			glTextureStorage3D(result.texture_id, level_count, gl_pixel_format.internal_format, width, height, (GLsizei)slice_count);

			// fill each slices into GPU
			for (size_t i = 0; i < slice_count; ++i)
			{
				ImageDescription image = slice_registry.slices[i].description;

				ImageDescription effective_image = (final_pixel_format.component_count != 1 && image.pixel_format.component_count == 1)?
					ImageTools::ConvertPixels(image, final_pixel_format, conversion_buffer, false) :
					image;

				char * texture_buffer = GLTextureTools::PrepareGLTextureTransfert(effective_image);
				if (texture_buffer != nullptr)
				{
					int type = (effective_image.pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR) ? GL_UNSIGNED_BYTE : GL_FLOAT;

					GLPixelFormat slice_pixel_format = GLTextureTools::GetGLPixelFormat(effective_image.pixel_format);
					glTextureSubImage3D(result.texture_id, 0, 0, 0, (GLsizei)i, effective_image.width, effective_image.height, 1, slice_pixel_format.format, type, texture_buffer);		
				}
			}

			// finalize the result data
			result.texture_description.type = array_target;
			result.texture_description.width = width;
			result.texture_description.height = height;
			result.texture_description.depth = (int)slice_count;
			result.texture_description.internal_format = gl_pixel_format.internal_format;

			GLTextureTools::GenTextureApplyParameters(result, parameters);
		}

		// release the conversion buffer if necessary
		if (conversion_buffer != nullptr)
			delete[](conversion_buffer);

		return result;
	}

};