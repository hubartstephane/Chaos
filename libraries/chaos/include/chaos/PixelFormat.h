#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/PixelTypes.h>

namespace chaos
{
	/** 
	* PixelFormat : the accepted pixel formats
	*/

	class PixelFormat
	{
	public:

		/** the type of each component of each pixels */
		static int const TYPE_UNKNOWN = 0;
		static int const TYPE_UNSIGNED_CHAR = 1;
		static int const TYPE_FLOAT = 2;
		static int const TYPE_DEPTH_STENCIL = 3;

		/** the well known formats */
		static int const FORMAT_UNKNOWN       = 0;
		static int const FORMAT_GRAY          = 1;
		static int const FORMAT_RGB           = 2;
		static int const FORMAT_RGBA          = 3;
		static int const FORMAT_GRAY_FLOAT    = 4;
		static int const FORMAT_RGB_FLOAT     = 5;
		static int const FORMAT_RGBA_FLOAT    = 6;
		static int const FORMAT_DEPTH_STENCIL = 7;

	public:

		/** constructor */
		PixelFormat() = default;

		PixelFormat(PixelFormat const & other) = default;

		PixelFormat(int in_component_type, int in_component_count) : 
			component_type(in_component_type), 
			component_count(in_component_count){};

		PixelFormat(int in_format);

		/** get the size of one pixel */
		int GetPixelSize() const;
		/** get the known type of one pixel */
		int GetFormat() const;
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

		/** transform a type into pixel format (component type and count) */
		template<typename T>
		static PixelFormat GetPixelFormat();

		template<>
		static PixelFormat GetPixelFormat<PixelGray>(){ return PixelFormat(TYPE_UNSIGNED_CHAR, 1);}
		template<>
		static PixelFormat GetPixelFormat<PixelBGR>(){ return PixelFormat(TYPE_UNSIGNED_CHAR, 3);}
		template<>
		static PixelFormat GetPixelFormat<PixelBGRA>(){ return PixelFormat(TYPE_UNSIGNED_CHAR, 4);}
		template<>
		static PixelFormat GetPixelFormat<PixelGrayFloat>(){ return PixelFormat(TYPE_FLOAT, 1);}
		template<>
		static PixelFormat GetPixelFormat<PixelRGBFloat>(){ return PixelFormat(TYPE_FLOAT, 3);}
		template<>
		static PixelFormat GetPixelFormat<PixelRGBAFloat>(){ return PixelFormat(TYPE_FLOAT, 4);}
		template<>
		static PixelFormat GetPixelFormat<PixelDepthStencil>() { return PixelFormat(TYPE_DEPTH_STENCIL, 1); }

	public:

		/** the type of the components */
		int component_type = TYPE_UNKNOWN;
		/** the number of components for each pixels */
		int component_count = 0;		
	};

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

