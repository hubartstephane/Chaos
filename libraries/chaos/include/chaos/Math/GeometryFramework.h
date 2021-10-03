namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename VECTOR_TYPE>
	class AutoCastableVector;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==============================================================================================
	// rotator functions
	// ==============================================================================================

	/** rotator to matrix */
	glm::mat4x4 GetRotatorMatrix(glm::quat const& rotator);
	/** rotator to matrix */
	glm::dmat4x4 GetRotatorMatrix(glm::dquat const& rotator);
	/** rotator to matrix */
	glm::mat4x4 GetRotatorMatrix(float rotator);
	/** rotator to matrix */
	glm::dmat4x4 GetRotatorMatrix(double rotator);

	// ==============================================================================================
	// plane functions
	// ==============================================================================================

	glm::vec2  GetPlaneNormal(glm::vec3  const& p);
	glm::vec3  GetPlaneNormal(glm::vec4  const& p);
	glm::dvec2 GetPlaneNormal(glm::dvec3 const& p);
	glm::dvec3 GetPlaneNormal(glm::dvec4 const& p);

	float  GetPlaneOffset(glm::vec3  const& p);
	float  GetPlaneOffset(glm::vec4  const& p);
	double GetPlaneOffset(glm::dvec3 const& p);
	double GetPlaneOffset(glm::dvec4 const& p);

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
	template<typename T, int dimension>
	bool IsGeometryEmpty(type_box_base<T, dimension> const& b)
	{
		return glm::any(glm::lessThan(b.half_size, type_box_base<T, dimension>::vec_type(0)));
	}

	/** returns the perimeter of the box */
	template<typename T>
	T GetPerimeter(type_box_base<T, 2> const& b)
	{
		return static_cast<T>(4) * (b.half_size.x + b.half_size.y);
	}

	/** returns the surface of the box */
	template<typename T>
	T GetSurface(type_box_base<T, 2> const& b)
	{
		return static_cast<T>(4) * (b.half_size.x * b.half_size.y);
	}

	/** return the volume of the box */
	template<typename T>
	T GetVolume(type_box_base<T, 3> const& b)
	{
		return static_cast<T>(8) * b.half_size.x * b.half_size.y * b.half_size.z;
	}

	/** return the surface of the box */
	template<typename T>
	T GetSurface(type_box_base<T, 3> const& b)
	{
		return static_cast<T>(8) * ((b.half_size.x * b.half_size.y) + (b.half_size.y * b.half_size.z) + (b.half_size.z * b.half_size.x));
	};

	/** returns the bounding circle for the box */
	template<typename T>
	type_sphere<T, 2> GetBoundingSphere(type_box_base<T, 2> const& b)
	{
		return IsGeometryEmpty(b) ? type_sphere<T, 2>() : type_sphere<T, 2>(b.position, glm::length(b.half_size));
	}

	/** returns the inner circle for the box */
	template<typename T>
	type_sphere<T, 2> GetInnerSphere(type_box_base<T, 2> const& b)
	{
		return IsGeometryEmpty(b) ? type_sphere<T, 2>() : type_sphere<T, 2>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the bounding sphere for the box */
	template<typename T>
	type_sphere<T, 3> GetBoundingSphere(type_box_base<T, 3> const& b)
	{
		return IsGeometryEmpty(b) ? type_sphere<T, 3>() : type_sphere<T, 3>(b.position, glm::length(b.half_size));
	}

	/** returns the inner sphere for the box */
	template<typename T>
	type_sphere<T, 3> GetInnerSphere(type_box_base<T, 3> const& b)
	{
		return IsGeometryEmpty(b) ? type_sphere<T, 3>() : type_sphere<T, 3>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the "aspect" of the box (width/height) */
	template<typename T>
	T GetBoxAspect(type_box_base<T, 2> const& b)
	{
		return (b.half_size.y) ? (b.half_size.x / b.half_size.y) : static_cast<T>(1);
	}

	/** reduce a rectangle with an aspect (we do not use type_box_base<> because we want the result to be the exact type of the entry) */
	template<typename BOX_TYPE>
	BOX_TYPE ShrinkBoxToAspect(BOX_TYPE const& src, typename BOX_TYPE::type aspect)
	{
		// any negative component
		if (IsGeometryEmpty(src))
			return src;
		// aspect already good
		auto effective_aspect = GetBoxAspect(src);
		if (effective_aspect == aspect)
			return src;
		// make the update
		BOX_TYPE result = src;
		if (effective_aspect > aspect) // width too large
			result.half_size.x = src.half_size.y * aspect;
		else if (effective_aspect < aspect) // height too large
			result.half_size.y = src.half_size.x / aspect;

		return result;
	}

	/** transform rectangle to have desire aspect (if a component is 0, change it, otherwise take more prioritize aspect) */
	template<typename BOX_TYPE>
	BOX_TYPE AlterBoxToAspect(BOX_TYPE const& src, typename BOX_TYPE::type aspect, bool prefere_update_width)
	{
		using type = typename BOX_TYPE::type;

		// any (non null) negative component
		if (IsGeometryEmpty(src))
			return src;
		// cannot have no size
		if (src.half_size.x == (type)0 && src.half_size.y == (type)0)
			return src;
		// width axis to update ?
		if (src.half_size.x == (type)0)
			prefere_update_width = true;
		else if (src.half_size.y == (type)0)
			prefere_update_width = false;
		// make the update
		BOX_TYPE result = src;
		if (prefere_update_width)
			result.half_size.x = src.half_size.y * aspect;
		else
			result.half_size.y = src.half_size.x / aspect;

		return result;
	}

	// shuvvf

	// compute the final size of the particle
	//
	// XXX: explanation of 'particle_size' member usage
	//      -if .x AND .y are 0     => use the particle size in the atlas
	//      -if .x AND .y are not 0 => override particle size in the atlas
	//      -if .x OR  .y is  0     => use the particle effective ratio to compute the 0 member value

	template<typename BOX_TYPE>
	BOX_TYPE MakeBoxMatchAspect(BOX_TYPE const& src, typename BOX_TYPE const& aspect_box, bool prefere_update_with)
	{
		// the reference box that gives the aspect may not be empty
		assert(!IsGeometryEmpty(aspect_box));

		if (IsGeometryEmpty(src))
			return aspect_box;








	}

















	// ==============================================================================================
	// box functions
	// ==============================================================================================

	/** equality function for box */
	template<typename T, int dimension>
	bool operator == (type_box<T, dimension> const& b1, type_box<T, dimension> const& b2)
	{
		return (b1.position == b1.position) && (b1.half_size == b2.half_size);
	}

	/** difference function for box */
	template<typename T, int dimension>
	bool operator != (type_box<T, dimension> const& b1, type_box<T, dimension> const& b2)
	{
		return !(b1 == b2);
	}

	/** intersection of 2 boxes */
	template<typename T, int dimension>
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
	template<typename T, int dimension>
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
	template<typename T>
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
	template<typename T>
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
	template<typename T, int dimension>
	auto GetBoxCorners(type_box<T, dimension> const& b) // returns a std::pair<vec_type, vec_type>
	{
		if (!IsGeometryEmpty(b))
			return std::make_pair(b.position - b.half_size, b.position + b.half_size);
		return std::make_pair(b.position, b.position);
	}

	/** increase the box size with a single vertex */
	template<typename T, int dimension>
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

	template<typename T>
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

	template<typename T>
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
	glm::vec4 EncodeBoxToVector(box2 const& src);

	// ==============================================================================================
	// obox functions
	// ==============================================================================================

	/** equality function for obox */
	template<typename T, int dimension>
	bool operator == (type_obox<T, dimension> const& b1, type_obox<T, dimension> const& b2)
	{
		return (b1.position == b1.position) && (b1.half_size == b2.half_size) && (b1.rotator == b2.rotator);
	}

	/** difference function for obox */
	template<typename T, int dimension>
	bool operator != (type_obox<T, dimension> const& b1, type_obox<T, dimension> const& b2)
	{
		return !(b1 == b2);
	}






	// shu49. Ca vaut le coup de savoir dans quel ordre on genere ces vertex

	//  0    1                    0    1  
	//                    ou
	//  2    3                    3    2
	//  
	// histoire de faire du rendu plus facilement





	template<typename T>
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

	template<typename T>
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

	template<typename T>
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

	template<typename T>
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
	// triangles functions
	// ==============================================================================================

	/** equality test function for triangles */
	template<typename T, int dimension>
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

	/** difference test function for triangle */
	template<typename T, int dimension>
	bool operator != (type_triangle<T, dimension> const& t1, type_triangle<T, dimension> const& t2)
	{
		return !(t1 == t2);
	}

	/** returns true whether the triangle is empty */
	template<typename T, int dimension>
	bool IsGeometryEmpty(type_triangle<T, dimension> const& t)
	{
		if (t.a == t.b || t.a == t.c || t.b == t.c)
			return true;
		return false;
	}

	/** get the reversed triangle */
	template<typename T, int dimension>
	type_triangle<T, dimension> GetInvertedTriangle(type_triangle<T, dimension> const& t)
	{
		return type_triangle<T, dimension>(t.a, t.c, t.b);
	}

	// ==============================================================================================
	// ray functions
	// ==============================================================================================

	/** equality function for ray */
	template<typename T, int dimension>
	bool operator == (type_ray<T, dimension> const& r1, type_ray<T, dimension> const& r2)
	{
		return (r1.position == r2.position) && (r1.direction == r2.direction);
	}

	/** difference function for ray */
	template<typename T, int dimension>
	bool operator != (type_ray<T, dimension> const& r1, type_ray<T, dimension> const& r2)
	{
		return !(r1 == r2);
	}

	// ==============================================================================================
	// sphere/circle functions
	// ==============================================================================================

	/** returns true whether the circle is empty */
	template<typename T, int dimension>
	bool IsGeometryEmpty(type_sphere<T, dimension> const& c)
	{
		return (c.radius < 0);
	}

	/** equality function for circle */
	template<typename T, int dimension>
	bool operator == (type_sphere<T, dimension> const& c1, type_sphere<T, dimension> const& c2)
	{
		return (c1.position == c1.position) && (c1.radius == c2.radius);
	}

	/** difference function for circle */
	template<typename T, int dimension>
	bool operator != (type_sphere<T, dimension> const& c1, type_sphere<T, dimension> const& c2)
	{
		return !(c1 == c2);
	}

	/** returns the perimeter of the circle */
	template<typename T>
	T GetPerimeter(type_sphere<T, 2> const& c)
	{
		return static_cast<T>(2.0 * M_PI) * c.radius;
	}

	/** returns the surface of the circle */
	template<typename T>
	T GetSurface(type_sphere<T, 2> const& c)
	{
		return static_cast<T>(M_PI) * c.radius * c.radius;
	}

	/** returns the volume of the sphere */
	template<typename T>
	T GetVolume(type_sphere<T, 3> const& s)
	{
		return static_cast<T>((4.0 / 3.0) * M_PI) * s.radius * s.radius * s.radius;
	}

	/** returns the surface of the sphere */
	template<typename T>
	T GetSurface(type_sphere<T, 3> const& s)
	{
		return static_cast<T>(4.0 * M_PI) * s.radius * s.radius;
	}

	/** returns the bounding box of the circle */
	template<typename T>
	type_box<T, 2> GetBoundingBox(type_sphere<T, 2> const& c)
	{
		using vec_type = typename type_sphere<T, 2>::vec_type;

		return IsGeometryEmpty(c) ? type_box<T, 2>() : type_box<T, 2>(c.position, vec_type(c.radius, c.radius));
	}

	/** returns the bounding box of the circle (square) */
	template<typename T>
	type_box<T, 2> GetInnerBox(type_sphere<T, 2> const& c)
	{
		using vec_type = typename type_sphere<T, 2>::vec_type;

		static double const INV_SQRT2 = 0.707106781186547; /* 1.0 / sqrtf(2.0) */

		return IsGeometryEmpty(c) ? type_box<T, 2>() : type_box<T, 2>(c.position, vec_type(c.radius * static_cast<T>(INV_SQRT2)));
	}

	template<typename T>
	type_box<T, 3> GetBoundingBox(type_sphere<T, 3> const& s)
	{
		using vec_type = typename type_sphere<T, 3>::vec_type;

		return IsGeometryEmpty(s) ? type_box<T, 3>() : type_box<T, 3>(s.position, vec_type(s.radius));
	}

	template<typename T>
	type_box<T, 3> GetInnerBox(type_sphere<T, 3> const& s)
	{
		using vec_type = typename type_sphere<T, 3>::vec_type;

		static double const INV_SQRT3 = 0.577350269189625; /* 1.0 / sqrtf(3.0) */

		return IsGeometryEmpty(s) ? type_box<T, 3>() : type_box<T, 3>(s.position, vec_type(s.radius * static_cast<T>(INV_SQRT3)));
	}

	/** returns intersection of 2 spheres */
	template<typename T, int dimension>
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
	template<typename T, int dimension>
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

		// vectors

	template<typename T, glm::precision P>
	bool SaveIntoJSON(nlohmann::json& json, glm::tvec2<T, P> const& src)
	{
		if (!json.is_array())
			json = nlohmann::json::array();
		JSONTools::SetAttributeByIndex(json, 0, src.x);
		JSONTools::SetAttributeByIndex(json, 1, src.y);
		return true;
	}

	template<typename T, glm::precision P>
	bool LoadFromJSON(nlohmann::json const& json, glm::tvec2<T, P>& dst)
	{
		if (json.is_object())
		{
			JSONTools::GetAttribute(json, "x", dst.x);
			JSONTools::GetAttribute(json, "y", dst.y);
			return true;
		}
		else if (json.is_array())
		{
			size_t count = std::min(json.size(), (size_t)dst.length());
			for (size_t i = 0; i < count; ++i)
				dst[i] = json[i].get<T>();
			return true;
		}
		return false;
	}

	template<typename T, glm::precision P>
	bool SaveIntoJSON(nlohmann::json& json, glm::tvec3<T, P> const& src)
	{
		if (!json.is_array())
			json = nlohmann::json::array();
		JSONTools::SetAttributeByIndex(json, 0, src.x);
		JSONTools::SetAttributeByIndex(json, 1, src.y);
		JSONTools::SetAttributeByIndex(json, 2, src.z);
		return true;
	}

	template<typename T, glm::precision P>
	bool LoadFromJSON(nlohmann::json const& json, glm::tvec3<T, P>& dst)
	{
		if (json.is_object())
		{
			JSONTools::GetAttribute(json, "x", dst.x);
			JSONTools::GetAttribute(json, "y", dst.y);
			JSONTools::GetAttribute(json, "z", dst.z);
			return true;
		}
		else if (json.is_array())
		{
			size_t count = std::min(json.size(), (size_t)dst.length());
			for (size_t i = 0; i < count; ++i)
				dst[i] = json[i].get<T>();
			return true;
		}
		return false;
	}

	template<typename T, glm::precision P>
	bool SaveIntoJSON(nlohmann::json& json, glm::tvec4<T, P> const& src)
	{
		if (!json.is_array())
			json = nlohmann::json::array();
		JSONTools::SetAttributeByIndex(json, 0, src.x);
		JSONTools::SetAttributeByIndex(json, 1, src.y);
		JSONTools::SetAttributeByIndex(json, 2, src.z);
		JSONTools::SetAttributeByIndex(json, 3, src.w);
		return true;
	}

	template<typename T, glm::precision P>
	bool LoadFromJSON(nlohmann::json const& json, glm::tvec4<T, P>& dst)
	{
		if (json.is_object())
		{
			JSONTools::GetAttribute(json, "x", dst.x);
			JSONTools::GetAttribute(json, "y", dst.y);
			JSONTools::GetAttribute(json, "z", dst.z);
			JSONTools::GetAttribute(json, "w", dst.w);
			return true;
		}
		else if (json.is_array())
		{
			size_t count = std::min(json.size(), (size_t)dst.length());
			for (size_t i = 0; i < count; ++i)
				dst[i] = json[i].get<T>();
			return true;
		}
		return false;
	}

	template<typename T, int dimension>
	bool SaveIntoJSON(nlohmann::json& json, type_box<T, dimension> const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "half_size", src.half_size);
		return true;
	}

	template<typename T, int dimension>
	bool LoadFromJSON(nlohmann::json const& json, type_box<T, dimension>& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "position", dst.position);
		JSONTools::GetAttribute(json, "half_size", dst.half_size);
		return true;
	}

	template<typename T, int dimension>
	bool SaveIntoJSON(nlohmann::json& json, type_obox<T, dimension> const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "half_size", src.half_size);
		JSONTools::SetAttribute(json, "rotation", src.rotation);
		return true;
	}

	template<typename T, int dimension>
	bool LoadFromJSON(nlohmann::json const& json, type_obox<T, dimension>& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "position", dst.position);
		JSONTools::GetAttribute(json, "half_size", dst.half_size);
		JSONTools::GetAttribute(json, "rotation", dst.rotation);
		return true;
	}


	template<typename T, int dimension>
	bool SaveIntoJSON(nlohmann::json& json, type_sphere<T, dimension> const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "radius", src.radius);
		return true;
	}

	template<typename T, int dimension>
	bool LoadFromJSON(nlohmann::json const& json, type_sphere<T, dimension>& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "position", dst.position);
		JSONTools::GetAttribute(json, "radius", dst.radius);
		return true;
	}

	template<typename T, int dimension>
	bool SaveIntoJSON(nlohmann::json& json, type_ray<T, dimension> const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "direction", src.direction);
		return true;
	}

	template<typename T, int dimension>
	bool LoadFromJSON(nlohmann::json const& json, type_ray<T, dimension>& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "position", dst.position);
		JSONTools::GetAttribute(json, "direction", dst.direction);
		return true;
	}

#endif

}; // namespace chaos