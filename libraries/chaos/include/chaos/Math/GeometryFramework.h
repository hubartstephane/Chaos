#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{

	template<typename VECTOR_TYPE>
	class AutoCastableVector;

	enum class SetBoxAspectMethod;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	// ==============================================================================================
	// rotator functions
	// ==============================================================================================

	/** rotator to matrix */
	CHAOS_API glm::mat4x4 GetRotatorMatrix(glm::quat const& rotator);
	/** rotator to matrix */
	CHAOS_API glm::dmat4x4 GetRotatorMatrix(glm::dquat const& rotator);
	/** rotator to matrix */
	CHAOS_API glm::mat4x4 GetRotatorMatrix(float rotator);
	/** rotator to matrix */
	CHAOS_API glm::dmat4x4 GetRotatorMatrix(double rotator);

	// ==============================================================================================
	// plane functions
	// ==============================================================================================

	template<std::floating_point T, int plane_dimension>
	glm::vec<plane_dimension - 1, T> GetPlaneNormal(glm::vec<plane_dimension, T> const& src)
	{
		return *(glm::vec<plane_dimension - 1, T>*)&src;
	}

	template<std::floating_point T, int plane_dimension>
	T GetPlaneOffset(glm::vec<plane_dimension, T> const& src)
	{
		return src[plane_dimension - 1];
	}

	template<std::floating_point T, int plane_dimension>
	void NormalizePlane(glm::vec<plane_dimension, T> & src)
	{
		 auto normal = GetPlaneNormal(src);
		 T len = glm::length2(normal);
		 if (len != T(0) && len != T(1))
			 src /= std::sqrt(len);
	}

	template<std::floating_point T, int plane_dimension>
	glm::vec<plane_dimension, T> GetNormalizedPlane(glm::vec<plane_dimension, T> const & src)
	{
		auto normal = GetPlaneNormal(src);
		T len = glm::length2(normal);
		if (len != T(0) && len != T(1))
			return src / std::sqrt(len);
		return src;
	}

	// ==============================================================================================
	// GLM vector functions
	// ==============================================================================================

	/** cast a vector into another vector (arity and type may change) */
	template<typename VECTOR_TYPE1, typename VECTOR_TYPE2>
	VECTOR_TYPE1 RecastVector(VECTOR_TYPE2 const& value)
	{
		VECTOR_TYPE1 result(0);
		size_t count = std::min(result.length(), value.length());
		for (size_t i = 0; i < count; ++i)
			result[i] = static_cast<VECTOR_TYPE1::value_type>(value[i]);

		return result;
	}

	template<typename VECTOR_TYPE>
	class AutoCastableVector
	{
	public:

		/** the constructors */
		AutoCastableVector() = default;
		AutoCastableVector(AutoCastableVector const& src) = default;
		AutoCastableVector(VECTOR_TYPE const& in_src) : src(in_src) {}

		/** 'cast' function */
		template<typename VECTOR_DST_TYPE>
		operator VECTOR_DST_TYPE () const
		{
			VECTOR_DST_TYPE result(0);
			size_t count = std::min(result.length(), src.length());
			for (size_t i = 0; i < count; ++i)
				result[i] = static_cast<VECTOR_DST_TYPE::value_type>(src[i]);
			return result;
		}

	protected:

		/** the src vector that will be 'casted' on demand */
		VECTOR_TYPE src;
	};

	/** create a delayed 'cast' */
	template<typename T>
	AutoCastableVector<T> auto_cast_vector(AutoCastableVector<T> const& src) { return src; }

	/** create a delayed 'cast' */
	template<typename T>
	AutoCastableVector<T> auto_cast_vector(T const& src) { return src; }

	// ==============================================================================================
	// type_box_base functions
	// ==============================================================================================

	// shu46 : half_size.x == 0  -> pas vide


	/** returns true whether the box is empty */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool IsGeometryEmpty(type_box_base<T, dimension> const& b)
	{
		return glm::any(glm::lessThan(b.half_size, type_box_base<T, dimension>::vec_type(0)));
	}

	/** returns the perimeter of the box */
	template<std::floating_point T>
	T GetPerimeter(type_box_base<T, 2> const& b)
	{
		return static_cast<T>(4) * (b.half_size.x + b.half_size.y);
	}

	/** returns the surface of the box */
	template<std::floating_point T>
	T GetSurface(type_box_base<T, 2> const& b)
	{
		return static_cast<T>(4) * (b.half_size.x * b.half_size.y);
	}

	/** return the volume of the box */
	template<std::floating_point T>
	T GetVolume(type_box_base<T, 3> const& b)
	{
		return static_cast<T>(8) * b.half_size.x * b.half_size.y * b.half_size.z;
	}

	/** return the surface of the box */
	template<std::floating_point T>
	T GetSurface(type_box_base<T, 3> const& b)
	{
		return static_cast<T>(8) * ((b.half_size.x * b.half_size.y) + (b.half_size.y * b.half_size.z) + (b.half_size.z * b.half_size.x));
	};

	/** returns the bounding circle for the box */
	template<std::floating_point T>
	type_sphere<T, 2> GetBoundingSphere(type_box_base<T, 2> const& b)
	{
		return IsGeometryEmpty(b) ? type_sphere<T, 2>() : type_sphere<T, 2>(b.position, glm::length(b.half_size));
	}

	/** returns the inner circle for the box */
	template<std::floating_point T>
	type_sphere<T, 2> GetInnerSphere(type_box_base<T, 2> const& b)
	{
		return IsGeometryEmpty(b) ? type_sphere<T, 2>() : type_sphere<T, 2>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the bounding sphere for the box */
	template<std::floating_point T>
	type_sphere<T, 3> GetBoundingSphere(type_box_base<T, 3> const& b)
	{
		return IsGeometryEmpty(b) ? type_sphere<T, 3>() : type_sphere<T, 3>(b.position, glm::length(b.half_size));
	}

	/** returns the inner sphere for the box */
	template<std::floating_point T>
	type_sphere<T, 3> GetInnerSphere(type_box_base<T, 3> const& b)
	{
		return IsGeometryEmpty(b) ? type_sphere<T, 3>() : type_sphere<T, 3>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the "aspect" of the box (width/height) */
	template<std::floating_point T>
	T GetBoxAspect(type_box_base<T, 2> const& b)
	{
		return (b.half_size.y) ? (b.half_size.x / b.half_size.y) : static_cast<T>(1);
	}

	/** how box must be modified to match wanted aspect*/
	enum class SetBoxAspectMethod : int
	{
		SHRINK_BOX,           // shrink the side along the axis that is too long
		PREFER_UPDATE_WIDTH, // prefere to modify width whenever possible
		PREFER_UPDATE_HEIGHT // prefere to modify height whenever possible
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

	// ==============================================================================================
	// box functions
	// ==============================================================================================

	/** equality function for box */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool operator == (type_box<T, dimension> const& b1, type_box<T, dimension> const& b2)
	{
		return (b1.position == b1.position) && (b1.half_size == b2.half_size);
	}

	/** intersection of 2 boxes */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	type_box<T, dimension> operator & (type_box<T, dimension> const& b1, type_box<T, dimension> const& b2)
	{
		using vec_type = typename type_box<T, dimension>::vec_type;

		if (IsGeometryEmpty(b1) || IsGeometryEmpty(b2)) // any of the 2 is empty, intersection is empty
			return type_box<T, dimension>();

		vec_type A1 = b1.position + b1.half_size;
		vec_type B2 = b2.position - b2.half_size;

		if (glm::any(glm::lessThanEqual(A1, B2)))
			return type_box<T, dimension>();

		vec_type B1 = b1.position - b1.half_size;
		vec_type A2 = b2.position + b2.half_size;

		if (glm::any(glm::lessThanEqual(A2, B1)))
			return type_box<T, dimension>();

		vec_type A = glm::min(A1, A2);
		vec_type B = glm::max(B1, B2);

		return type_box<T, dimension>(std::make_pair(A, B));
	}

	/** union of 2 boxes */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	type_box<T, dimension> operator | (type_box<T, dimension> const& b1, type_box<T, dimension> const& b2)
	{
		using vec_type = typename type_box<T, dimension>::vec_type;

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

		return type_box<T, dimension>(std::make_pair(A, B));
	}

	/** returns one of the sub-boxes obtained by splitting the src */
	template<std::floating_point T>
	type_box<T, 2> GetSplitBox(type_box<T, 2> const& b, int i, int j)
	{
		using vec_type = typename type_box<T, 2>::vec_type;

		assert((i == 0) || (i == 1));
		assert((j == 0) || (j == 1));
		i = (i << 1) - 1;
		j = (j << 1) - 1;
		vec_type new_half_size = b.half_size / static_cast<T>(2);

		return type_box<T, 2>(
			b.position + new_half_size * vec_type(static_cast<T>(i), static_cast<T>(j)),
			new_half_size);
	}

	/** returns one of the sub-boxes obtained by splitting the src */
	template<std::floating_point T>
	type_box<T, 3> GetSplitBox(type_box<T, 3> const& b, int i, int j, int k)
	{
		using vec_type = typename type_box<T, 3>::vec_type;

		assert((i == 0) || (i == 1));
		assert((j == 0) || (j == 1));
		assert((k == 0) || (k == 1));
		i = (i << 1) - 1;
		j = (j << 1) - 1;
		k = (k << 1) - 1;
		vec_type new_half_size = b.half_size / static_cast<T>(2);

		return type_box<T, 3>(
			b.position + new_half_size * vec_type(static_cast<T>(i), static_cast<T>(j), static_cast<T>(k)),
			new_half_size);
	}

	/** get the corners of the box */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	auto GetBoxCorners(type_box<T, dimension> const& b) // returns a std::pair<vec_type, vec_type>
	{
		if (!IsGeometryEmpty(b))
			return std::make_pair(b.position - b.half_size, b.position + b.half_size);
		return std::make_pair(b.position, b.position);
	}

	/** increase the box size with a single vertex */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	void ExtendBox(type_box<T, dimension>& b, typename type_box<T, dimension>::vec_type const& v)
	{
		using vec_type = typename type_box<T, dimension>::vec_type;

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
			b = type_box<T, dimension>(corners);
		}
	}

	template<std::floating_point T>
	auto GetBoxVertices(type_box_base<T, 2> const& b, typename type_box_base<T, 2>::vec_type* result, bool global = true) // expect an array of 4 elements
	{
		assert(result != nullptr);

		using vec_type = typename type_box_base<T, 2>::vec_type;

		T NX = -b.half_size.x;
		T PX = +b.half_size.x;

		T NY = -b.half_size.y;
		T PY = +b.half_size.y;

		result[0] = vec_type(NX, NY);
		result[1] = vec_type(PX, NY);
		result[2] = vec_type(NX, PY);
		result[3] = vec_type(PX, PY);

		if (global)
			for (int i = 0; i < 4; ++i)
				result[i] += b.position;

		return result;
	}

	template<std::floating_point T>
	auto GetBoxVertices(type_box_base<T, 3> const& b, typename type_box_base<T, 3>::vec_type* result, bool global = true) // expect an array of 8 elements
	{
		assert(result != nullptr);

		using vec_type = typename type_box_base<T, 3>::vec_type;

		T NX = -b.half_size.x;
		T PX = +b.half_size.x;

		T NY = -b.half_size.y;
		T PY = +b.half_size.y;

		T NZ = -b.half_size.z;
		T PZ = +b.half_size.z;

		result[0] = vec_type(NX, NY, NZ);
		result[1] = vec_type(PX, NY, NZ);
		result[2] = vec_type(NX, PY, NZ);
		result[3] = vec_type(PX, PY, NZ);
		result[4] = vec_type(NX, NY, PZ);
		result[5] = vec_type(PX, NY, PZ);
		result[6] = vec_type(NX, PY, PZ);
		result[7] = vec_type(PX, PY, PZ);

		if (global)
			for (int i = 0; i < 8; ++i)
				result[i] += b.position;

		return result;
	}

	/** encode a box2 into a vector4 */
	CHAOS_API glm::vec4 EncodeBoxToVector(box2 const& src);

	/** get planes from a box */
	template<std::floating_point T>
	type_box_plane<T, 2> GetBoxPlanes(type_box_base<T, 2> const& b)
	{
		type_box_plane<T, 2> result;
		result.neg_x = { T(-1),  T(0), -b.half_size.x};
		result.pos_x = { T(+1),  T(0), -b.half_size.x };
		result.neg_y = {  T(0), T(-1), -b.half_size.y};
		result.pos_y = {  T(0), T(+1), -b.half_size.y };
		return result;
	}
	/** get planes from a box */
	template<std::floating_point T>
	type_box_plane<T, 3> GetBoxPlanes(type_box_base<T, 3> const& b)
	{
		type_box_plane<T, 3> result;
		result.neg_x = { T(-1),  T(0),  T(0), -b.half_size.x};
		result.pos_x = { T(+1),  T(0),  T(0), -b.half_size.x };
		result.neg_y = {  T(0), T(-1),  T(0), -b.half_size.y };
		result.pos_y = {  T(0), T(+1),  T(0), -b.half_size.y };
		result.neg_z = {  T(0),  T(0), T(-1), -b.half_size.z };
		result.pos_z = {  T(0),  T(0), T(+1), -b.half_size.z };
		return result;
	}

	// ==============================================================================================
	// obox functions
	// ==============================================================================================

	/** equality function for obox */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool operator == (type_obox<T, dimension> const& b1, type_obox<T, dimension> const& b2)
	{
		return (b1.position == b1.position) && (b1.half_size == b2.half_size) && (b1.rotator == b2.rotator);
	}

	// shu49. Ca vaut le coup de savoir dans quel ordre on genere ces vertex

	//  0    1                    0    1
	//                    ou
	//  2    3                    3    2
	//
	// histoire de faire du rendu plus facilement

	template<std::floating_point T>
	auto GetBoxVertices(type_obox<T, 2> const& b, typename type_box_base<T, 2>::vec_type* result, bool global = true) // expect an array of 4 elements
	{
		GetBoxVertices((type_box_base<T, 2> const&)b, result, false); // do not integrate translation because this would produce a wrong rotation/translation combinaison

		if (global)
		{
			auto transform = GetRotatorMatrix(b.rotator);
			for (int i = 0; i < 4; ++i)
			{
				result[i] = GLMTools::Mult(transform, result[i]);
				result[i] += b.position;
			}
		}
		return result;
	}

	template<std::floating_point T>
	auto GetBoxVertices(type_obox<T, 3> const& b, typename type_box_base<T, 3>::vec_type* result, bool global = true) // expect an array of 8 elements
	{
		GetBoxVertices((type_box_base<T, 3> const&)b, result, false); // do not integrate translation because this would produce a wrong rotation/translation combinaison

		if (global)
		{
			auto transform = GetRotatorMatrix(b.rotator);
			for (int i = 0; i < 8; ++i)
			{
				result[i] = GLMTools::Mult(transform, result[i]);
				result[i] += b.position;
			}
		}
	}

	template<std::floating_point T>
	type_box<T, 2> GetBoundingBox(type_obox<T, 2> const& b)
	{
		type_box<T, 2> result;

		// copy position
		result.position = b.position;
		// get all vertices and compute max values
		typename type_box<T, 2>::vec_type vertices[4];
		GetBoxVertices(b, vertices, false);

		auto max_vec = glm::tvec2<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max());

		auto transform = GetRotatorMatrix(b.rotator);
		for (int i = 0; i < 4; ++i)
			max_vec = glm::max(max_vec, GLMTools::Mult(transform, vertices[i]));
		result.half_size = max_vec;

		return result;
	}

	template<std::floating_point T>
	type_box<T, 3> GetBoundingBox(type_obox<T, 3> const& b)
	{
		type_box<T, 3> result;

		// copy position
		result.position = b.position;

		// get all vertices and compute max values
		typename type_box<T, 3>::vec_type vertices[8];
		GetBoxVertices(b, vertices, false);

		auto max_vec = glm::tvec3<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), -std::numeric_limits<T>::max());

		auto transform = GetRotatorMatrix(b.rotator);
		for (int i = 0; i < 8; ++i)
			max_vec = glm::max(max_vec, GLMTools::Mult(transform, vertices[i]));
		result.half_size = max_vec;

		return result;
	}

	// ==============================================================================================
	// aabox functions
	// ==============================================================================================

	/** equality function for obox */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool operator == (type_aabox<T, dimension> const& b1, type_aabox<T, dimension> const& b2)
	{
		return (b1.position == b1.position) && (b1.size == b2.size);
	}

	/** returns true whether the box is empty */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool IsGeometryEmpty(type_aabox<T, dimension> const& b)
	{
		return glm::any(glm::lessThan(b.size, type_aabox<T, dimension>::vec_type(0)));
	}

	/** returns the perimeter of the box */
	template<std::floating_point T>
	T GetPerimeter(type_aabox<T, 2> const& b)
	{
		return static_cast<T>(2) * (b.size.x + b.size.y);
	}

	/** returns the surface of the box */
	template<std::floating_point T>
	T GetSurface(type_aabox<T, 2> const& b)
	{
		return (b.size.x * b.size.y);
	}

	/** return the volume of the box */
	template<std::floating_point T>
	T GetVolume(type_aabox<T, 3> const& b)
	{
		return b.size.x * b.size.y * b.size.z;
	}

	/** return the surface of the box */
	template<std::floating_point T>
	T GetSurface(type_aabox<T, 3> const& b)
	{
		return static_cast<T>(2) * ((b.size.x * b.size.y) + (b.size.y * b.size.z) + (b.size.z * b.size.x));
	};

	/** returns the "aspect" of the box (width/height) */
	template<std::floating_point T>
	T GetBoxAspect(type_aabox<T, 2> const& b)
	{
		return (b.size.y) ? (b.size.x / b.size.y) : static_cast<T>(1);
	}


	/** update a box aspect */
	template<std::floating_point T>
	type_aabox<T, 2> SetBoxAspect(type_aabox<T, 2> const& src, typename T aspect, SetBoxAspectMethod method)
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

		type_aabox<T, 2> result = src;
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

	// ==============================================================================================
	// triangles functions
	// ==============================================================================================

	/** equality test function for triangles */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool operator == (type_triangle<T, dimension> const& t1, type_triangle<T, dimension> const& t2)
	{
		if (t1.a == t2.a)
		{
			if (t1.b == t2.b && t1.c == t2.c)
				return true;
			if (t1.c == t2.b && t1.b == t2.c)
				return true;
		}
		if (t1.a == t2.b)
		{
			if (t1.b == t2.a && t1.c == t2.c)
				return true;
			if (t1.c == t2.a && t1.b == t2.c)
				return true;
		}
		if (t1.a == t2.c)
		{
			if (t1.b == t2.a && t1.c == t2.b)
				return true;
			if (t1.c == t2.a && t1.b == t2.b)
				return true;
		}
		return false;
	}

	/** returns true whether the triangle is empty */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool IsGeometryEmpty(type_triangle<T, dimension> const& t)
	{
		if (t.a == t.b || t.a == t.c || t.b == t.c)
			return true;
		return false;
	}

	/** get the reversed triangle */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	type_triangle<T, dimension> GetInvertedTriangle(type_triangle<T, dimension> const& t)
	{
		return type_triangle<T, dimension>(t.a, t.c, t.b);
	}

	// ==============================================================================================
	// ray functions
	// ==============================================================================================

	/** equality function for ray */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool operator == (type_ray<T, dimension> const& r1, type_ray<T, dimension> const& r2)
	{
		return (r1.position == r2.position) && (r1.direction == r2.direction);
	}

	// ==============================================================================================
	// sphere/circle functions
	// ==============================================================================================

	/** returns true whether the circle is empty */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool IsGeometryEmpty(type_sphere<T, dimension> const& c)
	{
		return (c.radius < 0);
	}

	/** equality function for circle */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool operator == (type_sphere<T, dimension> const& c1, type_sphere<T, dimension> const& c2)
	{
		return (c1.position == c2.position) && (c1.radius == c2.radius);
	}

	/** returns the perimeter of the circle */
	template<std::floating_point T>
	T GetPerimeter(type_sphere<T, 2> const& c)
	{
		return static_cast<T>(2.0 * M_PI) * c.radius;
	}

	/** returns the surface of the circle */
	template<std::floating_point T>
	T GetSurface(type_sphere<T, 2> const& c)
	{
		return static_cast<T>(M_PI) * c.radius * c.radius;
	}

	/** returns the volume of the sphere */
	template<std::floating_point T>
	T GetVolume(type_sphere<T, 3> const& s)
	{
		return static_cast<T>((4.0 / 3.0) * M_PI) * s.radius * s.radius * s.radius;
	}

	/** returns the surface of the sphere */
	template<std::floating_point T>
	T GetSurface(type_sphere<T, 3> const& s)
	{
		return static_cast<T>(4.0 * M_PI) * s.radius * s.radius;
	}

	/** returns the bounding box of the circle */
	template<std::floating_point T>
	type_box<T, 2> GetBoundingBox(type_sphere<T, 2> const& c)
	{
		using vec_type = typename type_sphere<T, 2>::vec_type;

		return IsGeometryEmpty(c) ? type_box<T, 2>() : type_box<T, 2>(c.position, vec_type(c.radius, c.radius));
	}

	/** returns the bounding box of the circle (square) */
	template<std::floating_point T>
	type_box<T, 2> GetInnerBox(type_sphere<T, 2> const& c)
	{
		using vec_type = typename type_sphere<T, 2>::vec_type;

		static double const INV_SQRT2 = 0.707106781186547; /* 1.0 / sqrtf(2.0) */

		return IsGeometryEmpty(c) ? type_box<T, 2>() : type_box<T, 2>(c.position, vec_type(c.radius * static_cast<T>(INV_SQRT2)));
	}

	template<std::floating_point T>
	type_box<T, 3> GetBoundingBox(type_sphere<T, 3> const& s)
	{
		using vec_type = typename type_sphere<T, 3>::vec_type;

		return IsGeometryEmpty(s) ? type_box<T, 3>() : type_box<T, 3>(s.position, vec_type(s.radius));
	}

	template<std::floating_point T>
	type_box<T, 3> GetInnerBox(type_sphere<T, 3> const& s)
	{
		using vec_type = typename type_sphere<T, 3>::vec_type;

		static double const INV_SQRT3 = 0.577350269189625; /* 1.0 / sqrtf(3.0) */

		return IsGeometryEmpty(s) ? type_box<T, 3>() : type_box<T, 3>(s.position, vec_type(s.radius * static_cast<T>(INV_SQRT3)));
	}

	/** returns intersection of 2 spheres */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	type_sphere<T, dimension> operator & (type_sphere<T, dimension> const& s1, type_sphere<T, dimension> const& s2) // intersection
	{
		using vec_type = typename type_sphere<T, dimension>::vec_type;

		if (IsGeometryEmpty(s1) || IsGeometryEmpty(s2))
			return type_sphere<T, dimension>();
		if (s1.position == s2.position)
			return type_sphere<T, dimension>(s1.position, glm::min(s1.radius, s2.radius));

		vec_type delta_pos = s2.position - s1.position;   // vector that go from center 1 to center 2
		T        distance = glm::length(delta_pos);      // length of such a vector

		if (distance >= s1.radius + s2.radius) // sphere too far => returns the empty sphere
			return type_sphere<T, dimension>();

		T t1 = s1.radius / distance;  // positive
		T t2 = s2.radius / distance;  // positive

		T a = glm::max(-t1, static_cast<T>(1) - t2);
		T b = glm::min(t1, static_cast<T>(1) + t2);

		return type_sphere<T, dimension>(
			s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
			((b - a) / static_cast<T>(2)) * distance);
	}

	/** returns union of 2 spheres */
	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	type_sphere<T, dimension> operator | (type_sphere<T, dimension> const& s1, type_sphere<T, dimension> const& s2) // union
	{
		using vec_type = typename type_sphere<T, dimension>::vec_type;

		if (IsGeometryEmpty(s1))
			return s2;
		if (IsGeometryEmpty(s2))
			return s1;
		if (s1.position == s2.position)
			return type_sphere<T, dimension>(s1.position, glm::max(s1.radius, s2.radius));

		vec_type delta_pos = s2.position - s1.position;    // vector that go from center 1 to center 2
		T        distance = glm::length(delta_pos);       // length of such a vector

		T t1 = s1.radius / distance;  // positive
		T t2 = s2.radius / distance;  // positive

		T a = glm::min(-t1, static_cast<T>(1) - t2);
		T b = glm::max(t1, static_cast<T>(1) + t2);

		return type_sphere<T, dimension>(
			s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
			((b - a) / static_cast<T>(2)) * distance);
	}

	// ==============================================================================================
	// JSON functions
	// ==============================================================================================

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoSaveIntoJSON(nlohmann::json* json, type_box<T, dimension> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "half_size", src.half_size);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoLoadFromJSON(JSONReadConfiguration config, type_box<T, dimension>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "half_size", dst.half_size);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoSaveIntoJSON(nlohmann::json* json, type_obox<T, dimension> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "half_size", src.half_size);
		JSONTools::SetAttribute(json, "rotation", src.rotation);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoLoadFromJSON(JSONReadConfiguration config, type_obox<T, dimension>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "half_size", dst.half_size);
		JSONTools::GetAttribute(config, "rotation", dst.rotation);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoSaveIntoJSON(nlohmann::json* json, type_aabox<T, dimension> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "size", src.size);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoLoadFromJSON(JSONReadConfiguration config, type_aabox<T, dimension>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "size", dst.size);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoSaveIntoJSON(nlohmann::json* json, type_sphere<T, dimension> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "radius", src.radius);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoLoadFromJSON(JSONReadConfiguration config, type_sphere<T, dimension>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "radius", dst.radius);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoSaveIntoJSON(nlohmann::json* json, type_ray<T, dimension> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "direction", src.direction);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(T , dimension)
	bool DoLoadFromJSON(JSONReadConfiguration config, type_ray<T, dimension>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "direction", dst.direction);
		return true;
	}

}; // namespace chaos

	// XXX: for namespace resolution in JSONTools::LoadFromJSON(...) and JSONTools::SaveIntoJSON(...),
	//      it is important that the following functions belong to the same namespace than the target (glm)

