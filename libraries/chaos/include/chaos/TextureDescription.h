#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/PixelFormat.h>

namespace chaos
{
	/**
	* SurfaceDescription : used to describe an openGL surface
	*/

	class SurfaceDescription
	{
	public:

		/** the format of the surface (RGB / RGBA ...) */
		PixelFormat pixel_format;
		/** the image width */
		int    width = 0;
		/** the image height */
		int    height = 0;
		/** the image depth */
		int    depth = 0;

	public:

		/** get the pixel format of the description */
		PixelFormat GetPixelFormat() const;
		/** get the size of the texture */
		glm::ivec2 GetSize(int mipmap = 0) const;
	};


	/**
	* TextureDescription : used to describe an openGL texture
	*/

	class TextureDescription : public SurfaceDescription
	{
	public:

		/** the type of the texture (1D/2D/3D/RECTANGLE/CUBE) */
		GLenum type = GL_NONE;
	};

}; // namespace chaos
