#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ImageDescription.h>
#include <chaos/JSONTools.h>

namespace chaos
{
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

	/** JSON saving entry point */
	bool SaveIntoJSON(nlohmann::json& json_entry, ImageProcessor const& src);
	/** JSON loading entry point */
	bool LoadFromJSON(nlohmann::json const& json_entry, ImageProcessor& dst);

}; // namespace chaos
