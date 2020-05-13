#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ImageDescription.h>
#include <chaos/GeometryFramework.h>
#include <chaos/JSONTools.h>

namespace chaos
{

	//DECLARE_IMAGE_PROCESSOR(ImageProcessor);

	/**
	* ImageProcessor : take an image as an entry and returns a new image as output
	*/

	class ImageProcessor : public ReferencedObject
	{
	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc);

		/** the processor may save its configuration into a JSON file */
		virtual bool SaveIntoJSON(nlohmann::json& json_entry) const;
		/** the processor may save its configuration from a JSON file */
		virtual bool LoadFromJSON(nlohmann::json const& json_entry);
	};

	/**
	* ImageProcessorOutline : add an outline to any image
	*/

	//DECLARE_IMAGE_PROCESSOR(ImageProcessorOutline);

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


}; // namespace chaos