namespace glm
{
	template<int SIZE, typename T, glm::precision P>
	void DrawImGuiVariableImpl(glm::vec<SIZE, T, P>& value, chaos::DrawImGuiVariableFlags flags = chaos::DrawImGuiVariableFlags::NONE)
	{
		// ImGui does not provide a InputDoubleX method, that's why we are using InputScalarN here instead of (InputIntX and InputFloatX)
		// (InputScalarN is easier because the size can be given has an argument)	
		if constexpr (std::is_same_v<T, int>)
			ImGui::InputScalarN("", ImGuiDataType_S32, &value, SIZE, NULL, NULL, "%d", 0);
		else if constexpr (std::is_same_v<T, float>)
		{
			if (HasAnyFlags(flags, chaos::DrawImGuiVariableFlags::COLOR)) // special case for colors
			{
				if (SIZE == 3)
				{
					ImGui::ColorEdit3("", &value.x, 0);
					return;
				}
				if (SIZE == 4)
				{
					ImGui::ColorEdit4("", &value.x, 0);
					return;
				}
			}
			ImGui::InputScalarN("", ImGuiDataType_Float, &value, SIZE, NULL, NULL, "%f", 0);
		}		
		else if constexpr (std::is_same_v<T, double>)
			ImGui::InputScalarN("", ImGuiDataType_Double, &value, SIZE, NULL, NULL, "%lf", 0);
	}

