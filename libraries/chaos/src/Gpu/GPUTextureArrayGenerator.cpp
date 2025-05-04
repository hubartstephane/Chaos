#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================================
	// GPUTextureArraySliceRegistry functions
	// ========================================================================

	bool GPUTextureArraySliceRegistry::InsertSlice(ImageDescription const & description, void * user_data)
	{
		// test whether the slice format is valid
		if (!IsImageSliceValid(description))
			return false;

		// insert the slice into the registry
		GPUTextureArraySliceRegistryEntry entry;
		entry.description = description;
		entry.user_data = user_data;
		slices.push_back(entry);
		return true;
	}

	bool GPUTextureArraySliceRegistry::IsImageSliceValid(ImageDescription const & description) const
	{
		if (!description.IsValid(false))
			return false;
		if (description.IsEmpty(false))
			return false;
		return true;
	}

	// ========================================================================
	// GPUTextureArraySliceGenerator_Image functions
	// ========================================================================

	GPUTextureArraySliceGenerator_Image::~GPUTextureArraySliceGenerator_Image()
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

	void GPUTextureArraySliceGenerator_Image::RegisterSlices(GPUTextureArraySliceRegistry & slice_registry)
	{
		if (image != nullptr)
			slice_registry.InsertSlice(ImageTools::GetImageDescription(image), image);
	}

	bool GPUTextureArraySliceGenerator_Image::PreRegister()
	{
		if (image == nullptr)
		{
			image = ImageTools::LoadImageFromFile(image_path);
			if (image == nullptr)
				return false;
			release_image = true;
		}
		return true;
	}

	// ========================================================================
	// GPUTextureArrayGenerator functions
	// ========================================================================

	GPUTextureArrayGenerator::GPUTextureArrayGenerator(GPUDevice* in_device):
		GPUDeviceResourceInterface(in_device)
	{
	}

	GPUTextureArrayGenerator::~GPUTextureArrayGenerator()
	{
		Clean();
	}

	bool GPUTextureArrayGenerator::AddGenerator(GPUTextureArraySliceGenerator * generator, SliceInfo * slice_info)
	{
		assert(generator != nullptr);

		shared_ptr<GPUTextureArraySliceGenerator> generator_ptr = generator; // want to ensure that if PreRegister(...) fails, the resource is released

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

	void GPUTextureArrayGenerator::Clean()
	{
		generators.clear(); // destroy the intrusive_ptr
	}

	GPUTexture * GPUTextureArrayGenerator::GenTextureObject(PixelFormatMergeParams const & merge_params, GenTextureParameters const & parameters)
	{
		GPUTextureArraySliceRegistry  slice_registry;
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
				entry.slice_info->first_slice = int(c1);
				entry.slice_info->slice_count = int(c2 - c1);
			}

			slice_counts.push_back(c2 - c1); // insert the count of slices inserted for that generator
		}

		// no slice, no texture
		if (generators.size() != 0 && slice_registry.GetSliceCount() == 0)
			return nullptr;

		// search max size and merged pixel format
		PixelFormatMerger pixel_format_merger(merge_params);

		int width = 0;
		int height = 0;
		for (GPUTextureArraySliceRegistryEntry const & entry : slice_registry.slices)
		{
			width = std::max(width, entry.description.width);
			height = std::max(height, entry.description.height);
			pixel_format_merger.Merge(entry.description.pixel_format);
		}

		// test whether the final size is valid
		if (width <= 0 || height <= 0)
			if (generators.size() != 0)
				return nullptr;

		PixelFormat pixel_format = pixel_format_merger.GetResult();
		if (!pixel_format.IsValid())
		{
			if (generators.size() > 0)
				return nullptr;
			pixel_format = (merge_params.pixel_format.IsValid()) ? merge_params.pixel_format : PixelFormat::BGRA;
		}

		// create the texture and fill the slices
		GPUTexture * result = GenTextureObjectHelper(slice_registry, pixel_format, width, height, parameters);
		if (result == nullptr)
			return nullptr;

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

	GPUTexture * GPUTextureArrayGenerator::GenTextureObjectHelper(GPUTextureArraySliceRegistry & slice_registry, PixelFormat const & pixel_format, int width, int height, GenTextureParameters const & parameters) const
	{
		// the number of slices
		int slice_count = (int)slice_registry.slices.size();
		if (slice_count == 0)
			return nullptr;

		// compute the 'flat' texture target
		TextureType flat_type = GLTextureTools::GetTexture2DTypeFromSize(width, height);
		if (flat_type == TextureType::Unknown)
			return nullptr;

		// convert to 'array' target
		TextureType array_type = GLTextureTools::ToArrayTextureType(flat_type);
		if (array_type == TextureType::Unknown)
			return nullptr;

		// find whether some conversion will be necessary (same remarks than for GLTextureTools::GenTextureObject(CubeMapImages ...)
		// and allocate the buffer
		char* conversion_buffer = nullptr;

		if (pixel_format.component_count != 1) // destination is not GRAY, the RED texture + SWIZZLE cannot be used on independant slices
		{
			size_t required_allocation = 0;
			for (size_t i = 0; i < slice_count; ++i)
			{
				ImageDescription const& desc = slice_registry.slices[i].description;
				if (desc.pixel_format.component_count == 1)
					required_allocation = std::max(required_allocation, (size_t)ImageTools::GetMemoryRequirementForAlignedTexture(pixel_format, desc.width, desc.height)); // slice is GRAY
			}
			if (required_allocation > 0)
			{
				conversion_buffer = new char[required_allocation];
				if (conversion_buffer == nullptr)
					return nullptr;
			}
		}

		// create the texture
		TextureDescription description;
		description.width        = width;
		description.height       = height;
		description.depth        = slice_count;
		description.pixel_format = pixel_format;
		description.type         = array_type;
		description.use_mipmaps  = parameters.reserve_mipmaps;

		GPUTexture* result = GetGPUDevice()->CreateTexture(description);
		if (result == nullptr)
		{
			if (conversion_buffer != nullptr)
				delete[](conversion_buffer);
			return nullptr;
		}

		// fill each slices into GPU
		for (int i = 0; i < slice_count; ++i)
		{
			ImageDescription const & image = slice_registry.slices[i].description;

			ImageDescription effective_image = (pixel_format.component_count != 1 && image.pixel_format.component_count == 1)?
				ImageTools::ConvertPixels(image, pixel_format, conversion_buffer, ImageTransform::NO_TRANSFORM) :
				image;

			result->SetSubImage(effective_image, {0, 0, i});
		}

		result->SetMinificationFilter(parameters.min_filter);
		result->SetMagnificationFilter(parameters.mag_filter);
		result->SetWrapMethods(parameters.wrap_methods);

		if (parameters.build_mipmaps && parameters.reserve_mipmaps)
			result->GenerateMipmaps();

		// release the conversion buffer if necessary
		if (conversion_buffer != nullptr)
			delete[](conversion_buffer);

		return result;
	}

}; // namespace chaos