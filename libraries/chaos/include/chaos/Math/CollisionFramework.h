namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class CollisionType;

	template<typename T, int dimension>
	class RayConvexGeometryIntersectionResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==============================================================================================
	// CollisionType : kind of collision event that can happen
	// ==============================================================================================

	enum class CHAOS_API CollisionType : int
	{
		FINISHED = 0,
		STARTED = 1,
		AGAIN = 2
	};

	// ==============================================================================================
	// velocity functions
	// ==============================================================================================

	/** update the velocity by comparing position before and after collision */
	template<typename T>
	/*CHAOS_API*/ void UpdateVelocityFromCollision(T const& old_position, T const& new_position, T& velocity)
	{
		size_t dimension = velocity.length();

		for (size_t i = 0; i < dimension; ++i)
		{
			if (old_position[i] > new_position[i])
				velocity[i] = -abs(velocity[i]);
			else if (old_position[i] < new_position[i])
				velocity[i] = abs(velocity[i]);
		}
	}

	// ==============================================================================================
	// Restriction functions
	// ==============================================================================================

	template<typename T, int dimension>
	bool IsPointInside(typename type_box<T, dimension>::vec_type const& pt, type_box<T, dimension> const& b, bool open_geometry = false)
	{
		if (IsGeometryEmpty(b))
			return false;

		if (open_geometry)
		{
			return
				glm::all(glm::lessThanEqual(pt, b.position + b.half_size)) &&
				glm::all(glm::greaterThanEqual(pt, b.position - b.half_size));
		}
		else
		{
			return
				glm::all(glm::lessThan(pt, b.position + b.half_size)) &&
				glm::all(glm::greaterThan(pt, b.position - b.half_size));
		}
	}

	template<typename T, int dimension>
	bool IsPointInside(typename type_aabox<T, dimension>::vec_type const& pt, type_aabox<T, dimension> const& b, bool open_geometry = false)
	{
		if (IsGeometryEmpty(b))
			return false;

		if (open_geometry)
		{
			return
				glm::all(glm::lessThanEqual(pt, b.position + b.size)) &&
				glm::all(glm::greaterThanEqual(pt, b.position));
		}
		else
		{
			return
				glm::any(glm::lessThan(pt, b.position + b.size)) &&
				glm::any(glm::greaterThan(pt, b.position));
		}
	}


	template<typename T, int dimension>
	bool IsPointInside(typename type_obox<T, dimension>::vec_type const& pt, type_obox<T, dimension> const& b, bool open_geometry = false)
	{


		assert(0); // shuxxx todo
		return false;
	}

	template<typename T, int dimension>
	bool IsPointInside(typename type_sphere<T, dimension>::vec_type const& pt, type_sphere<T, dimension> const& s, bool open_geometry = false)
	{
		if (IsGeometryEmpty(s))
			return false;
		if (open_geometry)
			return glm::distance2(pt, s.position) < s.radius * s.radius;
		else
			return glm::distance2(pt, s.position) <= s.radius * s.radius;
	}



	// ==============================================================================================
	// Restriction functions
	// ==============================================================================================

	template<typename T, int dimension>
	/*CHAOS_API*/ auto GetClosestPoint(type_box<T, dimension> const& b, typename type_box<T, dimension>::vec_type const& src)
	{
		// clamp X, Y, Z for all planes
		return glm::min(glm::max(src, b.position - b.half_size), b.position + b.half_size);
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ auto GetClosestPoint(type_obox<T, dimension> const& b, typename type_obox<T, dimension>::vec_type const& src)
	{
		typename type_sphere<T, dimension>::vec_type result;






		return result;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ auto GetClosestPoint(type_sphere<T, dimension> const& b, typename type_sphere<T, dimension>::vec_type const& src)
	{
		typename type_sphere<T, dimension>::vec_type result;











		return result;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ auto GetClosestPoint(type_triangle<T, dimension> const& b, typename type_sphere<T, dimension>::vec_type const& src)
	{
		typename type_triangle<T, dimension>::vec_type result;








		return result;
	}

	// ==============================================================================================
	// Restriction functions
	// ==============================================================================================

	template<typename T>
	/*CHAOS_API*/ bool MoveValue(T& bigger, T& smaller, bool move_big, T delta)
	{
		if (move_big)
			bigger += delta;
		else
			smaller -= delta;
		return true;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool RestrictToInside(type_box<T, dimension>& bigger, type_box<T, dimension>& smaller, bool move_big)
	{
		bool result = false;

		// if one is empty, nothing to do
		if (IsGeometryEmpty(bigger) || IsGeometryEmpty(smaller))
			return false;

		auto big_corners = GetBoxCorners(bigger);
		auto small_corners = GetBoxCorners(smaller);

		int const count = dimension;
		for (int i = 0; i < count; ++i)
		{
			// degenerated case : smaller ISNOT the smallest
			//  => just make 'smaller' fullfill 'bigger' surface
			//
			//               'smaller'
			//   +----------------------------------+ smaller fullfill fully the bigger surface
			//
			//         +---------------------+
			//                'bigger'

			if (smaller.half_size[i] >= bigger.half_size[i])
			{
				T delta1 = small_corners.first[i] - big_corners.first[i];

				if (delta1 > 0)
					result |= MoveValue(bigger.position[i], smaller.position[i], move_big, delta1);
				else
				{
					T delta2 = small_corners.second[i] - big_corners.second[i];
					if (delta2 < 0)
						result |= MoveValue(bigger.position[i], smaller.position[i], move_big, delta2);
				}
			}
			// normal case : smaller IS the smallest
			//
			//               'smaller'
			//         +---------------------+
			//
			//   +----------------------------------+ smaller fullfill inside the bigger surface
			//                'bigger'
			else
			{
				T delta1 = small_corners.first[i] - big_corners.first[i];
				if (delta1 < 0)
					result |= MoveValue(bigger.position[i], smaller.position[i], move_big, delta1);
				else
				{
					T delta2 = small_corners.second[i] - big_corners.second[i];
					if (delta2 > 0)
						result |= MoveValue(bigger.position[i], smaller.position[i], move_big, delta2);
				}
			}
		}
		return result;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ auto GetRestrictToOutsideDisplacement(type_box<T, dimension> const& src, type_box<T, dimension> const& target, int axis_of_interrests = -1)
	{
		// the null result
		typename type_box<T, dimension>::vec_type result = type_box<T, dimension>::vec_type(0);

		if (IsGeometryEmpty(src) || IsGeometryEmpty(target))
			return result;

		auto src_corners = GetBoxCorners(src);
		auto target_corners = GetBoxCorners(target);

		// compute the minimum distance, and best direction (+X, -X, +Y ...) to move the box
		// (best_direction encode the axis as X = 2 , Y = 4  and the negative/direction as a bitfield)
		T   best_distance = (T)-1;
		int best_direction = -1;
		for (int i = 0; i < dimension; ++i)
		{
			// in negative direction (dist_neg is to be positive)
			T dist_neg = target_corners.second[i] - src_corners.first[i];
			if (dist_neg <= 0)
				return result; // separating plane -> no collision -> exit

			int negative_flag = (1 << (i * 2 + 0));
			if ((axis_of_interrests & negative_flag) != 0)
			{

				if (best_distance < 0 || dist_neg < best_distance)
				{
					best_distance = dist_neg;
					best_direction = 2 * i + 0;
				}
			}

			// in positive direction (dist_pos is to be positive)
			T dist_pos = src_corners.second[i] - target_corners.first[i];
			if (dist_pos <= 0)
				return result; // separating plane -> no collision -> exit

			int positive_flag = (1 << (i * 2 + 1));
			if ((axis_of_interrests & positive_flag) != 0)
			{
				if (best_distance < 0 || dist_pos < best_distance)
				{
					best_distance = dist_pos;
					best_direction = 2 * i + 1;
				}
			}
		}

		// do the displacement
		if (best_direction >= 0)
		{
			if ((best_direction & 1) == 0)
				result[best_direction / 2] = -best_distance; // negative direction
			else
				result[best_direction / 2] = +best_distance; // positive direction
		}
		return result;
	}


	template<typename T, int dimension>
	/*CHAOS_API*/ bool RestrictToOutside(type_box<T, dimension>& src, type_box<T, dimension>& target, int axis_of_interrests = -1)
	{
		auto delta_position = GetRestrictToOutsideDisplacement(src, target, axis_of_interrests);
		if (delta_position == type_box<T, dimension>::vec_type(0))
			return false;
		target.position += delta_position;
		return true;
	}















	template<typename T, int dimension>
	/*CHAOS_API*/ bool RestrictToInside(type_sphere<T, dimension>& bigger, type_sphere<T, dimension>& smaller, bool move_big)
	{
		// if one is empty, nothing to do
		if (IsGeometryEmpty(bigger) || IsGeometryEmpty(smaller))
			return false;

		// ensure smaller and bigger are coherent
		if (smaller.radius > bigger.radius)
			return false;

		auto delta_pos = smaller.position - bigger.position;
		T    l = glm::length(delta_pos);

		if (l + smaller.radius <= bigger.radius) // smaller fully inside bigger
			return false;

		T factor = ((bigger.radius - smaller.radius) / l);

		delta_pos = delta_pos * factor;

		if (move_big)
			bigger.position = smaller.position - delta_pos;
		else
			smaller.position = bigger.position + delta_pos;
		return true;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool RestrictToOutside(type_sphere<T, dimension>& src, type_sphere<T, dimension>& target)
	{
		if (IsGeometryEmpty(src) || IsGeometryEmpty(target))
			return false;

		auto delta_pos = target.position - src.position;
		T    l = glm::length(delta_pos);

		if (l - target.radius >= src.radius)
			return false;

		if (l != 0)
		{
			T factor = ((src.radius + target.radius) / l);
			delta_pos = delta_pos * factor;
		}
		else
		{
			auto new_direction = type_sphere<T, dimension>::vec_type(0);
			new_direction[0] = (T)1;

			T factor = (src.radius + target.radius);
			delta_pos = new_direction * factor;
		}

		target.position = src.position + delta_pos;

		return true;
	}

	// ==============================================================================================
	// ray intersections
	// ==============================================================================================



	// shu49 le passage de parametre est pourri. pourquoi pas directement type_plane et type_vec


	template<typename T, int dimension>
	/*CHAOS_API*/ int GetIntersection(type_ray<T, dimension> const& r, typename type_ray<T, dimension>::plane_type const& p, typename type_ray<T, dimension>::vec_type& res)
	{
		using vec_type = typename type_ray<T, dimension>::vec_type;

		auto normal = GetPlaneNormal(p);

		// no solution ?
		auto denum = glm::dot(r.direction, normal);
		if (denum == (T)0)
			return false;

		auto t = (-GetPlaneOffset(p) - glm::dot(r.position, normal)) / denum;

		res = r.position + t * r.direction;

		return 1;
	}




	template<typename T, int dimension>
	/*CHAOS_API*/ int GetIntersection(type_ray<T, dimension> const& r, type_triangle<T, dimension> const& t, typename type_ray<T, dimension>::vec_type& res)
	{



		return 0;
	}

	// Ray/Sphere intersection
	//
	// intersection point P is on the circle (C, r)
	// intersection point P is on the ray (A, dir)
	// 
	// P = A + dir.t
	// dist2(P, C) = r^2
	//
	// ----------------------------------------------------------
	// dist2(A + dir.t, C) = r^2
	// ----------------------------------------------------------
	// [(A.x + dir.x t) - C.x]^2 + [(A.y + dir.y t) - C.y]^2 + [(A.y + dir.y t) - C.y]^2 = r^2
	// ----------------------------------------------------------
	// [(A.x + dir.x t) - C.x]^2 + [(A.y + dir.y t) - C.y]^2 + [(A.y + dir.y t) - C.y]^2 = r^2
	// ----------------------------------------------------------
	// [(A.x - C.x) + dir.x t]^2 + [(A.y - C.y) + dir.x t]^2 +[(A.z - C.z) + dir.z t]^2  = r^2
	// ----------------------------------------------------------
	// size2(AC) + 2.t.dot(AC, dir) + t^2.size2(dir) - r^2 = 0

	template<typename T, int dimension>
	class RayConvexGeometryIntersectionResult
	{
	public:

		/** information concerning an intersection */
		class IntersectionInfo
		{
		public:

			/** position of the solution */
			glm::vec<dimension, T> position = glm::vec<dimension, T>(T(0));
			/** distance along the ray for that solution */
			float t = 0.0f;
		};

	public:

		/** returns whether there is at least one intersection point */
		operator bool() const
		{
			return (count > 0);
		}

		/** add an intersection to the solution */
		void AddIntersectionPoint(float t, glm::vec<dimension, T> const& position)
		{
			assert(count < 2);
			info[count++] = { position, t};
		}

		/** remove all intersections in opposite direction of the ray */
		RayConvexGeometryIntersectionResult FilterPositiveIntersectionOnly() const
		{
			RayConvexGeometryIntersectionResult result;
			for (int i = 0; i < count ; ++i)
				if (info[i]->t >= 0.0f)
					result.AddIntersectionPoint(info[i]->t, info[i]->position);
			return result;
		}

		/** keep only all intersections in opposite direction of the ray */
		RayConvexGeometryIntersectionResult FilterNegativeIntersectionOnly() const
		{
			RayConvexGeometryIntersectionResult result;
			for (int i = 0; i < count ; ++i)
				if (info[i]->t <= 0.0f)
					result.AddIntersectionPoint(info[i]->t, info[i]->position);
			return result;
		}

		/** get number of intersection */
		int GetIntersectionCount() const
		{
			return count;
		}

		/** get the intersection info */
		IntersectionInfo const & operator [](size_t index) const
		{
			assert(index <= 1);
			assert(index < count);
			return info[index].value();
		}

	public:

		/** data for each intersection */
		std::optional<IntersectionInfo> info[2];
		/** number of intersections */
		int count = 0;
	};

	template<typename T, int dimension>
	/*CHAOS_API*/ RayConvexGeometryIntersectionResult<T, dimension> GetIntersection(type_ray<T, dimension> const& r, type_sphere<T, dimension> const& s)
	{
		RayConvexGeometryIntersectionResult<T, dimension> result;

		auto AC = r.position - s.position;

		T a = glm::length2(r.direction);
		T b = T(2) * glm::dot(AC, r.direction);
		T c = glm::length2(AC) - s.radius * s.radius;

		MathTools::Polynome2Solution<T> solutions = MathTools::SolvePolynome2(a, b, c);
		for (int i = 0; i < solutions.count; ++i)
			result.AddIntersectionPoint(solutions.solutions[i], r.position + r.direction * solutions.solutions[i]);
 
		return result;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ int GetIntersection(type_ray<T, dimension> const& r, type_box<T, dimension> const& b, typename type_ray<T, dimension>::vec_type& res1, typename type_ray<T, dimension>::vec_type& res2)
	{



		return 0;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ int GetIntersection(type_ray<T, dimension> const& r, type_obox<T, dimension> const& b, typename type_ray<T, dimension>::vec_type& res1, typename type_ray<T, dimension>::vec_type& res2)
	{



		return 0;
	}

	// ==============================================================================================
	// Symetric Collisions
	// ==============================================================================================

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_box<T, dimension> const& src1, type_box<T, dimension> const& src2, bool open_geometry = false)
	{
		if (IsGeometryEmpty(src1) || IsGeometryEmpty(src2))
			return false;

		auto src1_corners = GetBoxCorners(src1);
		auto src2_corners = GetBoxCorners(src2);

		if (open_geometry)
		{
			if (glm::any(glm::lessThanEqual(src1_corners.second, src2_corners.first)))
				return false;
			if (glm::any(glm::greaterThanEqual(src1_corners.first, src2_corners.second)))
				return false;
		}
		else
		{
			if (glm::any(glm::lessThan(src1_corners.second, src2_corners.first)))
				return false;
			if (glm::any(glm::greaterThan(src1_corners.first, src2_corners.second)))
				return false;
		}
		return true;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_sphere<T, dimension> const& src1, type_sphere<T, dimension> const& src2, bool open_geometry = false)
	{
		if (IsGeometryEmpty(src1) || IsGeometryEmpty(src2))
			return false;

		if (open_geometry)
			return glm::distance2(src1.position, src2.position) < MathTools::Square(src1.radius + src2.radius);
		else
			return glm::distance2(src1.position, src2.position) <= MathTools::Square(src1.radius + src2.radius);
	}

	template<typename T, typename TRANSFORM = EmptyClass>
	/*CHAOS_API*/ bool IsSeparatingPlane(glm::tvec2<T> const& a, glm::tvec2<T> const& b, glm::tvec2<T>* vertices, size_t count, TRANSFORM transform = {})
	{
		bool has_positive = false;
		bool has_negative = false;

		auto ab = b - a;

		for (size_t i = 0; i < count; ++i)
		{
			auto vert = vertices[i];
			if constexpr (!std::is_same_v<TRANSFORM, EmptyClass>)
				vert = GLMTools::MultWithTranslation(transform, vert);

			// on which side of BA is the considered point ?
			auto z = GLMTools::Get2DCrossProductZ(ab, vert - a);
			if (z == 0)
				continue;
			if (z < 0)
			{
				if (has_positive)
					return false;
				has_negative = true;
			}
			else
			{
				if (has_negative)
					return false;
				has_positive = true;
			}
		}
		return true;
	}

	// SEARCH SEPARATING PLANE
	//
	//         4 = (1 << 2)
	//               8 = (1 << 3)
	//
	//    5       1      9
	//         |     |
	//  -------+-----+--------     1 = (1 << 0)
	//         |     |
	//    4    |  0  |   8
	//         |     |
	//  -------+-----+--------     2 = (1 << 1)
	//         |     |
	//    6       2      10
	//
	//

	// shu47 : utiliser open_geometry

	template<typename T, typename TRANSFORM = EmptyClass>
	/*CHAOS_API*/ bool HasSeparatingPlane(type_box<T, 2> const& b, typename type_box<T, 2>::vec_type const* vertices, size_t count, bool open_geometry = false, TRANSFORM transform = {})
	{
		if (count == 0 || vertices == nullptr)
			return false;

		// the edge we are interresting in
		int edge_candidates = 1 | 2 | 4 | 8;

		// for each edge, we describe the tests to do
		int   edge_component[] = { 0, 0, 1 , 1 };
		float edge_multiplier[] = { -1.0f, 1.0f, -1.0f , 1.0f };
		float edge_value[] = { b.position.x - b.half_size.x, b.position.x + b.half_size.x, b.position.y - b.half_size.y, b.position.y + b.half_size.y };

		// iterate over all vertices and eliminate some edges
		for (size_t i = 0; i < count; ++i)
		{
			auto vert = vertices[i];
			if constexpr (!std::is_same_v<TRANSFORM, EmptyClass>)
				vert = GLMTools::MultWithTranslation(transform, vert);

			// iterate over all remaining edges (work on a copy so we can iterate)
			int edges = edge_candidates;
			while (edges != 0)
			{
				// get the edge of interrest and remove it from current iteration
				int edge_index = BitTools::bsf(edges);
				edges &= ~(1 << edge_index);

				int   c = edge_component[edge_index];
				float m = edge_multiplier[edge_index];
				float v = edge_value[edge_index];

				// is edge still candidate => remove opposite edge (for both global flags and current iteration, just in case it has not been checked yet)
				if (m * vert[c] > m * v)
				{
					edge_candidates &= ~(1 << (edge_index ^ 1));
					edges &= ~(1 << (edge_index ^ 1));
				}
				// edge is not a good candidate : remove it (because opposite edges are parallel)
				else
				{
					edge_candidates &= ~(1 << edge_index);
				}
			}
			// no possible edge separator ?
			if (edge_candidates == 0)
				return false;
		}
		return true; // there are still at least one separator edge
	}

	template<typename T>
	/*CHAOS_API*/ bool Collide(type_obox<T, 2> const& src1, type_obox<T, 2> const& src2, bool open_geometry = false)
	{
		if (IsGeometryEmpty(src1) || IsGeometryEmpty(src2))
			return false;

		// search separate box for one to the other (and the inverse)
		type_obox<T, 2> const* sources[] = { &src1, &src2 };
		for (int i = 0; i < 2; ++i)
		{
			// get transformation matrix to go the one source axis aligned system
			type_box<T, 2> b;
			b.position = { 0, 0 };
			b.half_size = sources[i]->half_size;

			auto transform = GetRotatorMatrix(-sources[i]->rotator) * glm::translate(glm::tvec3<T>(-sources[i]->position, 0)); // world => local BOX

			// get vertices of the other source and search a separating plane
			glm::tvec2<T> vertices[4];
			GetBoxVertices(*sources[1 - i], vertices, true);
			if (HasSeparatingPlane(b, vertices, 4, open_geometry, transform))
				return false;
		}
		// no separating plane
		return true;
	}


	template<typename T>
	/*CHAOS_API*/ bool Collide(type_obox<T, 3> const& src1, type_obox<T, 3> const& src2, bool open_geometry = false)
	{
		if (IsGeometryEmpty(src1) || IsGeometryEmpty(src2))
			return false;





		return false;
	}


	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_triangle<T, dimension> const& src1, type_triangle<T, dimension> const& src2, bool open_geometry = false)
	{







		return false; // TODO COLLISION
	}

	// ==============================================================================================
	// Collisions sphere/box
	// ==============================================================================================

#if 0

	template<typename T>
	/*CHAOS_API*/ bool Collide(type_box<T, 2> const& b, type_sphere<T, 2> const& s, bool open_geometry = false)
	{
		using box_type = type_box<T, 2>;
		using vec_type = box_type::vec2_type;

		// 1 : test whether any entry is null (sphere is faster call, first)
		if (IsGeometryEmpty(s))
			return false;
		if (IsGeometryEmpty(b))
			return false;

		// 2 : test whether the center of the sphere is inside the box
		if (Collide(s.position, b))
			return true;

		// 3 : test distance from the sphere center to the 4 corners of the box
		auto r2 = s.radius * s.radius;

		auto corners = GetBoxCorners(b);
		auto const& A = corners.first;
		auto const& C = corners.second;

		vec_type V[4] =
		{
			A,
			vec_type(A.x, C.y),
			C,
			vec_type(C.x, A.y)
		};

		for (int i = 0; i < 4; ++i)
		{
			T dist = glm::distance2(V[i], s.position);
			if (dist <= r2)
				return true;
		}

		// 4 : test whether sphere center is too far from any edge
		if (s.position.x > b.position.x + b.half_size.x + s.radius)
			return false;
		if (s.position.x < b.position.x - b.half_size.x - s.radius)
			return false;
		if (s.position.y > b.position.y + b.half_size.y + s.radius)
			return false;
		if (s.position.y < b.position.y - b.half_size.y - s.radius)
			return false;
		return true;
	}

#endif


	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_box<T, dimension> const& b, type_sphere<T, dimension> const& s, bool open_geometry = false)
	{
		if (IsGeometryEmpty(s) || IsGeometryEmpty(b))
			return false;

		auto pt = GetClosestPoint(b, s.position);

		if (open_geometry)
			return glm::distance2(pt, s.position) < (s.radius * s.radius);
		else
			return glm::distance2(pt, s.position) <= (s.radius * s.radius);
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_sphere<T, dimension> const& s, type_box<T, dimension> const& b, bool open_geometry = false)
	{
		return Collide(b, s, open_geometry);
	}

	// ==============================================================================================
	// Collisions sphere/triangle
	// ==============================================================================================

	// TODO COLLISION       TRIANGLE 3 / SPHERE 3 ???



	template<typename T>
	/*CHAOS_API*/ bool Collide(type_triangle<T, 2> const& t, type_sphere<T, 2> const& s, bool open_geometry = false)
	{
		if (IsGeometryEmpty(s) || IsGeometryEmpty(t))
			return false;

		//	auto pt = GetClosestPoint(t, s.position);
		//	return glm::distance2(pt, s.position) <= (s.radius * s.radius);





#if 1



		auto r2 = s.radius * s.radius;

		// 2 : test whether any of the 3 vertices is inside the sphere
		for (int i = 0; i < 3; ++i)
			if (glm::length2(t[i] - s.position) <= r2) // do not use sphere::Contains(...) to avoid multiple computation of R * R
				return true;

		// 3 : test whether the sphere center is too far from one side of any edge
		//     the distance si given by
		//     length(cross(normalized_edge, e))
		//
		//     we can use a simplified version of cross product while in 2D
		//     => the cross will give a vector so that (X = 0, Y = 0)
		for (int i = 0; i < 3; ++i)
		{
			auto const& e1 = t[i];
			auto const& e2 = t[(i + 1) % 3];

			auto e1_S = s.position - e1;
			auto normalized_edge = glm::normalize(e2 - e1);

			auto d = GLMTools::Get2DCrossProductZ(normalized_edge, e1_S); // cross product, in plane, the only valid coordinate is Z = (x.y') - (x'y)
			if (d > s.radius)
				return false;
		}
#endif
		return true;
	}

	template<typename T>
	/*CHAOS_API*/ bool Collide(type_triangle<T, 3> const& t, type_sphere<T, 3> const& s, bool open_geometry = false)
	{


		return false;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_sphere<T, dimension> const& s, type_triangle<T, dimension> const& t, bool open_geometry = false)
	{
		return Collide(t, s, open_geometry);
	}

	// ==============================================================================================
	// Collisions box/triangle
	// ==============================================================================================

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_box<T, dimension> const& b, type_triangle<T, dimension> const& t, bool open_geometry = false)
	{








		return false; // TODO COLLISION
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_triangle<T, dimension> const& t, type_box<T, dimension> const& b, bool open_geometry = false)
	{
		return Collide(b, t, open_geometry);
	}

	// ==============================================================================================
	// Collisions obox/triangle
	// ==============================================================================================

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_obox<T, dimension> const& b, type_triangle<T, dimension> const& t, bool open_geometry = false)
	{






		return false; // TODO COLLISION
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_triangle<T, dimension> const& t, type_obox<T, dimension> const& b, bool open_geometry = false)
	{
		return Collide(b, t, open_geometry);
	}

	// ==============================================================================================
	// Collisions obox/sphere
	// ==============================================================================================

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_obox<T, dimension> const& b, type_sphere<T, dimension> const& s, bool open_geometry = false)
	{






		return false; // TODO COLLISION
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_sphere<T, dimension> const& s, type_obox<T, dimension> const& b, bool open_geometry = false)
	{
		return Collide(b, s, open_geometry);
	}

	// ==============================================================================================
	// Collision obox/box
	// ==============================================================================================

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_obox<T, dimension> const& b, type_box<T, dimension> const& s, bool open_geometry = false)
	{






		return false; // TODO COLLISION
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_box<T, dimension> const& b, type_obox<T, dimension> const& ob, bool open_geometry = false)
	{
		return Collide(ob, b, open_geometry);
	}

	// ==============================================================================================
	// Collision point/box
	// ==============================================================================================

	/** returns true whether the point is contained in the box */
	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(typename type_box<T, dimension>::vec_type const& pt, type_box<T, dimension> const& b, bool open_geometry = false)
	{
		if (open_geometry)
			return glm::all(glm::lessThan(glm::abs(pt - b.position), b.half_size));
		else
			return glm::all(glm::lessThanEqual(glm::abs(pt - b.position), b.half_size));
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_box<T, dimension> const& b, typename type_box<T, dimension>::vec_type const& pt, bool open_geometry = false)
	{
		return Collide(pt, b, open_geometry);
	}

	// ==============================================================================================
	// Collision point/obox
	// ==============================================================================================

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(typename type_obox<T, dimension>::vec_type const& pt, type_obox<T, dimension> const& b, bool open_geometry = false)
	{
		// set point from global to local system
		auto transform = GetRotatorMatrix(-b.rotator);
		auto transformed_ptr = GLMTools::Mult(transform, pt - b.position);
		// now we can considere we are in a standard BOX
		if (open_geometry)
			return glm::all(glm::lessThan(glm::abs(transformed_ptr), b.half_size));
		else
			return glm::all(glm::lessThanEqual(glm::abs(transformed_ptr), b.half_size));
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_obox<T, dimension> const& b, typename type_obox<T, dimension>::vec_type const& pt, bool open_geometry = false)
	{
		return Collide(pt, b, open_geometry);
	}

	// ==============================================================================================
	// Collision point/triangles
	// ==============================================================================================

	/** returns true whether the point is contained in the triangle */
	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(typename type_triangle<T, dimension>::vec_type const& pt, type_triangle<T, dimension> const& t, bool open_geometry = false)
	{
		using vec_type = typename type_triangle<T, dimension>::vec_type;

		// test whether the triangle is null
		if (IsGeometryEmpty(t))
			return false;

		// test whether the point is inside the edges
		for (int i = 0; i < 3; ++i)
		{
			vec_type const& e1 = t[i];
			vec_type const& e2 = t[(i + 1) % 3];

			vec_type e1_S = pt - e1;
			vec_type normalized_edge = glm::normalize(e2 - e1);

			auto d = GLMTools::Get2DCrossProductZ(normalized_edge, e1_S); // cross product, in plane, the only valid coordinate is Z = (x.y') - (x'y)
			if (d > 0.0f)
				return false;
		}
		return true;
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_triangle<T, dimension> const& t, typename type_triangle<T, dimension>::vec_type const& pt, bool open_geometry = false)
	{
		return Collide(pt, t, open_geometry);
	}

	// ==============================================================================================
	// Collision point/sphere
	// ==============================================================================================

	/** returns true whether the point is contained in the sphere */
	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(typename type_sphere<T, dimension>::vec_type const& pt, type_sphere<T, dimension> const& s, bool open_geometry = false)
	{
		if (open_geometry)
			return glm::distance2(pt, s.position) < (s.radius * s.radius);
		else
			return glm::distance2(pt, s.position) <= (s.radius * s.radius);
	}

	template<typename T, int dimension>
	/*CHAOS_API*/ bool Collide(type_sphere<T, dimension> const& s, typename type_sphere<T, dimension>::vec_type const& pt, bool open_geometry = false)
	{
		return Collide(pt, s, open_geometry);
	}

	// ==============================================================================================
	// Triangle preparation functions
	// ==============================================================================================

	// check the orientation of the triangle for future collision detection
	template<typename T>
	/*CHAOS_API*/ bool IsTriangleReadyForCollision(type_triangle<T, 2> const& t)
	{
		auto e1 = t.b - t.a;
		auto e2 = t.c - t.b;
		if (GLMTools::Get2DCrossProductZ(e2, e1) >= 0)
			return true;
		return false;
	}

	// ensure the orientation of the triangle is correct for further collision detection
	template<typename T>
	/*CHAOS_API*/ type_triangle<T, 2> PrepareTriangleForCollision(type_triangle<T, 2> const& t)
	{
		if (IsTriangleReadyForCollision(t))
			return t;
		return GetInvertedTriangle(t);
	}

#endif

}; // namespace chaos