	template<typename T, glm::precision P>
	bool DoSaveIntoJSON(nlohmann::json* json, glm::tvec2<T, P> const& src)
	{
		if (!chaos::PrepareSaveArrayIntoJSON(json))
			return false;
		chaos::JSONTools::SetElement(json, 0, src.x);
		chaos::JSONTools::SetElement(json, 1, src.y);
		return true;
	}

	template<typename T, glm::precision P>
	bool DoLoadFromJSON(chaos::JSONReadConfiguration config, glm::tvec2<T, P>& dst)
	{
		return chaos::JSONTools::ForEachSource(config, [&dst](nlohmann::json const* json)
		{
			if (json->is_object())
			{
				chaos::JSONTools::GetAttribute(json, "x", dst.x);
				chaos::JSONTools::GetAttribute(json, "y", dst.y);
				return true;
			}
			else if (json->is_array())
			{
				size_t count = std::min(json->size(), (size_t)dst.length());
				for (size_t i = 0; i < count; ++i)
					dst[i] = json->operator[](i).get<T>();
				return true;
			}
			return false;
		});
	}

	template<typename T, glm::precision P>
	bool DoSaveIntoJSON(nlohmann::json* json, glm::tvec3<T, P> const& src)
	{
		if (!chaos::PrepareSaveArrayIntoJSON(json))
			return false;
		chaos::JSONTools::SetElement(json, 0, src.x);
		chaos::JSONTools::SetElement(json, 1, src.y);
		chaos::JSONTools::SetElement(json, 2, src.z);
		return true;
	}

