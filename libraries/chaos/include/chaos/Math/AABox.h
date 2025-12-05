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
		using box_corners_type = typename geometry_type::box_corners_type;
		using box_base_type = typename box_base<DIMENSION, T>;

		/** constructor (empty box) */
		aabox() = default;
		/** copy constructor */
		aabox(aabox const& src) = default;
		/** other constructor */
		aabox(vec_type const& in_position, vec_type const& in_half_size, CenterAndHalfSizeToken):
			position(in_position - in_half_size),
			size(in_half_size + in_half_size){}
		/** other constructor */
		aabox(vec_type const& in_min, vec_type const& in_max, MinAndMaxPointsToken) :
			position(in_min),
			size(in_max - in_min){}
		/** other constructor */
		aabox(vec_type const& in_A, vec_type const& in_B, AnyTwoPointsToken):
			position(glm::min(in_A, in_B)),
			size(glm::abs(in_A - in_B)){}
		/** other constructor */
		aabox(vec_type const& in_min, vec_type const& in_size, MinCornerAndSizeToken = {}) :
			position(in_min),
			size(in_size){}
		/** conversion constructor */
		aabox(box_base_type const& src): 
			aabox(src.position, src.half_size, CenterAndHalfSize){}
		/** construct a box from the corners */
		aabox(box_corners_type const& corners):
			aabox(corners.min, corners.max, MinAndMaxPoints){}

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
		if (IsGeometryEmpty(b1))
		{
			if (IsGeometryEmpty(b2))
				return true;
		}
		else if (IsGeometryEmpty(b2))
		{
			return false;
		}
		return (b1.position == b1.position) && (b1.size == b2.size);
	}

	/** returns true whether the box is empty */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool IsGeometryEmpty(aabox<DIMENSION, T> const& b)
	{
		using geometry_type = geometry<DIMENSION, T>;

		return glm::any(glm::lessThan(b.size, geometry_type::vector_zero)); // a zero size box is valid !
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

	/** intersection of 2 boxes */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	aabox<DIMENSION, T> operator & (aabox<DIMENSION, T> const& b1, aabox<DIMENSION, T> const& b2)
	{
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;
		using result_type = typename geometry_type::aabox_type;

		if (IsGeometryEmpty(b1) || IsGeometryEmpty(b2)) // any of the 2 is empty, intersection is empty
			return result_type();

		vec_type B1 = b1.position + b1.size;
		vec_type A2 = b2.position;

		if (glm::any(glm::lessThanEqual(B1, A2)))
			return result_type();

		vec_type A1 = b1.position;
		vec_type B2 = b2.position + b2.size;

		if (glm::any(glm::lessThanEqual(B2, A1)))
			return result_type();

		vec_type A = glm::max(A1, A2);
		vec_type B = glm::min(B1, B2);

		return result(A, B - A);
	}

	/** union of 2 boxes */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	aabox<DIMENSION, T> operator | (aabox<DIMENSION, T> const& b1, aabox<DIMENSION, T> const& b2)
	{
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;
		using result_type = typename geometry_type::aabox_type;

		if (IsGeometryEmpty(b1)) // if one is empty, returns other
			return b2;
		if (IsGeometryEmpty(b2))
			return b1;

		vec_type A = glm::min(b1.position, b2.position);
		vec_type B = glm::max(b1.position + b1.size, b2.position + b2.size);

		return result_type(A, B - A);
	}

	/** increase the box size with a single vertex */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	void ExtendBox(aabox<DIMENSION, T>& b, glm::vec<DIMENSION, T> const& v)
	{
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;
		using box_corners_type = typename geometry_type::box_corners_type;

		if (IsGeometryEmpty(b))
		{
			b.position = v;
			b.size = geometry_type::vector_zero;
		}
		else
		{
			box_corners_type corners = GetBoxCorners(b);
			corners.min = glm::min(corners.min, v);
			corners.max = glm::max(corners.max, v);
			b = aabox<DIMENSION, T>(corners);
		}
	}

	/** returns one of the sub-boxes obtained by splitting the src */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	aabox<DIMENSION, T> GetSplitBox(aabox<DIMENSION, T> const& b, glm::vec<DIMENSION, int> const& p)
	{
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		assert(!glm::any(glm::lessThan(p, geometry::vector_zero)));
		assert(!glm::any(glm::greaterThan(p, geometry::vector_one)));

		vec_type factor = auto_cast_vector(p);
		vec_type new_size = b.size * geometry::vector_half;

		return
		{
			b.position + new_size * factor,
			new_size
		};
	}

	/** save aabox into JSON */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoSaveIntoJSON(nlohmann::json* json, aabox<DIMENSION, T> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "size", src.size);
		return true;
	}

	/** load aabox from JSON */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoLoadFromJSON(JSONReadConfiguration config, aabox<DIMENSION, T>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "size", dst.size);
		return true;
	}

#endif

}; // namespace chaos