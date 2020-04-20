#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/PixelFormat.h>

namespace chaos
{
	/**
	* BitmapGridAnimationInfo : some bitmaps represent a uniform grid of individual animation frames
	*/

	class BitmapGridAnimationInfo
	{
	public:

		/** the size of the grid */
		glm::ivec2 grid_size = glm::ivec2(0, 0);
		/** the last images that are not filled */
		int skip_lasts = 0;

	public:

		/** return the number of frames of the animation */
		size_t GetFrameCount() const;

		/** parsing the the name to extract the grid numbers */
		static bool ParseFromName(char const * name, BitmapGridAnimationInfo & result, std::string * name_result = nullptr);

	protected:

		/** utility method */
		static bool ParseFromNameReadGridX(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result = nullptr);
		/** utility method */
		static bool ParseFromNameReadGridY(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result = nullptr);
		/** utility method */
		static bool ParseFromNameReadGridSkip(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result = nullptr);
		/** utility method */
		static bool ParseDigitReverse(char const * str, int & start);
	};

	/**
	* ImageAnimationDescription : data to work with animation
	*/

	class ImageAnimationDescription
	{
	public:

		/** returns true whether the animation is of type frame (i.e GIF) */
		bool IsFrameAnimation() const;
		/** returns true whether the animation is of type grid */
		bool IsGridAnimation() const;
		/** get the duration of a frame in seconds */
		float GetFrameTime() const { return frame_time; }
		/** return the number of frames of the animation */
		size_t GetFrameCount() const;

	public:

		/** if the animation is stored inside a grid */
		BitmapGridAnimationInfo grid_data;
		/** number of child frame (directly following the BitmapInfo */
		int child_frame_count = 0;
		/** the duration of a frame in seconds */
		float frame_time = 0.0f;
	};

	/**
	* Some JSON utility functions
	*/
		
	bool SaveIntoJSON(nlohmann::json & json_entry, BitmapGridAnimationInfo const & src);

	bool LoadFromJSON(nlohmann::json const & json_entry, BitmapGridAnimationInfo & dst);

	bool SaveIntoJSON(nlohmann::json & json_entry, ImageAnimationDescription const & src);

	bool LoadFromJSON(nlohmann::json const & json_entry, ImageAnimationDescription & dst);

}; // namespace chaos

