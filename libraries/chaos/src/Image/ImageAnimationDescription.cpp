#include <chaos/Chaos.h>

namespace chaos
{
	// ========================================================================
	// BitmapGridAnimationInfo functions
	// ========================================================================

	bool BitmapGridAnimationInfo::ParseDigitReverse(char const * str, int & start)
	{
		// buffer already consumned
		if (start < 0)
			return false;
		// reverse parsing
		int i = start;
		while (i >= 0 && std::isdigit(str[i]))
			--i;
		// no figure found ?
		if (start == i)
			return false;
		start = i;
		return true;
	}

	bool BitmapGridAnimationInfo::ParseFromNameReadGridX(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result)
	{
		// get a number
		if (!ParseDigitReverse(name, i)) // k should be just before a number
			return false;
		if (i < 1 || name[i] != '_')
			return false;

		if (name_result != nullptr)
			*name_result = std::string(name, i);

		result.grid_size.x = i + 1;

		// compute the result (parsing numbers)
		assert(result.grid_size.x >= 0);
		assert(result.grid_size.y >= 0);

		result.grid_size.x = atoi(name + result.grid_size.x);
		result.grid_size.y = atoi(name + result.grid_size.y);
		if (result.skip_lasts >= 0)
			result.skip_lasts = atoi(name + result.skip_lasts); // XXX : beware, before result.skip_lasts was on index on a std::string
		else
			result.skip_lasts = 0;

		return true;
	}

	bool BitmapGridAnimationInfo::ParseFromNameReadGridY(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result)
	{
		// get a number
		if (!ParseDigitReverse(name, i)) // k should be just before a number
			return false;
		if (name[i] != 'x')
			return false;
		// store the index of the number
		result.grid_size.y = i + 1;
		// try to read x
		return ParseFromNameReadGridX(name, i - 1, result, name_result);
	}

	bool BitmapGridAnimationInfo::ParseFromNameReadGridSkip(char const * name, int i, BitmapGridAnimationInfo & result, std::string * name_result)
	{
		// get a number
		if (!ParseDigitReverse(name, i))
			return false;
		// we were reading Y in fact
		if (name[i] == 'x')
		{
			result.grid_size.y = i + 1;
			return ParseFromNameReadGridX(name, i - 1, result, name_result);
		}
		// is it really a skip ?
		if (name[i] != 's')
			return false;

		result.skip_lasts = i + 1; // XXX : beware, we store an index on a std::string
		return ParseFromNameReadGridY(name, i - 1, result, name_result);
	}

	bool BitmapGridAnimationInfo::ParseFromName(char const * name, BitmapGridAnimationInfo & result, std::string * name_result)
	{
		// hack the structure to hold 'pointer' on the string that contains the values
		BitmapGridAnimationInfo tmp;
		tmp.grid_size.x = -1;
		tmp.grid_size.y = -1;
		tmp.skip_lasts = -1;

		// the format of an animated grid image can be
		//   filename_1x4.png
		//   filename_1x4
		//   filename_1x4_s4.png
		//   filename_1x4_s4
		//
		//   the minimum suffis size for an animated image is 4 characters (_1x4)

		static constexpr int MIN_PREFIX_SIZE = 4;

		// get the length
		int len = (int)strlen(name);
		if (len < 1 + MIN_PREFIX_SIZE) // +1 for at least one character
			return false;

		// remove the extension if any
		int i = len;
		while (i >= 0 && name[i] != '.')
			--i;
		if (name[i] == '.') // prefix found => remove it
		{
			if (i < 1 + MIN_PREFIX_SIZE) // i is the size of the cut name
				return false;
			--i;
		}
		else
		{
			i = len - 1; // take the whole input
		}
		// parsing chain
		if (!ParseFromNameReadGridSkip(name, i, tmp, name_result))
			return false;
		// ensure the result is not empty
		if (tmp.GetFrameCount() > 0)
		{
			result = tmp;
			return true;
		}
		return false;
	}

	int BitmapGridAnimationInfo::GetFrameCount() const
	{
		if (grid_size.x <= 0 || grid_size.y <= 0)
			return 0;
		return std::max(grid_size.x * grid_size.y - skip_lasts, 0);
	}

	// ========================================================================
	// BitmapAnimationInfo functions
	// ========================================================================

	bool ImageAnimationDescription::IsFrameAnimation() const
	{
		return (child_frame_count > 0);
	}

	bool ImageAnimationDescription::IsGridAnimation() const
	{
		if (IsFrameAnimation())
			return false;
		return (grid_data.GetFrameCount() > 0);
	}

	int ImageAnimationDescription::GetFrameCount() const
	{
		if (IsGridAnimation())
			return grid_data.GetFrameCount();
		return child_frame_count + 1; // +1 for the very first image (that is not a child)
	}

	float ImageAnimationDescription::GetAnimationDuration() const
	{
		float frame_count = (float)GetFrameCount();
		return frame_count * GetFrameDuration();
	}

	float ImageAnimationDescription::GetFrameDuration() const
	{
		// frame time is known explicitly
		if (frame_duration > 0.0f)
			return frame_duration;
		// try to deduce the time from the whole animation duration
		float frame_count = (float)GetFrameCount();
		if (frame_count <= 0.0f)
			return 0.0f;
		// get 1.0 second animation duration if not known
		float effective_anim_duration = (anim_duration > 0.0f) ? anim_duration : 1.0f;
		return (effective_anim_duration / frame_count);
	}

	// ========================================================================
	// JSON functions
	// ========================================================================

	bool SaveIntoJSON(nlohmann::json & json, BitmapGridAnimationInfo const & src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "grid_size", src.grid_size);
		JSONTools::SetAttribute(json, "skip_lasts", src.skip_lasts);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const & json, BitmapGridAnimationInfo & dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "grid_size", dst.grid_size);
		JSONTools::GetAttribute(json, "skip_lasts", dst.skip_lasts);
		return true;
	}

	bool SaveIntoJSON(nlohmann::json & json, ImageAnimationDescription const & src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "grid_data", src.grid_data);
		JSONTools::SetAttribute(json, "child_frame_count", src.child_frame_count);
		JSONTools::SetAttribute(json, "frame_duration", src.frame_duration);
		JSONTools::SetAttribute(json, "anim_duration", src.anim_duration);
		JSONTools::SetAttribute(json, "default_wrap_mode", src.default_wrap_mode);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const & json, ImageAnimationDescription & dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "grid_data", dst.grid_data);
		JSONTools::GetAttribute(json, "child_frame_count", dst.child_frame_count);
		JSONTools::GetAttribute(json, "frame_duration", dst.frame_duration);
		JSONTools::GetAttribute(json, "anim_duration", dst.anim_duration);
		JSONTools::GetAttribute(json, "default_wrap_mode", dst.default_wrap_mode);
		return true;
	}

}; // namespace chaos
