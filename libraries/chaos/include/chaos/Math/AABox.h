 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class aabox;
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(aabox);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * aabox: a box defined by a starting point and a size
	 */

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class aabox : public geometry<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		/** constructor (empty box) */
		aabox() = default;
		/** copy constructor */
		aabox(aabox const& src) = default;
		/** other constructor */
		aabox(vec_type const& in_position, vec_type const& in_size) : position(in_position), size(in_size) {}
		/** construct a box from 2 points */
		aabox(std::pair<vec_type, vec_type> const& pts)
		{
			vec_type a = glm::min(pts.first, pts.second);
			vec_type b = glm::max(pts.first, pts.second);

			this->position = a;
			this->size     = (b - a);
		}

	public:

		/** the lower left corner of the box */
		vec_type position;
		/** the size the box */
		vec_type size = vec_type(-1);
	};

	/** equality function for aabox */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool operator == (aabox<DIMENSION, T> const& b1, aabox<DIMENSION, T> const& b2)
	{
		if (IsGeometryEmpty(b1) != IsGeometryEmpty(b2))
			return false;
		return (b1.position == b1.position) && (b1.size == b2.size);
	}

	/** returns true whether the box is empty */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool IsGeometryEmpty(aabox<DIMENSION, T> const& b)
	{
		return glm::any(glm::lessThan(b.size, aabox<DIMENSION, T>::vec_type(0)));
	}

	/** returns the perimeter of the box */
	template<std::floating_point T>
	T GetPerimeter(aabox<2, T> const& b)
	{
		assert(!IsGeometryEmpty(b));
		return static_cast<T>(2) * (b.size.x + b.size.y);
	}

	/** returns the surface of the box */
	template<std::floating_point T>
	T GetSurface(aabox<2, T> const& b)
	{
		assert(!IsGeometryEmpty(b));
		return (b.size.x * b.size.y);
	}

	/** return the volume of the box */
	template<std::floating_point T>
	T GetVolume(aabox<3, T> const& b)
	{
		return b.size.x * b.size.y * b.size.z;
	}

	/** return the surface of the box */
	template<std::floating_point T>
	T GetSurface(aabox<3, T> const& b)
	{
		assert(!IsGeometryEmpty(b));
		return static_cast<T>(2) * ((b.size.x * b.size.y) + (b.size.y * b.size.z) + (b.size.z * b.size.x));
	};

	/** returns the "aspect" of the box (width/height) */
	template<std::floating_point T>
	T GetBoxAspect(aabox<2, T> const& b)
	{
		assert(!IsGeometryEmpty(b));
		return (b.size.y) ? (b.size.x / b.size.y) : static_cast<T>(1);
	}

	/** update a box aspect */
	template<std::floating_point T>
	aabox<2, T> SetBoxAspect(aabox<2, T> const& src, typename T aspect, SetBoxAspectMethod method)
	{
		// any negative component
		if (IsGeometryEmpty(src))
			return src;

		// shrink method
		if (method == SetBoxAspectMethod::SHRINK_BOX)
		{
			auto effective_aspect = GetBoxAspect(src);
			if (effective_aspect == aspect)
				return src;

			if (effective_aspect > aspect) // width too large
				method = SetBoxAspectMethod::PREFER_UPDATE_WIDTH;
			else if (effective_aspect < aspect) // height too large
				method = SetBoxAspectMethod::PREFER_UPDATE_HEIGHT;
		}
		// other method
		else
		{
			// cannot have no size
			if (src.size.x == 0 && src.size.y == 0)
				return src;

			// if size is 0 along one axis, force/alter this axis
			if (src.size.x == 0)
				method = SetBoxAspectMethod::PREFER_UPDATE_WIDTH;
			else if (src.size.y == 0)
				method = SetBoxAspectMethod::PREFER_UPDATE_HEIGHT;
		}

		// make the update
		assert((method == SetBoxAspectMethod::PREFER_UPDATE_WIDTH) || (method == SetBoxAspectMethod::PREFER_UPDATE_HEIGHT));

		aabox<2, T> result = src;
		if (method == SetBoxAspectMethod::PREFER_UPDATE_WIDTH)
		{
			result.size.x = src.size.y * aspect;
			result.position.x -= (result.size.x - src.size.x) / static_cast<T>(2);
		}
		else if (method == SetBoxAspectMethod::PREFER_UPDATE_HEIGHT)
		{
			result.size.y = src.size.x / aspect;
			result.position.y -= (result.size.y - src.size.y) / static_cast<T>(2);
		}

		return result;
	}

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoSaveIntoJSON(nlohmann::json* json, aabox<DIMENSION, T> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "size", src.size);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoLoadFromJSON(JSONReadConfiguration config, aabox<DIMENSION, T>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "size", dst.size);
		return true;
	}

#endif

}; // namespace chaos