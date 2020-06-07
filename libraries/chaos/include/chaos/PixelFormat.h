#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/PixelTypes.h>

namespace chaos
{
	/** 
	 * the type of each component of each pixels 
	 */
	 
	enum class PixelComponentType : int
	{
		UNKNOWN = 0,
		UNSIGNED_CHAR = 1,
		FLOAT = 2,
		DEPTH_STENCIL = 3
	};


	/** JSON loading method */
	bool LoadFromJSON(nlohmann::json const& json_entry, PixelComponentType& dst);
	/** JSON saving method */
	bool SaveIntoJSON(nlohmann::json& json_entry, PixelComponentType const& src);


	/** 
	 * the well known formats 
	 */
	enum class PixelFormatType : int
	{
		UNKNOWN = 0,
		GRAY = 1,
		RGB = 2,
		RGBA = 3,
		GRAY_FLOAT = 4,
		RGB_FLOAT = 5,
		RGBA_FLOAT = 6,
		DEPTH_STENCIL = 7
	};

	/** 
	* PixelFormat : the accepted pixel formats
	*/

	class PixelFormat
	{

	public:

		/** constructor */
		PixelFormat() = default;

		PixelFormat(PixelFormat const & other) = default;

		PixelFormat(PixelComponentType in_component_type, int in_component_count) :
			component_type(in_component_type), 
			component_count(in_component_count){};

		PixelFormat(PixelFormatType in_format);

		/** get the size of one pixel */
		int GetPixelSize() const;
		/** get the known type of one pixel */
		PixelFormatType GetFormat() const;
		/** returns true whether the pixel format is handled */
		bool IsValid() const;
		/** returns true whether the pixel is a standard 'color' pixel */
		bool IsColorPixel() const;
		/** returns true whether the pixel is a special 'depth stencil' pixel */
		bool IsDepthStencilPixel() const;

		/** returns true whether the pixel format are same */
		bool operator == (PixelFormat const & other) const;
		/** returns true whether the pixel format are different */
		bool operator != (PixelFormat const & other) const;

		/** a static instance of all pixel formats */
		static PixelFormat const Gray;
		static PixelFormat const BGR;
		static PixelFormat const BGRA;
		static PixelFormat const GrayFloat;
		static PixelFormat const RGBFloat;
		static PixelFormat const RGBAFloat;
		static PixelFormat const DepthStencil;

		/** transform a type into pixel format (component type and count) */
		template<typename T>
		static PixelFormat GetPixelFormat();

		template<>
		static PixelFormat GetPixelFormat<PixelGray>(){ return Gray;}
		template<>
		static PixelFormat GetPixelFormat<PixelBGR>(){ return BGR;}
		template<>
		static PixelFormat GetPixelFormat<PixelBGRA>(){ return BGRA;}
		template<>
		static PixelFormat GetPixelFormat<PixelGrayFloat>(){ return GrayFloat;}
		template<>
		static PixelFormat GetPixelFormat<PixelRGBFloat>(){ return RGBFloat;}
		template<>
		static PixelFormat GetPixelFormat<PixelRGBAFloat>(){ return RGBAFloat;}
		template<>
		static PixelFormat GetPixelFormat<PixelDepthStencil>() { return DepthStencil; }

	public:

		/** the type of the components */
		PixelComponentType component_type = PixelComponentType::UNKNOWN;
		/** the number of components for each pixels */
		int component_count = 0;		
	};

	bool LoadFromJSON(nlohmann::json const& json_entry, PixelFormat& dst);

	bool SaveIntoJSON(nlohmann::json& json_entry, PixelFormat const& src);

	/**
	* PixelFormatMergeParams
	*/

	class PixelFormatMergeParams
	{

	public:

		/** if valid, the resulting pixel format should be set to this */
		PixelFormat pixel_format;
		/** if false the pixel_format cannot be 'upgraded' with incomming format */
		bool upgrade_pixel_format = false;
		/** if set to false, all luminances formats will become RGB */
		bool accept_luminance = true;
		/** if set to false, all float formats will become unsigned char */
		bool accept_float = true;	
	};

	bool LoadFromJSON(nlohmann::json const& json_entry, PixelFormatMergeParams& dst);

	bool SaveIntoJSON(nlohmann::json& json_entry, PixelFormatMergeParams const& src);

	/**
	* PixelFormatMerger : utility function to find a common PixelFormat for multiple images
	*/

	class PixelFormatMerger
	{

	public:

		/** constructor */
		PixelFormatMerger(PixelFormatMergeParams const & in_params = PixelFormatMergeParams());

		/** reset the merger */
		void Reset(PixelFormatMergeParams const & in_params = PixelFormatMergeParams());
		/** the merge method */
		void Merge(PixelFormat src);
		/** get the result if available */
		PixelFormat GetResult() const;

	protected:

		/** the result */
		PixelFormat result;
		/** whether a valid result is available */
		bool result_is_available = false;
		/** the parameter for the merge */
		PixelFormatMergeParams params;
	};

}; // namespace chaos

