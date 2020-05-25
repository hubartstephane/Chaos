#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
#include <chaos/ImageDescription.h>
#include <chaos/GeometryFramework.h>
#include <chaos/JSONTools.h>
#include <chaos/Class.h>

namespace chaos
{

	/**
	* ImageProcessor : take an image as an entry and returns a new image as output
	*/

	class ImageProcessor : public Object
	{
	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc);

		/** the processor may save its configuration into a JSON file */
		virtual bool SaveIntoJSON(nlohmann::json& json_entry) const;
		/** the processor may save its configuration from a JSON file */
		virtual bool LoadFromJSON(nlohmann::json const& json_entry);
	};

	CHAOS_REGISTER_CLASS1(ImageProcessor);

	/**
	* ImageProcessorOutline : add an outline to any image
	*/

	class ImageProcessorOutline : public ImageProcessor
	{
	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) override;

		/** the processor may save its configuration into a JSON file */
		virtual bool SaveIntoJSON(nlohmann::json& json_entry) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool LoadFromJSON(nlohmann::json const& json_entry) override;

	protected:

		/** the distance to check for adding an outline */
		float distance = 5.0f;
		/** the ouline color */
		glm::vec4 outline_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};

	CHAOS_REGISTER_CLASS2(ImageProcessorOutline, ImageProcessor);


}; // namespace chaos
