#ifdef CHAOS_FORWARD_DECLARATION


#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	/** encode a box2 into a vector4 */
	CHAOS_API glm::vec4 EncodeBoxToVector(box2 const& src);




	/** returns the bounding circle for the box */
	template<std::floating_point T>
	sphere<2, T> GetBoundingSphere(box_base<2, T> const& b)
	{
		return IsGeometryEmpty(b) ? sphere<2, T>() : sphere<2, T>(b.position, glm::length(b.half_size));
	}

	/** returns the inner circle for the box */
	template<std::floating_point T>
	sphere<2, T> GetInnerSphere(box_base<2, T> const& b)
	{
		return IsGeometryEmpty(b) ? sphere<2, T>() : sphere<2, T>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the bounding sphere for the box */
	template<std::floating_point T>
	sphere<3, T> GetBoundingSphere(box_base<3, T> const& b)
	{
		return IsGeometryEmpty(b) ? sphere<3, T>() : sphere<3, T>(b.position, glm::length(b.half_size));
	}

	/** returns the inner sphere for the box */
	template<std::floating_point T>
	sphere<3, T> GetInnerSphere(box_base<3, T> const& b)
	{
		return IsGeometryEmpty(b) ? sphere<3, T>() : sphere<3, T>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the "aspect" of the box (width/height) */
	template<std::floating_point T>
	T GetBoxAspect(box_base<2, T> const& b)
	{
		return (b.half_size.y) ? (b.half_size.x / b.half_size.y) : static_cast<T>(1);
	}



	// ==============================================================================================
	// box functions
	// ==============================================================================================





	/** get the corners of the box */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	auto GetBoxCorners(box<DIMENSION, T> const& b) // returns a std::pair<vec_type, vec_type>
	{
		if (!IsGeometryEmpty(b))
			return std::make_pair(b.position - b.half_size, b.position + b.half_size);
		return std::make_pair(b.position, b.position);
	}

	/** increase the box size with a single vertex */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	void ExtendBox(box<DIMENSION, T>& b, typename box<DIMENSION, T>::vec_type const& v)
	{
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
	}

	template<std::floating_point T>
	auto GetBoxVertices(box_base<2, T> const& b, typename box_base<2, T>::vec_type* result, bool global = true) // expect an array of 4 elements
	{
		assert(result != nullptr);

		using geometry_type = geometry<2, T>;
		using vec_type = typename geometry_type::vec_type;

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
	auto GetBoxVertices(box_base<3, T> const& b, typename box_base<3, T>::vec_type* result, bool global = true) // expect an array of 8 elements
	{
		assert(result != nullptr);

		using geometry_type = geometry<3, T>;
		using vec_type = typename geometry_type::vec_type;

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


	/** get planes from a box */
	template<std::floating_point T>
	box_plane<2, T> GetBoxPlanes(box_base<2, T> const& b)
	{
		box_plane<2, T> result;
		result.neg_x = { T(-1),  T(0), -b.half_size.x};
		result.pos_x = { T(+1),  T(0), -b.half_size.x };
		result.neg_y = {  T(0), T(-1), -b.half_size.y};
		result.pos_y = {  T(0), T(+1), -b.half_size.y };
		return result;
	}
	/** get planes from a box */
	template<std::floating_point T>
	box_plane<3, T> GetBoxPlanes(box_base<3, T> const& b)
	{
		box_plane<3, T> result;
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



	// shu49. Ca vaut le coup de savoir dans quel ordre on genere ces vertex

	//  0    1                    0    1
	//                    ou
	//  2    3                    3    2
	//
	// histoire de faire du rendu plus facilement

	template<std::floating_point T>
	auto GetBoxVertices(obox<2, T> const& b, typename box_base<2, T>::vec_type* result, bool global = true) // expect an array of 4 elements
	{
		GetBoxVertices((box_base<2, T> const&)b, result, false); // do not integrate translation because this would produce a wrong rotation/translation combinaison

		if (global)
		{
			auto transform = GetRotationMatrix(b.rotation);
			for (int i = 0; i < 4; ++i)
			{
				result[i] = GLMTools::Mult(transform, result[i]);
				result[i] += b.position;
			}
		}
		return result;
	}

	template<std::floating_point T>
	auto GetBoxVertices(obox<3, T> const& b, typename box_base<3, T>::vec_type* result, bool global = true) // expect an array of 8 elements
	{
		GetBoxVertices((box_base<3, T> const&)b, result, false); // do not integrate translation because this would produce a wrong rotation/translation combinaison

		if (global)
		{
			auto transform = GetRotationMatrix(b.rotation);
			for (int i = 0; i < 8; ++i)
			{
				result[i] = GLMTools::Mult(transform, result[i]);
				result[i] += b.position;
			}
		}
	}

	template<std::floating_point T>
	box<2, T> GetBoundingBox(obox<2, T> const& b)
	{
		box<2, T> result;

		// copy position
		result.position = b.position;
		// get all vertices and compute max values
		typename box<2, T>::vec_type vertices[4];
		GetBoxVertices(b, vertices, false);

		auto max_vec = glm::tvec2<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max());

		auto transform = GetRotationMatrix(b.rotation);
		for (int i = 0; i < 4; ++i)
			max_vec = glm::max(max_vec, GLMTools::Mult(transform, vertices[i]));
		result.half_size = max_vec;

		return result;
	}

	template<std::floating_point T>
	box<3, T> GetBoundingBox(obox<3, T> const& b)
	{
		box<3, T> result;

		// copy position
		result.position = b.position;

		// get all vertices and compute max values
		typename box<3, T>::vec_type vertices[8];
		GetBoxVertices(b, vertices, false);

		auto max_vec = glm::tvec3<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), -std::numeric_limits<T>::max());

		auto transform = GetRotationMatrix(b.rotation);
		for (int i = 0; i < 8; ++i)
			max_vec = glm::max(max_vec, GLMTools::Mult(transform, vertices[i]));
		result.half_size = max_vec;

		return result;
	}



	

	/** returns the bounding box of the circle */
	template<std::floating_point T>
	box<2, T> GetBoundingBox(sphere<2, T> const& c)
	{
		using geometry_type = geometry<2, T>;
		using vec_type = typename geometry_type::vec_type;

		return IsGeometryEmpty(c) ? box<2, T>() : box<2, T>(c.position, vec_type(c.radius, c.radius));
	}

	/** returns the bounding box of the circle (square) */
	template<std::floating_point T>
	box<2, T> GetInnerBox(sphere<2, T> const& c)
	{
		using geometry_type = geometry<2, T>;
		using vec_type = typename geometry_type::vec_type;

		static double const INV_SQRT2 = 0.707106781186547; /* 1.0 / sqrtf(2.0) */

		return IsGeometryEmpty(c) ? box<2, T>() : box<2, T>(c.position, vec_type(c.radius * static_cast<T>(INV_SQRT2)));
	}

	template<std::floating_point T>
	box<3, T> GetBoundingBox(sphere<3, T> const& s)
	{
		using geometry_type = geometry<3, T>;
		using vec_type = typename geometry_type::vec_type;

		return IsGeometryEmpty(s) ? box<3, T>() : box<3, T>(s.position, vec_type(s.radius));
	}

	template<std::floating_point T>
	box<3, T> GetInnerBox(sphere<3, T> const& s)
	{
		using geometry_type = geometry<3, T>;
		using vec_type = typename geometry_type::vec_type;

		static double const INV_SQRT3 = 0.577350269189625; /* 1.0 / sqrtf(3.0) */

		return IsGeometryEmpty(s) ? box<3, T>() : box<3, T>(s.position, vec_type(s.radius * static_cast<T>(INV_SQRT3)));
	}

}; // namespace chaos

#endif