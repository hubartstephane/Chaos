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








	/** intersection of 2 boxes */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	aabox<DIMENSION, T> operator & (aabox<DIMENSION, T> const& b1, aabox<DIMENSION, T> const& b2)
	{
#if 0
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		if (IsGeometryEmpty(b1) || IsGeometryEmpty(b2)) // any of the 2 is empty, intersection is empty
			return box<DIMENSION, T>();

		vec_type A1 = b1.position + b1.half_size;
		vec_type B2 = b2.position - b2.half_size;

		if (glm::any(glm::lessThanEqual(A1, B2)))
			return box<DIMENSION, T>();

		vec_type B1 = b1.position - b1.half_size;
		vec_type A2 = b2.position + b2.half_size;

		if (glm::any(glm::lessThanEqual(A2, B1)))
			return box<DIMENSION, T>();

		vec_type A = glm::min(A1, A2);
		vec_type B = glm::max(B1, B2);

		return box<DIMENSION, T>(std::make_pair(A, B));
#endif
		return {};
	}

	/** union of 2 boxes */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	aabox<DIMENSION, T> operator | (aabox<DIMENSION, T> const& b1, aabox<DIMENSION, T> const& b2)
	{
#if 0
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		if (IsGeometryEmpty(b1)) // if one is empty, returns other
			return b2;
		if (IsGeometryEmpty(b2))
			return b1;

		vec_type A1 = b1.position + b1.half_size;
		vec_type A2 = b2.position + b2.half_size;

		vec_type B1 = b1.position - b1.half_size;
		vec_type B2 = b2.position - b2.half_size;

		vec_type A = glm::max(A1, A2);
		vec_type B = glm::min(B1, B2);

		return box<DIMENSION, T>(std::make_pair(A, B));
#endif

		return {};
	}


	/** increase the box size with a single vertex */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	void ExtendBox(aabox<DIMENSION, T>& b, glm::vec<DIMENSION, T> const& v)
	{

#if 0
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		if (IsGeometryEmpty(b))
		{
			b.position = v;
			b.half_size = vec_type(0);
		}
		else
		{
			std::pair<vec_type, vec_type> corners = GetBoxCorners(b);
			corners.first = glm::min(corners.first, v);
			corners.second = glm::max(corners.second, v);
			b = box<DIMENSION, T>(corners);
		}
#endif
	}

	/** returns one of the sub-boxes obtained by splitting the src */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	aabox<DIMENSION, T> GetSplitBox(aabox<DIMENSION, T> const& b, glm::vec<DIMENSION, int> const& p)
	{
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		vec_type v0 = vec_type(T(0));
		vec_type v1 = vec_type(T(1));
		vec_type v2 = vec_type(T(2));

		assert(!glm::any(glm::lessThan(p, v0)));
		assert(!glm::any(glm::greaterThan(p, v1)));

		vec_type factor = auto_cast_vector(p);
		vec_type new_size = b.size / v2;

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