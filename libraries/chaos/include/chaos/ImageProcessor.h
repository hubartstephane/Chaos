#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
#include <chaos/ImageDescription.h>
#include <chaos/ImageAnimationDescription.h>
#include <chaos/GeometryFramework.h>
#include <chaos/JSONTools.h>
#include <chaos/Class.h>
#include <chaos/Metaprogramming.h>
#include <chaos/ColorFilter.h>
#include <chaos/JSONSerializable.h>

namespace chaos
{
	/**
	* ImageProcessor : take an image as an entry and returns a new image as output
	*/

	class ImageProcessor : public Object, public JSONSerializable
	{
		CHAOS_OBJECT_DECLARE_CLASS2(ImageProcessor, Object);

	public:


		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const;

		/** apply processing on a whole animation (grid or not) */
		virtual std::vector<FIBITMAP*> ProcessImageFrames(std::vector<FIBITMAP*> const& src, BitmapGridAnimationInfo const & grid_anim) const;

		/** the processor may save its configuration into a JSON file */
		virtual bool SaveIntoJSON(nlohmann::json& json_entry) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool LoadFromJSON(nlohmann::json const& json_entry) override;

	protected:

		/** an utility function to call functor on pixel accessor */
		template<typename FUNC>
		static FIBITMAP * DoImageProcessing(ImageDescription const& src_desc, FUNC func)
		{
			FIBITMAP* result = nullptr;

			// iterate over all PixelTypes
			meta::for_each<PixelTypes>([src_desc, func, &result](auto value) -> bool // XXX: value is an instance of identity<pixel_type>
			{
				using pixel_type = typename decltype(value)::type;
				if (src_desc.pixel_format == PixelFormat::GetPixelFormat<pixel_type>())
				{
					result = func(ImagePixelAccessor<pixel_type>(src_desc)); 
					return true;
				}
				return false;

			}, false);

			return result;
		}	
	};

	/**
	* ImageProcessorOutline : add an outline to any image
	*/

	class ImageProcessorOutline : public ImageProcessor
	{
		CHAOS_OBJECT_DECLARE_CLASS2(ImageProcessorOutline, ImageProcessor);

	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const override;

		/** the processor may save its configuration into a JSON file */
		virtual bool SaveIntoJSON(nlohmann::json& json_entry) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool LoadFromJSON(nlohmann::json const& json_entry) override;

	public:

		/** the size of the outline */
		int distance = 1;
		/** filter to check pixel to keep */
		ColorFilter color_filter;
		/** the ouline color */
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		/** the empty color */
		glm::vec4 empty_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	};

	/**
	* ImageProcessorOutline : add an alpha channel according to RGB value
	*/

	class ImageProcessorAddAlpha : public ImageProcessor
	{
		CHAOS_OBJECT_DECLARE_CLASS2(ImageProcessorAddAlpha, ImageProcessor);

	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const override;

		/** the processor may save its configuration into a JSON file */
		virtual bool SaveIntoJSON(nlohmann::json& json_entry) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool LoadFromJSON(nlohmann::json const& json_entry) override;
	};



















	/**
	* ImageProcessorShadow : add a shadow to an image
	*/

	class ImageProcessorShadow : public ImageProcessor
	{
		CHAOS_OBJECT_DECLARE_CLASS2(ImageProcessorShadow, ImageProcessor);

	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const override;

		/** the processor may save its configuration into a JSON file */
		virtual bool SaveIntoJSON(nlohmann::json& json_entry) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool LoadFromJSON(nlohmann::json const& json_entry) override;

	public:

		/** the offset of the shadow */
		glm::vec2 offset = glm::vec2(5, 5);
		/** filter to check pixel to keep */
		ColorFilter color_filter;
		/** the ouline color */
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		/** the empty color */
		glm::vec4 empty_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	};

}; // namespace chaos
