 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class box_base;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class box;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class obox;

	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box_base);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(obox);

	enum class SetBoxAspectMethod;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * box_base: base classes for most boxes
	 */

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class box_base : public geometry<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		/** constructor (empty box) */
		box_base() = default;
		/** copy constructor */
		box_base(box_base const& src) = default;
		/** other constructor */
		box_base(vec_type const & in_position, vec_type const & in_half_size) : position(in_position), half_size(in_half_size) {}

	public:

		/** the center of the box */
		vec_type position;
		/** the half size the box */
		vec_type half_size = vec_type(-1);
	};

	/**
	 * box: the simple box, axes aligned to the system axes
	 */

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class box : public box_base<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		/** constructor (empty box) */
		box() = default;
		/** copy constructor */
		box(box const & src) = default;
		/** constructor from base (usefull for obox conversion) */
		box(box_base<DIMENSION, T> const & src) : box_base<DIMENSION, T>(src.position, src.half_size) {}
		/** other constructor */
		box(vec_type const & in_position, vec_type const & in_half_size) : box_base<DIMENSION, T>(in_position, in_half_size) {}
		/** construct a box from 2 points */
		box(std::pair<vec_type, vec_type> const & pts)
		{
			vec_type a = glm::min(pts.first, pts.second);
			vec_type b = glm::max(pts.first, pts.second);

			this->position = (b + a) / static_cast<T>(2);
			this->half_size = (b - a) / static_cast<T>(2);
		}
	};

	/**
	 * obox: a box with any orientation in space
	 */

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class obox : public box_base<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;
		using rot_type = typename geometry_type::rot_type;

		/** constructor (empty box) */
		obox() = default;
		/** copy constructor */
		obox(obox const& src) = default;
		/** constructor from base (usefull for box conversion) */
		obox(box_base<DIMENSION, T> const& src) : box_base<DIMENSION, T>(src.position, src.half_size){}
		/** other constructor */
		obox(vec_type const & in_position, vec_type const & in_half_size, rot_type const & in_rotation) : box_base<DIMENSION, T>(in_position, in_half_size), rotation(in_rotation) {}

	public:

		/** the angle/quaternion of rotation to apply to a box to have this obox */
		rot_type rotation = zero_rotator();
	};

	/** how box must be modified to match wanted aspect */
	enum class SetBoxAspectMethod : int
	{
		SHRINK_BOX,           // shrink the side along the axis that is too long
		PREFER_UPDATE_WIDTH,  // prefere to modify width whenever possible
		PREFER_UPDATE_HEIGHT  // prefere to modify height whenever possible
	};

	/** equality function for box */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool operator == (box<DIMENSION, T> const& b1, box<DIMENSION, T> const& b2)
	{
		if (IsGeometryEmpty(b1) != IsGeometryEmpty(b2))
			return false;
		return (b1.position == b1.position) && (b1.half_size == b2.half_size);
	}

	/** equality function for obox */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool operator == (obox<DIMENSION, T> const& b1, obox<DIMENSION, T> const& b2)
	{
		if (IsGeometryEmpty(b1) != IsGeometryEmpty(b2))
			return false;
		return (b1.position == b1.position) && (b1.half_size == b2.half_size) && (b1.rotation == b2.rotation);
	}

	/** returns true whether the box is empty */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool IsGeometryEmpty(box_base<DIMENSION, T> const& b)
	{
		return glm::any(glm::lessThan(b.half_size, box_base<DIMENSION, T>::vec_type(0)));
	}

	/** returns the perimeter of the box */
	template<std::floating_point T>
	T GetPerimeter(box_base<2, T> const& b)
	{
		assert(!IsGeometryEmpty(b));
		return static_cast<T>(4) * (b.half_size.x + b.half_size.y);
	}

	/** returns the surface of the box */
	template<std::floating_point T>
	T GetSurface(box_base<2, T> const& b)
	{
		assert(!IsGeometryEmpty(b));
		return static_cast<T>(4) * (b.half_size.x * b.half_size.y);
	}

	/** return the volume of the box */
	template<std::floating_point T>
	T GetVolume(box_base<3, T> const& b)
	{
		assert(!IsGeometryEmpty(b));
		return static_cast<T>(8) * b.half_size.x * b.half_size.y * b.half_size.z;
	}

	/** return the surface of the box */
	template<std::floating_point T>
	T GetSurface(box_base<3, T> const& b)
	{
		assert(!IsGeometryEmpty(b));
		return static_cast<T>(8) * ((b.half_size.x * b.half_size.y) + (b.half_size.y * b.half_size.z) + (b.half_size.z * b.half_size.x));
	};

	/** update a box aspect */
	template<typename BOX_TYPE>
	BOX_TYPE SetBoxAspect(BOX_TYPE const& src, typename BOX_TYPE::type aspect, SetBoxAspectMethod method)
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
		// other methods
		else
		{
			// cannot have no size
			if (src.half_size.x == 0 && src.half_size.y == 0)
				return src;

			// if size is 0 along one axis, force/alter this axis
			if (src.half_size.x == 0)
				method = SetBoxAspectMethod::PREFER_UPDATE_WIDTH;
			else if (src.half_size.y == 0)
				method = SetBoxAspectMethod::PREFER_UPDATE_HEIGHT;
		}

		// make the update
		assert((method == SetBoxAspectMethod::PREFER_UPDATE_WIDTH) || (method == SetBoxAspectMethod::PREFER_UPDATE_HEIGHT));

		BOX_TYPE result = src;
		if (method == SetBoxAspectMethod::PREFER_UPDATE_WIDTH)
			result.half_size.x = src.half_size.y * aspect;
		else if (method == SetBoxAspectMethod::PREFER_UPDATE_HEIGHT)
			result.half_size.y = src.half_size.x / aspect;

		return result;
	}

	/** intersection of 2 boxes */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	box<DIMENSION, T> operator & (box<DIMENSION, T> const& b1, box<DIMENSION, T> const& b2)
	{
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
	}

	/** union of 2 boxes */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	box<DIMENSION, T> operator | (box<DIMENSION, T> const& b1, box<DIMENSION, T> const& b2)
	{
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
	}

	/** returns one of the sub-boxes obtained by splitting the src */
	template<std::floating_point T>
	box<2, T> GetSplitBox(box<2, T> const& b, int i, int j)
	{
		using geometry_type = geometry<2, T>;
		using vec_type = typename geometry_type::vec_type;

		assert((i == 0) || (i == 1));
		assert((j == 0) || (j == 1));
		i = (i << 1) - 1;
		j = (j << 1) - 1;
		vec_type new_half_size = b.half_size / static_cast<T>(2);

		return box<2, T>(
			b.position + new_half_size * vec_type(static_cast<T>(i), static_cast<T>(j)),
			new_half_size);
	}

	/** returns one of the sub-boxes obtained by splitting the src */
	template<std::floating_point T>
	box<3, T> GetSplitBox(box<3, T> const& b, int i, int j, int k)
	{
		using geometry_type = geometry<3, T>;
		using vec_type = typename geometry_type::vec_type;

		assert((i == 0) || (i == 1));
		assert((j == 0) || (j == 1));
		assert((k == 0) || (k == 1));
		i = (i << 1) - 1;
		j = (j << 1) - 1;
		k = (k << 1) - 1;
		vec_type new_half_size = b.half_size / static_cast<T>(2);

		return box<3, T>(
			b.position + new_half_size * vec_type(static_cast<T>(i), static_cast<T>(j), static_cast<T>(k)),
			new_half_size);
	}

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoSaveIntoJSON(nlohmann::json* json, box<DIMENSION, T> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "half_size", src.half_size);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoLoadFromJSON(JSONReadConfiguration config, box<DIMENSION, T>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "half_size", dst.half_size);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoSaveIntoJSON(nlohmann::json* json, obox<DIMENSION, T> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "half_size", src.half_size);
		JSONTools::SetAttribute(json, "rotation", src.rotation);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoLoadFromJSON(JSONReadConfiguration config, obox<DIMENSION, T>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "half_size", dst.half_size);
		JSONTools::GetAttribute(config, "rotation", dst.rotation);
		return true;
	}

#endif

}; // namespace chaos