	template<typename T, glm::precision P>
	bool DoLoadFromJSON(chaos::JSONReadConfiguration config, glm::tvec3<T, P>& dst)
	{
		return chaos::JSONTools::ForEachSource(config, [&dst](nlohmann::json const* json)
		{
			if (json->is_object())
			{
				chaos::JSONTools::GetAttribute(json, "x", dst.x);
				chaos::JSONTools::GetAttribute(json, "y", dst.y);
				chaos::JSONTools::GetAttribute(json, "z", dst.z);
				return true;
			}
			else if (json->is_array())
			{
				size_t count = std::min(json->size(), (size_t)dst.length());
				for (size_t i = 0; i < count; ++i)
					dst[i] = json->operator[](i).get<T>();
				return true;
			}
			return false;
		});
	}

	template<typename T, glm::precision P>
	bool DoSaveIntoJSON(nlohmann::json* json, glm::tvec4<T, P> const& src)
	{
		if (!chaos::PrepareSaveArrayIntoJSON(json))
			return false;
		chaos::JSONTools::SetElement(json, 0, src.x);
		chaos::JSONTools::SetElement(json, 1, src.y);
		chaos::JSONTools::SetElement(json, 2, src.z);
		chaos::JSONTools::SetElement(json, 3, src.w);
		return true;
	}

	template<typename T, glm::precision P>
	bool DoLoadFromJSON(chaos::JSONReadConfiguration config, glm::tvec4<T, P>& dst)
	{
		return chaos::JSONTools::ForEachSource(config, [&dst](nlohmann::json const* json)
		{
			if (json->is_object())
			{
				chaos::JSONTools::GetAttribute(json, "x", dst.x);
				chaos::JSONTools::GetAttribute(json, "y", dst.y);
				chaos::JSONTools::GetAttribute(json, "z", dst.z);
				chaos::JSONTools::GetAttribute(json, "w", dst.w);
				return true;
			}
			else if (json->is_array())
			{
				size_t count = std::min(json->size(), (size_t)dst.length());
				for (size_t i = 0; i < count; ++i)
					dst[i] = json->operator[](i).get<T>();
				return true;
			}
			return false;
		});
	}

}; // namespace glm

#endif